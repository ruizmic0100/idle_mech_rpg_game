#include <iostream>
#include <fstream> // For file/json loading
#include <algorithm>
#include <stdexcept> // For std::runtime_error

#include "Game.h"
#include "GameClasses.h"

// Helper for JSON to Enum conversion
StatType stringToStatType(const std::string& s) {
	if (s == "HEALTH") return StatType::HEALTH;
	if (s == "ARMOR") return StatType::ARMOR;
	if (s == "ENERGY_SHIELD") return StatType::ENERGY_SHIELD;
	if (s == "ATTACK") return StatType::ATTACK;
	if (s == "ATTACK_SPEED") return StatType::ATTACK_SPEED;
	if (s == "MOBILITY") return StatType::MOBILITY;
	if (s == "ENERGY") return StatType::ENERGY;
	if (s == "ENERGY_RECOVERY") return StatType::ENERGY_RECOVERY;
	if (s == "REPAIR") return StatType::REPAIR;
	if (s == "TECHNOLOGY") return StatType::TECHNOLOGY;
	
	// Should never get here but incase it does; will throw and error
	throw std::runtime_error("Unknown StatType stirng: " + s);
}

EquipmentSlot stringToEquipmentSlot(const std::string& s) {
	if (s == "HEAD") return EquipmentSlot::HEAD;
	if (s == "CHEST") return EquipmentSlot::CHEST;
	if (s == "ARMS") return EquipmentSlot::ARMS;
	if (s == "LEGS") return EquipmentSlot::LEGS;
	if (s == "GENERATOR") return EquipmentSlot::GENERATOR;
	if (s == "LEFT_ARM_WEAPON") return EquipmentSlot::LEFT_ARM_WEAPON;
	if (s == "RIGHT_ARM_WEAPON") return EquipmentSlot::RIGHT_ARM_WEAPON;
	if (s == "LEFT_SHOULDER_WEAPON") return EquipmentSlot::LEFT_SHOULDER_WEAPON;
	if (s == "RIGHT_SHOULDER_WEAPON") return EquipmentSlot::RIGHT_SHOULDER_WEAPON;
	return EquipmentSlot::NONE; // Default
}

std::string equipmentSlotToString(const EquipmentSlot& s) {
	if (s == EquipmentSlot::HEAD) return "HEAD";
	if (s == EquipmentSlot::CHEST) return "CHEST";
	if (s == EquipmentSlot::ARMS) return "ARMS";
	if (s == EquipmentSlot::LEGS) return "LEGS";
	if (s == EquipmentSlot::GENERATOR) return "GENERATOR";
	if (s == EquipmentSlot::LEFT_ARM_WEAPON) return "LEFT_ARM_WEAPON";
	if (s == EquipmentSlot::RIGHT_ARM_WEAPON) return "RIGHT_ARM_WEAPON";
	if (s == EquipmentSlot::LEFT_SHOULDER_WEAPON) return "LEFT_SHOULDER_WEAPON";
	if (s == EquipmentSlot::RIGHT_SHOULDER_WEAPON) return "RIGHT_SHOULDER_WEAPON";
	return "NONE";
}

Game::Game() : current_floor(1), enemies_defeated_on_floor(0), combat_phase(CombatPhase::IDLE), game_running(false) {

	std::cout << "Game Object constructed!" << std::endl;

	// Initialize player mech with some basic default stats
	Stats player_base_stats = {
		{StatType::HEALTH, 100},
		{StatType::ARMOR, 1},
		{StatType::ENERGY_SHIELD, 1},
		{StatType::ATTACK, 1},
		{StatType::ATTACK_SPEED, 3},
		{StatType::MOBILITY, 1},
		{StatType::ENERGY, 10},
		{StatType::ENERGY_RECOVERY, 1},
		{StatType::REPAIR, 0},
		{StatType::TECHNOLOGY, 1}

	};

	// Add in selected class stuff
       	 	

	player_mech = Mech("Player", player_base_stats);
	std::cout << "`player_mech` initialized with `player_base_stats`." << std::endl;

	// Initialize a test enemy mech with default constructor stats
	Stats test_enemy_stats = { 
		{StatType::HEALTH, 10},
		{StatType::ARMOR, 1},
		{StatType::ENERGY_SHIELD, 1},
		{StatType::ATTACK, 1},
		{StatType::ATTACK_SPEED, 3},
		{StatType::MOBILITY, 1},
		{StatType::ENERGY, 10},
		{StatType::ENERGY_RECOVERY, 1},
		{StatType::REPAIR, 0},
		{StatType::TECHNOLOGY, 1}
	};
	current_enemy = Mech("Test Enemy", test_enemy_stats);
	std::cout << "`current_enemy` initialized with `test_enemy_stats`." << std::endl;

}

