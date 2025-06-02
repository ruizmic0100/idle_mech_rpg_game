const API_ENDPOINT = '/api/gamestate';
const POLLING_INTERVAL = 1000; // Poll every 1 second (1000 ms)

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
        const response = await fetch(API_ENDPOINT);
        if (!response.ok) {
            throw new Error(`HTTP error! Status: ${response.status}`);
        }
        const data = await response.json();
        updateUI(data);
    } catch (error) {
        console.error("Failed to fetch game state:", error);
        // Maybe display an error message on the page
        const logList = document.getElementById('game-log');
        const li = document.createElement('li');
        li.textContent = `Error fetching data: ${error.message}`;
        li.style.color = 'red';
        logList.appendChild(li);
    }
}

// Start polling
fetchGameState(); // Initial fetch
setInterval(fetchGameState, POLLING_INTERVAL);
