const API_ENDPOINT = '/api/gamestate';
const API_STARTGAME_ENDPOINT = '/api/startgame';
const POLLING_INTERVAL = 1000; // Poll every 1 second (1000 ms)

const startScreen = document.getElementById('start-screen');
const gameScreen = document.getElementById('game-screen');
const startGameButton = document.getElementById('start-game-button');
const startMessageElement = document.getElementById('start-message');

let gameStateIntervalId = null // To store the interval ID for polling

// --- UI Update Functions ---

function formatStats(stats) {
    let output = '';
    for (const key in stats) {
        // Format numbers nicely (e.g., armor as percentage)
        let value = stats[key];
        if (key === 'ARMOR') {
            value = (value * 100).toFixed(1) + '%';
        } else if (Number.isFinite(value) && Math.abs(value) > 0.01) {
             // Avoid scientific notation for small numbers, round others
             value = Number(value.toFixed(2));
        }
        output += `${key.padEnd(18)}: ${value}\n`;
    }
    return output;
}

function formatEquipment(equipment) {
    let output = '';
    for (const slot in equipment) {
         output += `${slot.padEnd(25)}: ${equipment[slot] || '(Empty)'}\n`;
    }
     return output;
}


function updateUI(data) {
	if (data.player_name === "Game Not Started") {
		showStartScreen();
		const logList = document.getElementById('game-log'); // Still update log on start screen
		logList.innerHTML = '';
		data.log.forEach(entry => {
			const li = document.createElement('li');
			li.textContent = entry;
			logList.appendChild(li);
		});
		// Ensure game screen elements are cleared or show placeholder if game not started
		document.getElementById('player-name').textContent = '--';
		document.getElementById('player-hp').textContent = '--';
		document.getElementById('player-max-hp').textContent = '--';
		document.getElementById('player-shield').textContent = '--';
		document.getElementById('player-max-shield').textContent = '--';
		document.getElementById('player-stats').textContent = '--';
		document.getElementById('player-equipment').textContent = '--';
		
		return;
	}

	showGameScreen(); // If game is started, ensure game screen is visible

    // Player Panel
    document.getElementById('player-name').textContent = data.player.name;
    document.getElementById('player-hp').textContent = Math.max(0, data.player.hp).toFixed(0);
    document.getElementById('player-max-hp').textContent = data.player.max_hp.toFixed(0);
    document.getElementById('player-shield').textContent = Math.max(0, data.player.shield).toFixed(0);
    document.getElementById('player-max-shield').textContent = data.player.max_shield.toFixed(0);
    document.getElementById('player-stats').textContent = formatStats(data.player.stats);
    document.getElementById('player-equipment').textContent = formatEquipment(data.player.equipment);


    // Enemy Panel
    document.getElementById('enemy-name').textContent = data.enemy.name;
    document.getElementById('enemy-hp').textContent = Math.max(0, data.enemy.hp).toFixed(0);
    document.getElementById('enemy-max-hp').textContent = data.enemy.max_hp.toFixed(0);
    document.getElementById('enemy-shield').textContent = Math.max(0, data.enemy.shield).toFixed(0);
    document.getElementById('enemy-max-shield').textContent = data.enemy.max_shield.toFixed(0);
    document.getElementById('enemy-is-boss').textContent = data.enemy.is_boss ? '[BOSS]' : '';


    // Progress Panel
    document.getElementById('current-floor').textContent = data.progress.floor;
    document.getElementById('enemies-defeated').textContent = data.progress.enemies_defeated;

    // Log Panel
    const logList = document.getElementById('game-log');
    logList.innerHTML = ''; // Clear previous logs
    data.log.forEach(entry => {
        const li = document.createElement('li');
        li.textContent = entry;
        logList.appendChild(li);
    });
     // Auto-scroll to bottom
    logList.scrollTop = logList.scrollHeight;
}

async function fetchGameState() {
    try {
        const response = await fetch(API_GAMESTATE_ENDPOINT);
        if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
        }
        const data = await response.json();
        updateUI(data);

		// If game is now running and wasn't before, make sure polling is active
		if (data.player_name !== "Game Not Started" && !gameStateIntervalId) {
			startPollingGameState(); // Should already be polling if we reach here usually
		}
    } catch (error) {
        console.error("Failed to fetch game state:", error);
        // Maybe display an error message on the page
        const logList = document.getElementById('game-log');
		logList.innerHTML = ''; // Clear
        const li = document.createElement('li');
        li.textContent = `Error fetching data: ${error.message}`;
        li.style.color = 'red';
        logList.appendChild(li);
		showStartScreen(); // If error fetching, might indicate game isn't ready, show start
    }
}

async function handleStartGame() {
	startGameButton.disabled = true;
	startMessageElement.textContent = 'Starting game...';
	try {
		const response = await fetch(API_STARTGAME_ENDPOINT, { method: 'POST' });
		if (response.ok) {
			startMessageElement.textContent = 'Game started! Loading...';
			showGameScreen();
			fetchGameState(); // Fetch state immediately after starting
			startPollingGameState(); // Start regular polling
		}
	} else {
		const errorText = await resposne.text();
		startMessageElement.textContent = `Error starting game: ${error.message}`;
		startGameButton.disabled = false;
	}
}

function showStartScreen() {
	startScreen.style.display = 'block';
	gameScreen.style.display = 'none';
}

function startPollingGameState() {
	if (gameStateIntervalId) {
		clearInterval(gameStateIntervalId); // Clear existing interval if any
	}
	fetchGameState(); // Initial fetch
	gameSateIntervalId = setInterval(fetchGameState, POLLING_INTERVAL);
}

// --- Initialization ---
startGameButton.addEventListener('click', handleStartGame);

startPollingGameState();

// Start polling
//fetchGameState(); // Initial fetch
//setInterval(fetchGameState, POLLING_INTERVAL);
