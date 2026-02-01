const API_GAMESTATE_ENDPOINT = '/api/gamestate';
const API_STARTGAME_ENDPOINT = '/api/startgame';
const API_EQUIP_ENDPOINT = '/api/equip';
const POLLING_INTERVAL = 500; 

const startScreen = document.getElementById('start-screen');
const gameScreen = document.getElementById('game-screen');
const startGameButton = document.getElementById('start-game-button');
const startMessageElement = document.getElementById('start-message');
const modal = document.getElementById('equipment-modal');

let gameStateIntervalId = null;
let currentInventory = []; // Stores the inventory list from the backend
let currentSlotOpening = null; // Tracks which slot we are trying to equip

// -- Damage Tracking Logic --
// We track the previous state to calculate differences (damage/healing)
let lastPlayerHp = null;
let lastEnemyHp = null;
let lastEnemyName = null;
let lastEnemyShield = null;

// --- Formatting Helpers ---
function formatStats(stats) {
    let output = '';
    // Create div elements string for the grid
    for (const key in stats) {
        let value = stats[key];
//        if (key === 'ARMOR') value = (value * 100).toFixed(1) + '%';
//        else if (Number.isFinite(value) && Math.abs(value) > 0.01) value = Number(value.toFixed(1));
		if (Number.isFinite(value) && Math.abs(value) > 0.01) value = Number(value.toFixed(1));
		
        
        // Cleanup key names
        let niceKey = key.replace('_', ' ');
        output += `<div>${niceKey}</div><div style="text-align:right; color:#fff;">${value}</div>`;
    }
    return output;
}

// --- Visual Effect: Spawn Floating Text --
function spawnFloatingText(targetElementId, text, type) {
	const target = document.getElementById(targetElementId);
	if (!target) return;
	
	const value = Math.round(text);
	const el = document.createElement('div');
	el.textContent = value;
	
	// Determining type of floating text
	if (type === 'heal') {
		el.className = 'damage-popup heal-text';
	} else if (type === 'shield-damage') {
		el.className = 'damage-popup shield-damage-text';
		el.style.colr = '#00aaff'; // Setting blue-color
	} else {
		el.className = 'damage-popup damage-text';
	}

	// --- SCALING LOGIC FOR DAMAGE NUMBERS ---
	// Base size is 1.5em
	// Going to add 0.05em per point of damage
	// Max cap at 5.0em
	const baseSize = 1.5;
	const sizeMultiplier = Math.min(3.5, value / 20); // Adjust '20' to change sensitivity
	el.style.fontSize = `${baseSize + sizeMultiplier}em`;

	// Randomize position slightly around the center
	const randomX = 38 + (Math.random() * 20 - 10);
	const randomY = 35 + (Math.random() * 20 - 10);

	el.style.left = `${randomX}%`;
	el.style.top = `${randomY}%`;

	target.appendChild(el);

	// Remove after animation finishes
	setTimeout(() => {
		el.remove();
	}, 1000);
}

// --- Main UI Update ---
function updateUI(data) {
    if (data.player.name === "Game Not Started") {
        showStartScreen();
        return;
    }

    showGameScreen();

	// 0. Check for Damage/Heals (Compare previous state vs current)
	// Player Logic side
	if (lastPlayerHp !== null) {
		const diff = data.player.hp - lastPlayerHp;
		if (diff < -0.25) { // Damage (tolerance for float errors)
			// spawnFloatingText('player-panel', Math.abs(diff), 'damage');
		} else if (diff > 0.25) { // Heal
			// spawnFloatingText('player-panel', diff, 'heal');
		}
	}
	lastPlayerHp = data.player.hp;

	// Enemy Logic side
	// If enemy name changed, its a new spawn, so reset tracker instead of showing damage
	if (data.enemy.name !== lastEnemyName) {
		lastEnemyName = data.enemy.name;
		lastEnemyHp = data.enemy.hp;
		lastEnemyShield = data.enemy.shield;
	} else {
		if (lastEnemyHp !== null) {
			const diff = data.enemy.hp - lastEnemyHp;
			if (diff < -0.25) { // Enemy Took Damage
				spawnFloatingText('enemy-panel', Math.abs(diff), 'damage');	
			}
		}

		if (lastEnemyShield !== null) {
			const diff = data.enemy.shield - lastEnemyShield;
			if (diff < -0.25) { // Shield Damage Detected
				spawnFloatingText('enemy-panel', Math.abs(diff), 'shield-damage');
			}
		}

		lastEnemyHp = data.enemy.hp;
		lastEnemyShield = data.enemy.shield;

	}

    // 1. Update Text Stats
    document.getElementById('player-name').textContent = data.player.name;
    document.getElementById('player-stats').innerHTML = formatStats(data.player.stats);
    
    document.getElementById('enemy-name').textContent = data.enemy.name;
    document.getElementById('enemy-is-boss').textContent = data.enemy.is_boss ? '[WARNING: BOSS]' : '';
    document.getElementById('enemy-stats').innerHTML = formatStats(data.enemy.stats || {});

    document.getElementById('current-floor').textContent = data.progress.floor;
    document.getElementById('enemies-defeated').textContent = data.progress.enemies_defeated;

    // 2. Update Progress Bars
    const setBar = (id, current, max) => {
        const pct = Math.max(0, Math.min(100, (current / max) * 100));
        document.getElementById(id).style.width = `${pct}%`;
    };
    setBar('p-hp-bar', data.player.hp, data.player.max_hp);
    setBar('p-shield-bar', data.player.shield, data.player.max_shield);
    setBar('e-hp-bar', data.enemy.hp, data.enemy.max_hp);
    setBar('e-shield-bar', data.enemy.shield, data.enemy.max_shield);

	const player = data.player;
	const expPercent = (player.exp / player.exp_needed) * 100;
	document.getElementById('exp-bar').style.width = expPercent + '%';
	document.getElementById('player-level').innerText = player.level;
	document.getElementById('current-exp').innerText = Math.floor(player.exp);
	document.getElementById('next-level-exp').innerText = player.exp_needed;

    // 3. Update Equipment List (Clickable)
    const equipList = document.getElementById('player-equipment-list');
    equipList.innerHTML = '';
    
    // We iterate a fixed list to keep order consistent
    const slots = ["HEAD", "CHEST", "ARMS", "LEGS", "GENERATOR", "LEFT_ARM_WEAPON", "RIGHT_ARM_WEAPON", "LEFT_SHOULDER_WEAPON", "RIGHT_SHOULDER_WEAPON"];
    
    slots.forEach(slot => {
        const itemStr = data.player.equipment[slot] || "Empty System";
        const div = document.createElement('div');
        div.className = 'equip-slot';
        div.innerHTML = `<span class="slot-label">${slot}</span> <span class="item-name">${itemStr}</span>`;
        div.onclick = () => openEquipModal(slot);
        equipList.appendChild(div);
    });

    // 4. Store Inventory for the Modal
    currentInventory = data.inventory || []; // Ensure we handle empty inventory

    // 5. Update Log
    const logList = document.getElementById('game-log');
    logList.innerHTML = '';
    data.log.forEach(entry => {
        const li = document.createElement('li');
        li.textContent = `> ${entry}`;
        logList.appendChild(li);
    });
    logList.scrollTop = logList.scrollHeight;
}