Game::~Game() {
	std::cout << "Game Object destructed!" << std::endl;
}

void Game::loadData(const std::string& item_file_path, const std::string& boss_file_path) {
	std::lock_guard<std::mutex> lock(game_state_mutex); // Protect data loading

	// Load Items
	std::ifstream item_fs(item_file_path);
	if (!item_fs.is_open()) {
		throw std::runtime_error("Failed to open item file: " + item_file_path);
	}
	json item_json_data;
	try {
		item_fs >> item_json_data; // Pipe in the item data
	} catch (json::parse_error& e) {
		throw std::runtime_error("Failed to parse item JSON: " + std::string(e.what()));
	}

	item_templates.clear();
	for (const auto& item_entry : item_json_data) {
		auto tpl = std::make_shared<ItemTemplate>();
		tpl->id = item_entry.at("id").get<std::string>();
		tpl->name = item_entry.at("name").get<std::string>();
		tpl->description = item_entry.at("description").get<std::string>();
		tpl->slot = stringToEquipmentSlot(item_entry.at("slot").get<std::string>());
		tpl->required_tech = item_entry.value("required_tech", 0);

		if (item_entry.contains("stats")) {
			for (auto& [stat_key_str, stat_value] : item_entry.at("stats").items()) {
				tpl->base_stats[stringToStatType(stat_key_str)] = stat_value.get<double>();
			}
		}
		std::cout << "\n --- Item Template Added ---" << std::endl;
		std::cout << "ID: " << tpl->id << std::endl;
		std::cout << "NAME: " << tpl->name << std::endl;
		std::cout << "DESCRIPTION: " << tpl->description << std::endl;
		std::cout << "slot: " << equipmentSlotToString(tpl->slot) << std::endl;
		std::cout << "required_tech: " << tpl->required_tech << std::endl;
		item_templates.push_back(tpl);
	}
	std::cout << "Loaded " + std::to_string(item_templates.size()) + " item_templates." << std::endl;
	//std::cout << "item_templates: " << item_json_data.dump(4);

	// Load Bossess
	std::ifstream boss_fs(boss_file_path);
	if (!boss_fs.is_open()) {
		throw std::runtime_error("Failed to open boss file: " + boss_file_path);	
	}
	json boss_json_data;
	try {
		boss_fs >> boss_json_data; // Pipe in the boss data
	} catch (json::parse_error& e) {
		throw std::runtime_error("Failed to parse boss JSON: " + std::string(e.what()));
	}

	boss_data.clear();
	for (auto& [floor_str, boss_entry] : boss_json_data.items()) {
		int floor_num = std::stoi(floor_str);
		BossData bd;
		bd.name = boss_entry.at("name").get<std::string>();
		if (boss_entry.contains("stats")) {
			for (auto& [stat_key_str, stat_value] : boss_entry.at("stats").items()) {
				bd.stats[stringToStatType(stat_key_str)] = stat_value.get<double>();
			}
		}
		boss_data[floor_num] = bd;
	}

	std::cout << "Loaded " + std::to_string(boss_data.size()) + " boss definitions." << std::endl;
	//std::cout << "boss data: " << boss_json_data.dump(4);	
}

