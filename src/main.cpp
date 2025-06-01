#include <iostream>	
#include <string>
#include <vector>
#include <fstream>
#include "crow_all.h"
#include "Game.h"
#include "json.hpp"

using json = nlohmann::json;

// --- JSON Serialization for GameStateForWeb ---
// Need to tell nlohmann/json how to convert our structs to JSON

// Helper to convert Stats map to JSON object
void mech_stats_to_json(json& j, Mech& m) {
	j = json::object();
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

		j[key] = pair.second;
	}

	json wrapped_json_object;
	wrapped_json_object[mech_name_key] = j;
}

// --- End JSON Serialization ---




int main() {
	// Initialize Game
	Game game_instance;

	

//	try {
//		game_instance.loadData("data/items.json", "data/bosses.json");
//	} catch (const std::exception& e) {
//		std::cerr << "Error loading game data: " << e.what() << std::endl;
//		return 1;
//	}

	// Start Game Loop in a separate thread
	//game_instance.startGameLoop();
	std::cout << "Game loop started." << std::endl;

	game_instance.print_player_mech_stats();
	game_instance.print_enemy_mech_stats();

	std::cout << "Creating json object" << std::endl;
	json test_j;
	mech_stats_to_json(test_j, game_instance.player_mech);
	std::cout << "Creating json file" << std::endl;
	std::ofstream o("output.json");
	o << test_j.dump(4) << std::endl;
	o.close();
	

	// Initialize Web Server (Crow)
	//crow::SimpleApp app;

	// API endpoint to get current game state
//	CROW_ROUTE(app, "/api/gamestate")
//	([&game_instance]() { // Capture game_instance by reference
//		GameStateForWeb current_state = game_instance.getGameState();
//		json response_json = current_state; // Uses the to_json function we defined
//		return crow::response(response_json.dump()); // Convert JSON object to string
//	 });

	// Simple route to serve the HTML file (adjust path if needed)
	


	return 1;
}