// --- Modal Logic ---
function openEquipModal(slot) {
    currentSlotOpening = slot;
    document.getElementById('modal-title').innerText = `Equip: ${slot}`;
    modal.style.display = 'block';
    
    const list = document.getElementById('inventory-list');
    list.innerHTML = '';
    
    // Filter inventory to match the slot we clicked
    const relevantItems = currentInventory.filter(item => item.slot === slot);
    
    if (relevantItems.length === 0) {
        list.innerHTML = '<div style="color:#666; text-align:center; padding:20px;">No compatible items in cargo.</div>';
    } else {
        relevantItems.forEach(item => {
            const div = document.createElement('div');
            div.className = 'inventory-item';
            div.innerHTML = `
                <span class="inventory-item-name rarity-${item.rarity}">${item.name}</span>
                <span class="inventory-item-stats">Tech Lvl: ${item.tech || 1}</span>
            `;
            div.onclick = () => equipItem(item.index);
            list.appendChild(div);
        });
    }
}

async function equipItem(index) {
    try {
        const response = await fetch(API_EQUIP_ENDPOINT, {
            method: 'POST',
            body: JSON.stringify({ index: index }),
            headers: { 'Content-Type': 'application/json' }
        });
        
        if(response.ok) {
            modal.style.display = 'none';
            fetchGameState(); // Update immediately
        } else {
            alert("Failed to equip item.");
        }
    } catch (e) {
        console.error(e);
    }
}

// --- Game State Handling ---
async function fetchGameState() {
    try {
        const response = await fetch(API_GAMESTATE_ENDPOINT);
        if (!response.ok) throw new Error(`HTTP ${response.status}`);
        const data = await response.json();
        updateUI(data);
        
        if (data.player.name !== "Game Not Started" && !gameStateIntervalId) {
            startPollingGameState();
        }
    } catch (error) {
        console.error("Fetch error:", error);
    }
}

async function handleStartGame() {
    startGameButton.disabled = true;
    startMessageElement.textContent = 'Initializing...';
    try {
        const response = await fetch(API_STARTGAME_ENDPOINT, { method: 'POST' });
        if (response.ok) {
            showGameScreen();
            fetchGameState();
        } else {
            startMessageElement.textContent = 'Failed to initialize.';
            startGameButton.disabled = false;
        }
    } catch (error) {
        startMessageElement.textContent = error.message;
    }
}

function showStartScreen() {
    startScreen.style.display = 'block';
    gameScreen.style.display = 'none';
}

function showGameScreen() {
    startScreen.style.display = 'none';
    gameScreen.style.display = 'grid';
}

function startPollingGameState() {
    if (gameStateIntervalId) clearInterval(gameStateIntervalId);
    gameStateIntervalId = setInterval(fetchGameState, POLLING_INTERVAL);
}

// Close modal if clicked outside
window.onclick = function(event) {
    if (event.target == modal) {
        modal.style.display = "none";
    }
}

startGameButton.addEventListener('click', handleStartGame);