bool Game::startGame() {
	std::cerr << "DEBUG: Game::startGame() called." << std::endl;
	std::lock_guard<std::mutex> lock(game_state_mutex); // Protect game_running state
	if (!game_running) {
		game_running = true;
		std::cerr << "DEBUG: Game::startGame() - game_running is SET to true." << std::endl;
		// NOTE(MSR): It's important that game_loop itself doesn't re-spawn the first enemy
		// if it's already set by a previous, stoppedgame. Or reset things here.
		// For a fresh start:
		current_floor = 1;
		enemies_defeated_on_floor = 0;
		combat_phase = CombatPhase::IDLE; // Reset Combat phase
		game_log.clear();
		std::cerr << "DEBUG: Game::StartGame() - Game state reset." << std::endl;

		// WARN(MSR): current_enemy might need to be cleared or reset if a fully fresh start is wanted
		// THE CRITICAL PART: Thread creation
		try {
			game_thread = std::thread(&Game::gameLoop, this);
			std::cerr << "DEBUG: Game::startGame() - game_thread object CREATED." << std::endl;

			// Give starter gear to player
			Equipment& player_mech_equipment = player_mech.getEquipment();
			std::cout << "\nEquiping basic loadout onto player mech" << std::endl;
			

			// Starter equipment based on class picked.	
			if (player_pulot
			auto common_laser_gun_item = std::make_shared<Item>(item_templates[0], Rarity::COMMON);
			player_mech_equipment.equip(common_laser_gun_item);
			player_mech_equipment.equip(std::make_shared<Item>(item_templates[2], Rarity::COMMON));
			player_mech_equipment.equip(std::make_shared<Item>(item_templates[3], Rarity::COMMON));

			player_mech.printCurrentEquipment();


		} catch (const std::system_error& e) {
			std::cerr << "FATAL ERROR: Game::startGame() - std::system_error while creating thread: " << e.what() << " (Code: " << e.code() << ")" << std::endl;
			game_running = false; // Revert state as thread creation failed.
			return false;
		} catch (const std::exception& e) {
			std::cerr << "FATAL ERROR: Game::startGame() - std::exception while creating thread: " << e.what() << std::endl;
			game_running = false; // Revert state
			return false;
		} catch (...) {
			std::cerr << "FATAL ERROR: Game::startGame() - Unknown error while creating thread." << std::endl;	
			game_running = false; // revert state
			return false;
		}

		// If thread creation didn't throw:
		std::cerr << "DEBUG: Game::startGame() - Successfully started. Returning true." << std::endl;
		return true;
	} else {
		std::cerr << "DEBUG: Game::startGame() - game_running was already true. Not starting again." << std::endl;
		return false; // Already running or failed to start
	}

}

void Game::stopGameLoop() {
	game_running = false;
	if (game_thread.joinable()) {
		game_thread.join();
	}
}

bool Game::isGameRunning() const {
	return game_running.load(); // Safely read atomic bool
}

void Game::gameLoop() {
	std::cerr<< "DEBUG: Game::gameLoop() THREAD STARTED." << std::endl;
	auto last_time = std::chrono::high_resolution_clock::now();
	std::this_thread::sleep_for(std::chrono::milliseconds(INITIAL_GAMELOOP_DELAY_MS)); // Wait for main thread to start up then this.
	while (game_running) {
		auto current_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = current_time - last_time;
		double delta_time = elapsed.count();
		last_time = current_time;

		gameTick(delta_time);

		// Cap update rate slightly to prevent 100% CPU usage on one core if gameTick is too fast
		std::this_thread::sleep_for(std::chrono::milliseconds(GAMELOOP_DELAY_MS)); // Approx 33 FPS for game logic
	}
	std::cerr << "DEBUG: Game::gameLoop() THREAD EXITED." << std::endl;
}

void Game::gameTick(double delta_time) {
	std::lock_guard<std::mutex> lock(game_state_mutex);

	// player_mech.regenerate(delta_time); // Player always regenerates
	if (combat_phase == CombatPhase::IDLE) {
		startCombat();
	} else if (combat_phase == CombatPhase::ENEMY_DEFEATED) {
		std::cout << "Enemy defeated..." << std::endl;
		awardLoot();
		enemies_defeated_on_floor++;

		if (is_enemy_boss) {
			current_floor++;
			enemies_defeated_on_floor = 0;
			std::cout << "Boss defeated! Advancing to next floor " + std::to_string(current_floor) << std::endl;
		}

		if (enemies_defeated_on_floor >= ENEMIES_PER_FLOOR) {
			spawnBoss();
		} else {
			spawnNextEnemy();
		}
		combat_phase = CombatPhase::IDLE; // Will trigger startCombat on next tick
	} else { // PLAYER_TURN, ENEMY_TURN, BETWEEN_TURNS
		handleCombat(delta_time);
	}

	// Check for player death (basic)
	if (!player_mech.isAlive()) {
		std::cout << "Player has been defeated. Try again." << std::endl;
		combat_phase = CombatPhase::IDLE;
		// Revive player for now
		player_mech.resetCombatState();
		std::cout << "Player Mech has been repaired." << std::endl;
	}
}

/**
	1. Checks for how many enemies defeated on the floor to see if it needs to spawn the boss or the next enemy
	2. Resets the player and current_enemy's combat state
	3. Determines who goes first by using the StatType::MOBILITY stat
  **/
void Game::startCombat() {
	std::cout << std::endl;
	std::cout << " ----------------------------- " << std::endl;
	std::cout << "Starting new combat encounter..." << std::endl;

	if (!current_enemy.isAlive() || current_enemy.getName().empty()) { // If no ememy or previous one was defeated
		if (enemies_defeated_on_floor >= ENEMIES_PER_FLOOR) {
			spawnBoss();
		} else {
			spawnNextEnemy();
		}
	}

	player_mech.resetCombatState();
	current_enemy.resetCombatState();
	
	std::cout << "Combat started against: " + current_enemy.getName() << std::endl;
	std::cout << "Player HP: " + std::to_string(player_mech.getCurrentHp()) + " Player Energy Shield: " + std::to_string(player_mech.getCurrentEnergyShield()) + ", Enemy HP: " + std::to_string(current_enemy.getCurrentHp()) + " Enemy Energy Shield: " + std::to_string(current_enemy.getCurrentEnergyShield()) << std::endl;

	// Determine who goes first
	double player_mobility = getStat(player_mech.getTotalStats(), StatType::MOBILITY);
	double enemy_mobility = getStat(current_enemy.getTotalStats(), StatType::MOBILITY);

	if (player_mobility > enemy_mobility) {
		combat_phase = CombatPhase::PLAYER_TURN;
		std::cout << "Player takes the first turn." << std::endl;
	} else {
		combat_phase = CombatPhase::ENEMY_TURN;
		std::cout << current_enemy.getName() + " takes the first turn." << std::endl;
	}
	time_since_last_action = 0.0;
}

/**
	1. Checks for the CombatPhase. (CombatPhase::PLAYER_TURN or CombatPhase::ENEMY_TURN)
	2. 
  **/
void Game::handleCombat(double delta_time) {
	time_since_last_action += delta_time;

	Mech* attacker = nullptr;
	Mech* defender = nullptr;
	CombatPhase next_phase_if_alive = CombatPhase::IDLE;
	std::string attacker_name;

	if (combat_phase == CombatPhase::PLAYER_TURN) {
		attacker = &player_mech;
		defender = &current_enemy;
		attacker_name = player_mech.getName();
		next_phase_if_alive = CombatPhase::ENEMY_TURN;
	} else if (combat_phase == CombatPhase::ENEMY_TURN) {
		attacker = &current_enemy;
		defender = &player_mech;
		attacker_name = current_enemy.getName();
		next_phase_if_alive = CombatPhase::PLAYER_TURN;
	} else {
		return; // Not a turn phase
	}

	double attack_speed = getStat(attacker->getTotalStats(), StatType::ATTACK_SPEED);
	if (attack_speed <= 0) attack_speed = 0.1; // Prevent division by zero and super slow attacks
	double required_delay = 1.0 / attack_speed;

	if (time_since_last_action >= required_delay) {
		double damage = attacker->calculateAttackDamage();
		std::cout << attacker_name + " attacks for " + std::to_string(damage).substr(0,4) + " damage." << std::endl;
		defender->takeDamage(damage); 

		if (!defender->isAlive()) {
			std::cout << defender->getName() + " has been defeated!" << std::endl;
			if (defender == &current_enemy) {
				combat_phase = CombatPhase::ENEMY_DEFEATED;
			} else { // Player was defeated
				std::cout << "Player defeated!" << std::endl;
			}
		} else {
			combat_phase = next_phase_if_alive;
			// std::cout << defender->getName() + " HP: " + std::to_string(defender->getCurrentHp()).substr(0,5) + " EN_SHIELD: " + std::to_string(defender->getCurrentEnergyShield()) << std::endl;
		}
		time_since_last_action = 0.0; // Reset timer for next action
		std::cout << " --- " << std::endl;
	}
}

void Game::spawnNextEnemy() {
	std::cout << "Spawning next regular enemy for floor " + std::to_string(current_floor) + ", defeated: " + std::to_string(enemies_defeated_on_floor + 1) << std::endl;	

	Stats enemy_stats;
	double base_hp = 50.0 * pow(1.2, current_floor-1) * pow(1.05, enemies_defeated_on_floor);
	double base_attack = 10.0 * pow(1.15, current_floor-1) * pow(1.03, enemies_defeated_on_floor);
	double base_armor = 1 * pow(1.1, current_floor-1);
	double base_shield = 20.0 * pow(1.1, current_floor-1);

	enemy_stats[StatType::HEALTH]		 = base_hp;
	enemy_stats[StatType::ATTACK]		 = base_attack;
	enemy_stats[StatType::ARMOR]		 = std::max(1.0, base_armor); // always have atleast 1 armor
	enemy_stats[StatType::ENERGY_SHIELD] = base_shield;
	enemy_stats[StatType::MOBILITY]		 = 5.0 + current_floor;
	enemy_stats[StatType::ATTACK_SPEED]  = 0.5 + (current_floor * 0.05);
	
	current_enemy.setName("Grunt Mech Mk." + std::to_string(current_floor) + "-" + std::to_string(enemies_defeated_on_floor + 1));
	current_enemy.setBaseStats(enemy_stats);
	current_enemy.resetCombatState(); // NOTE(MSR): This is crucial to do for stats like HP/Shield
	is_enemy_boss = false;
	std::cout << "Spawned: " + current_enemy.getName() + " with HP " + std::to_string(current_enemy.getCurrentHp()) << std::endl;
}

void Game::spawnBoss() {
	std::cout << "Spawning BOSS for floor " + std::to_string(current_floor) << std::endl;

	auto boss_it = boss_data.find(current_floor);
	if (boss_it != boss_data.end()) {
		const BossData& bd = boss_it->second;
		current_enemy.setName(bd.name);
		current_enemy.setBaseStats(bd.stats);
		current_enemy.resetCombatState();
		is_enemy_boss = true;
		std::cout << "Spawned BOSS: " + current_enemy.getName() << std::endl;
	} else {
		std::cout << "ERROR: No boss data found for floor " + std::to_string(current_floor) + ". Spawning a strong Grunt instead." << std::endl;
		// Fallback: spawn a very strong regular enemy
		Stats fallback_boss_stats;
		double base_hp = 200.0 * pow(1.5, current_floor-1);
		double base_attack = 50.0 * pow(1.4, current_floor-1);
		fallback_boss_stats[StatType::HEALTH] = base_hp;
		fallback_boss_stats[StatType::ATTACK] = base_attack;
		fallback_boss_stats[StatType::ARMOR] = 1 * pow(1.2, current_floor-1);
		fallback_boss_stats[StatType::ENERGY_SHIELD] = 100.0 * pow(1.3, current_floor-1);
		fallback_boss_stats[StatType::MOBILITY] = 10.0 + current_floor * 2;
		fallback_boss_stats[StatType::ATTACK_SPEED] = 0.8 + (current_floor * 0.1);

		current_enemy.setName("Overcharged Grunt");
		current_enemy.setBaseStats(fallback_boss_stats);
		current_enemy.resetCombatState();
		is_enemy_boss = true;
	}
}

void Game::awardLoot() {
	std::shared_ptr<Item> dropped_item = generateRandomItem();
	if (dropped_item) {
		std::cout << "Loot dropped: " + dropped_item->getName() + " (" + rarityToString(dropped_item->getRarity()) + ")" << std::endl;
		// For now, we don't auto-equip if its better or add to inventory just logging
		// TODO(MSR): player_mech.addToInventory(dropped_item)
	} else {
		std::cout << "No loot dropped this time." << std::endl;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(AWARDLOOT_DELAY_MS)); // Adding small delay so that awarded loot is given time to be read.
}

std::shared_ptr<Item> Game::generateRandomItem() {
	if (item_templates.empty()) {
		std::cout << "No item templates loaded, cannot generate loot." << std::endl;
		return nullptr;
	}

	// Simple rarity roll
	// 60% Common, 25% Uncommon, 10% Rare, 5% Legendary
	double roll = myRandomDouble(0, 100); 
	Rarity chosen_rarity;
	if (roll < 5) chosen_rarity = Rarity::LEGENDARY;
	else if (roll < 15) chosen_rarity = Rarity::RARE;
	else if (roll < 40) chosen_rarity = Rarity::UNCOMMON;
	else chosen_rarity = Rarity::COMMON;

	// Pick a random template
	int template_index = static_cast<int>(myRandomDouble(0, item_templates.size() - 0.0001)); // -0.0001 to make it exclusive for size
	std::shared_ptr<const ItemTemplate> chosen_template = item_templates[template_index];

	auto new_item = std::make_shared<Item>(chosen_template, chosen_rarity);
	// Item constructor calls generateInstanceStats
	return new_item;
}

// -- Equip Logic --
bool Game::playerEquipItem(int inventory_index) {
	std::lock_guard<std::mutex> lock(game_state_mutex);

	auto item_to_equip = player_mech.getItemFromInventory(inventory_index);
	if (!item_to_equip) return false;

	// Remove from inventory first
	player_mech.removeFromInventory(inventory_index);

	// Equip and get old item back
	auto old_item = player_mech.getEquipment().equip(item_to_equip);

	// Put old item in inventory if it exists
	if (old_item) {
		player_mech.addToInventory(old_item);
	}

	logEvent("Equipped " + item_to_equip->getName());
	return true;
}

void Game::logEvent(const std::string& msg) {
	game_log.push_back(msg);
	if (game_log.size() > MAX_LOG_SIZE) game_log.erase(game_log.begin());
}

GameStateForWeb Game::getGameState() {
	std::lock_guard<std::mutex> lock(game_state_mutex);
	GameStateForWeb state;

	if (!isGameRunning()) {
		state.player_name = "Game Not Started";
		state.recent_log.push_back("Game is waiting to be started.");

		// Fill other fields with placeholder/default values if needed by JS
		state.player_hp = 0; state.player_max_hp = 0;
		state.enemy_name = "-";
		return state;
	}

	// Player Info
	if (!player_mech.getName().empty()) {
		state.player_name = player_mech.getName();
		std::cerr << "DEBUG: Player name: " << state.player_name << std::endl;
	} else {
		std::cerr << "DEBUG: Player name is empty!" << std::endl;
		state.player_name = "DefaultPlayer"; // Fallback
	}

	// Check player_mech.equipment unique_ptr
	if (!player_mech.getEquipmentInternalPtr()) {
		std::cerr << "FATAL DEBUG: player_mech.equipment is NULL in getGameState" << std::endl;	
	} else {
		std::cerr << "DEBUG: player_mech.equipment is valid." << std::endl;
	}


	Stats p_total_stats = player_mech.getTotalStats();
	state.player_hp = player_mech.getCurrentHp();
	state.player_max_hp = getStat(p_total_stats, StatType::HEALTH);
	state.player_shield = player_mech.getCurrentEnergyShield();
	state.player_max_shield = getStat(p_total_stats, StatType::ENERGY_SHIELD);
	state.player_total_stats = p_total_stats;
	for (const auto& pair : player_mech.getEquipment().getEquippedItems()) {
		if (pair.second) {
			state.player_equipment_names[pair.first] = pair.second->getName() + " (" + rarityToString(pair.second->getRarity()) + ")";
		} else {
			state.player_equipment_names[pair.first] = "(Empty)";
		}
	}

	// Populate Inventory for Web
	const auto& inv = player_mech.getInventory();
	for (size_t i = 0; i < inv.size(); i++) {
		if (inv[i]) {
			InventoryItemWeb item_web;
			item_web.index = static_cast<int>(i);
			item_web.name = inv[i]->getName();
			item_web.slot = equipmentSlotToString(inv[i]->getSlot());
			item_web.rarity = rarityToString(inv[i]->getRarity());
			item_web.tech = inv[i]->getRequiredTech();
			state.inventory.push_back(item_web);
		}
	}

	// Enemy Info
	if (!current_enemy.getName().empty()) { // Check if an enemy is actually spawned
		state.enemy_name = current_enemy.getName();
		Stats e_total_stats = current_enemy.getTotalStats(); // Assuming enemy might have equipment

		state.enemy_hp = current_enemy.getCurrentHp();
		state.enemy_max_hp = getStat(e_total_stats, StatType::HEALTH);
		state.enemy_shield = current_enemy.getCurrentEnergyShield();
		state.enemy_max_shield = getStat(e_total_stats, StatType::ENERGY_SHIELD);
		state.enemy_is_boss = is_enemy_boss;
	} else {
		state.enemy_name = "No Target";
		state.enemy_hp = 0;
		state.enemy_max_hp = 0;
		state.enemy_shield = 0;
		state.enemy_max_shield = 0;
		state.enemy_is_boss = false;
	}

	// Game Progress
	state.current_floor = current_floor;
	state.enemies_defeated_on_floor = enemies_defeated_on_floor;

	// Log
	state.recent_log = game_log; // Copy the log

	return state;
}


// DEBUGMETHODS: print mech stats
void Game::print_player_mech_stats() {
	Stats pm_stats = player_mech.getBaseStats();
	std::cout << " --- " << player_mech.getName() << " Stats --- " << std::endl;
	std::cout << "HEALTH: " << getStat(pm_stats, StatType::HEALTH) <<  std::endl;
	std::cout << "ARMOR: " << getStat(pm_stats, StatType::ARMOR) <<  std::endl;
	std::cout << "ENERGY_SHIELD: " << getStat(pm_stats, StatType::ENERGY_SHIELD) <<  std::endl;
	std::cout << "ATTACK: " << getStat(pm_stats, StatType::ATTACK) <<  std::endl;
	std::cout << "ATTACK_SPEED: " << getStat(pm_stats, StatType::ATTACK_SPEED) <<  std::endl;
	std::cout << "MOBILITY: " << getStat(pm_stats, StatType::MOBILITY) <<  std::endl;
	std::cout << "ENERGY: " << getStat(pm_stats, StatType::ENERGY) <<  std::endl;
	std::cout << "ENERGY_RECOVERY: " << getStat(pm_stats, StatType::ENERGY_RECOVERY) <<  std::endl;
	std::cout << "REPAIR: " << getStat(pm_stats, StatType::REPAIR) <<  std::endl;
	std::cout << "TECHNOLOGY: " << getStat(pm_stats, StatType::TECHNOLOGY) <<  std::endl;

}

void Game::print_enemy_mech_stats() {
	std::cout << " --- " << current_enemy.getName() << " Stats --- " << std::endl;
	Stats em_stats = current_enemy.getBaseStats();
	std::cout << "HEALTH: " << getStat(em_stats, StatType::HEALTH) <<  std::endl;
	std::cout << "ARMOR: " << getStat(em_stats, StatType::ARMOR) <<  std::endl;
	std::cout << "ENERGY_SHIELD: " << getStat(em_stats, StatType::ENERGY_SHIELD) <<  std::endl;
	std::cout << "ATTACK: " << getStat(em_stats, StatType::ATTACK) <<  std::endl;
	std::cout << "ATTACK_SPEED: " << getStat(em_stats, StatType::ATTACK_SPEED) <<  std::endl;
	std::cout << "MOBILITY: " << getStat(em_stats, StatType::MOBILITY) <<  std::endl;
	std::cout << "ENERGY: " << getStat(em_stats, StatType::ENERGY) <<  std::endl;
	std::cout << "ENERGY_RECOVERY: " << getStat(em_stats, StatType::ENERGY_RECOVERY) <<  std::endl;
	std::cout << "REPAIR: " << getStat(em_stats, StatType::REPAIR) <<  std::endl;
	std::cout << "TECHNOLOGY: " << getStat(em_stats, StatType::TECHNOLOGY) <<  std::endl;
}
