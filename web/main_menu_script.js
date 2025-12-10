// --- THREE.JS BACKGROUND SETUP ---
let scene, camera, renderer, stars, starGeo;
let mouseX = 0;
let mouseY = 0;
let targetX = 0;
let targetY = 0;

const windowHalfX = window.innerWidth / 2;
const windowHalfY = window.innerHeight / 2;

function init3D() {
	scene = new THREE.Scene();
	// Fog to fade stars in the distance
	scene.fog = new THREE.FogExp2(0x000000, 0.001); 

	camera = new THREE.PerspectiveCamera(60, window.innerWidth / window.innerHeight, 1, 1000);
	camera.position.z = 1;
	camera.rotation.x = Math.PI / 2;

	renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true });
	renderer.setSize(window.innerWidth, window.innerHeight);
	document.getElementById('canvas-container').appendChild(renderer.domElement);

	// Create Starfield using BufferGeometry (Fixed for newer Three.js versions)
	starGeo = new THREE.BufferGeometry();
	const starCount = 6000;
	const positions = new Float32Array(starCount * 3);
	const velocities = []; // Store velocities separately
	const accelerations = []; // Store accelerations separately

	for (let i = 0; i < starCount; i++) {
		// X, Y, Z coordinates
		positions[i * 3] = Math.random() * 600 - 300;     
		positions[i * 3 + 1] = Math.random() * 600 - 300; 
		positions[i * 3 + 2] = Math.random() * 600 - 300; 
		
		velocities.push(0);
		accelerations.push(0.02);
	}

	// Assign position attribute to geometry
	starGeo.setAttribute('position', new THREE.BufferAttribute(positions, 3));
	
	// Store physics data in userData so we can access it in animate loop
	starGeo.userData = { velocities, accelerations };

	// Simple white sprite for stars
	let sprite = new THREE.TextureLoader().load('https://threejs.org/examples/textures/sprites/disc.png');

	let starMaterial = new THREE.PointsMaterial({
		color: 0xaaaaaa,
		size: 0.7,
		map: sprite,
		transparent: true
	});

	stars = new THREE.Points(starGeo, starMaterial);
	scene.add(stars);

	// Event Listeners
	document.addEventListener('mousemove', onDocumentMouseMove, false);
	window.addEventListener('resize', onWindowResize, false);
	
	animate();
}

function onWindowResize() {
	camera.aspect = window.innerWidth / window.innerHeight;
	camera.updateProjectionMatrix();
	renderer.setSize(window.innerWidth, window.innerHeight);
}

function onDocumentMouseMove(event) {
	// Normalize mouse position from -1 to 1
	mouseX = (event.clientX - windowHalfX) * 0.001;
	mouseY = (event.clientY - windowHalfY) * 0.001;
}

function animate() {
	requestAnimationFrame(animate);

	// Smooth camera movement based on mouse
	targetX = mouseX * 0.5; // Sensitivity
	targetY = mouseY * 0.5;

	// Simple lerp for smoothness
	camera.rotation.y += 0.05 * (mouseX - camera.rotation.y);
	camera.rotation.x += 0.05 * (-mouseY - camera.rotation.x);

	// Animate Stars (Warp effect)
	const positions = starGeo.attributes.position.array;
	const velocities = starGeo.userData.velocities;
	const accelerations = starGeo.userData.accelerations;

	for (let i = 0; i < 6000; i++) {
		velocities[i] += accelerations[i];
		
		// Update Y position (index i*3 + 1)
		positions[i * 3 + 1] -= velocities[i]; 
		
		// Reset star if it goes off screen
		if (positions[i * 3 + 1] < -200) {
			positions[i * 3 + 1] = 200;
			velocities[i] = 0;
		}
	}
	
	// Mark position attribute as needing update
	starGeo.attributes.position.needsUpdate = true;
	
	// Rotate the entire star system slowly for dynamism
	stars.rotation.y += 0.002;

	renderer.render(scene, camera);
}

// --- UI LOGIC ---

const mainMenu = document.getElementById('main-menu');
const optionsPanel = document.getElementById('options-panel');
const gameUI = document.getElementById('game-ui');

function switchPanel(showPanel) {
	// Hide all
	mainMenu.classList.add('hidden-panel');
	mainMenu.style.display = 'none';
	
	optionsPanel.classList.add('hidden-panel');
	optionsPanel.style.display = 'none';
	
	gameUI.classList.add('hidden-panel');
	gameUI.style.display = 'none';

	// Show target
	showPanel.style.display = 'flex';
	// Slight delay to allow display:flex to apply before removing opacity class for transition
	setTimeout(() => {
		showPanel.classList.remove('hidden-panel');
	}, 50);
}

function startGame() {
	console.log("Starting game...");
	switchPanel(gameUI);
	
	// Warp speed effect trigger
	stars.material.color.setHex(0x00ffff); // Turn stars cyan
	stars.material.size = 2; // Make them bigger
}

function showOptions() {
	console.log("Opening options...");
	switchPanel(optionsPanel);
}

function backToMenu() {
	console.log("Back to menu...");
	switchPanel(mainMenu);
	
	// Reset warp effect
	stars.material.color.setHex(0xaaaaaa);
	stars.material.size = 0.7;
}

// Initialize
window.onload = () => {
	init3D();
};
