#include <iostream>	
#include <string>
#include <vector>
#include <fstream>
#include "crow_all.h"
#include "Game.h"
#include "json.hpp"

using json = nlohmann::json;


// Helper to convert EquipmentSlot enum to string
std::string slotToString(EquipmentSlot slot) {
	switch(slot) {
		case EquipmentSlot::HEAD: return "HEAD";
		case EquipmentSlot::CHEST: return "CHEST";
		case EquipmentSlot::ARMS: return "ARMS";
		case EquipmentSlot::LEGS: return "LEGS";
		case EquipmentSlot::GENERATOR: return "GENERATOR";
		case EquipmentSlot::LEFT_ARM_WEAPON: return "LEFT_ARM_WEAPON";
		case EquipmentSlot::RIGHT_ARM_WEAPON: return "RIGHT_ARM_WEAPON";
		case EquipmentSlot::LEFT_SHOULDER_WEAPON: return "LEFT_SHOULDER_WEAPON";
		case EquipmentSlot::RIGHT_SHOULDER_WEAPON: return "RIGHT_SHOULDER_WEAPON";
		default: return "NONE";
	}
}

// --- JSON Serialization for GameStateForWeb ---
// Need to tell nlohmann/json how to convert our structs to JSON
void to_json(json& j, const Stats& s) {
	j = json::object();

	for (const auto& pair : s) {
		// Convert StatType enum to string for JSON key
		std::string key;
		switch(pair.first) {
			case StatType::HEALTH:
				key = "HEALTH";
				break;
			case StatType::ARMOR:
				key = "ARMOR";
				break;
			case StatType::ENERGY_SHIELD:
				key = "ENERGY_SHIELD";
				break;
			case StatType::ATTACK:
				key = "ATTACK";
				break;
			case StatType::ATTACK_SPEED:
				key = "ATTACK_SPEED";
				break;
			case StatType::MOBILITY:
				key = "MOBILITY";
				break;
			case StatType::ENERGY:
				key = "ENERGY";
				break;
			case StatType::ENERGY_RECOVERY:
				key = "ENERGY_RECOVERY";
				break;
			case StatType::REPAIR:
				key = "REPAIR";
				break;
			case StatType::TECHNOLOGY:
				key = "TECHNOLOGY";
				break;
			default:
				key = "UNKNOWN";
				break;
		}

		j[key] = pair.second;
	}
}

// Main conversion function for GameStateForWeb
void to_json(json& j, const GameStateForWeb& gs) {
	json player_equip = json::object();
	for (const auto& pair : gs.player_equipment_names) {
		player_equip[slotToString(pair.first)] = pair.second;
	}

	j = json {
		{"player", {
			{"name", gs.player_name},
			{"hp", gs.player_hp},
			{"max_hp", gs.player_max_hp},
			{"shield", gs.player_shield},
			{"max_shield", gs.player_max_shield},
			{"stats", gs.player_total_stats}, // Uses the Stats to_json helper
			{"equipment", player_equip}

		}},
		{"enemy", {
			{"name", gs.enemy_name},
			{"hp", gs.enemy_hp},
			{"max_hp", gs.enemy_max_hp},
			{"shield", gs.enemy_shield},
			{"max_shield", gs.enemy_max_shield},
			{"is_boss", gs.enemy_is_boss}
		}},
		{"progress", {
			{"floor", gs.current_floor},
			{"enemies_defeated", gs.enemies_defeated_on_floor}
		}},
		{"log", gs.recent_log}
	};
}

// Helper to convert Stats map to JSON object
void mech_stats_to_json(json& j, Mech& m) {
	j = json::object();
	json wrapped_json_object;
	Stats s = m.getBaseStats();
	std::string mech_name_key = m.getName();

	for (const auto& pair : s) {
		// Convert StatType enum to string for JSON key
		std::string key;
		switch(pair.first) {
			case StatType::HEALTH:
				key = "HEALTH";
				break;
			case StatType::ARMOR:
				key = "ARMOR";
				break;
			case StatType::ENERGY_SHIELD:
				key = "ENERGY_SHIELD";
				break;
			case StatType::ATTACK:
				key = "ATTACK";
				break;
			case StatType::ATTACK_SPEED:
				key = "ATTACK_SPEED";
				break;
			case StatType::MOBILITY:
				key = "MOBILITY";
				break;
			case StatType::ENERGY:
				key = "ENERGY";
				break;
			case StatType::ENERGY_RECOVERY:
				key = "ENERGY_RECOVERY";
				break;
			case StatType::REPAIR:
				key = "REPAIR";
				break;
			case StatType::TECHNOLOGY:
				key = "TECHNOLOGY";
				break;
			default:
				key = "UNKNOWN";
				break;
		}

		wrapped_json_object[key] = pair.second;
	}

	j[mech_name_key] = wrapped_json_object;
}

// --- End JSON Serialization ---





int main() {
	// Initialize Game
	Game game_instance;

	try {
		game_instance.loadData("data/items.json", "data/bosses.json");
	} catch (const std::exception& e) {
		std::cerr << "Error loading game data: " << e.what() << std::endl;
		return 1;
	}

	// Start Game Loop in a separate thread
	game_instance.startGameLoop();
	std::cout << "Game loop started." << std::endl;
	
	// Creating player_mech json stats file
	json player_mech_j;
	mech_stats_to_json(player_mech_j, game_instance.player_mech);
	std::ofstream p_o("player_mech.json");
	p_o << player_mech_j.dump(4);
	p_o.close();
	std::cout << "Created player_mech json stats file" << std::endl;


	//Initialize Web Server (Crow)
	crow::SimpleApp app;
	crow::mustache::set_global_base("web");

	// API endpoint to get current game state
	CROW_ROUTE(app, "/api/gamestate")
	([&game_instance]() { // Capture game_instance by reference
		GameStateForWeb current_state = game_instance.getGameState();
		json response_json = current_state; // Uses the to_json function we defined
		return crow::response(response_json.dump()); // Convert JSON object to string
	 });

	// Simple route to serve the HTML file (adjust path if needed)
	CROW_ROUTE(app, "/")
	([]() {
		// Might want a more robust way to find/serve files
		// Crow has crow::mustche for templating, but simple serve is used for now
		auto page = crow::mustache::load("index.html");
		return page.render();
	});

	// Route to serve JS
	CROW_ROUTE(app, "/script.js")
	([]() {
		// Serve the file, make sure the content type is correct
		std::ifstream t("web/script.js");
		std::stringstream buffer;
		buffer << t.rdbuf();
		crow::response res(buffer.str());
		res.set_header("Content-Type", "text/javascript");
		return res;		

	}); 

	CROW_ROUTE(app, "/style.css")
	([]() {
		std::ifstream t("web/style.css");
		std::stringstream buffer;
		buffer << t.rdbuf();
		crow::response res(buffer.str());
		res.set_header("Content-Type", "text/css");
		return res;
	});

	// Run the server on port 18080
	// Make it run threaded so it doesn't block the main thread
	std::cout << "Web server running on http://localhost:18080" << std::endl;
	std::cout << std::endl;
	app.port(18080).multithreaded().run();
	

	std::cout << "Press Enter to stop the game..." << std::endl;
	std::cin.get(); // Wait for user input

	// Cleanup
	std::cout << "Stopping game loop..." << std::endl;
	game_instance.stopGameLoop();
	std::cout << "Game stopped." << std::endl;

	return 1;
}
