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

	std::cout << "Press Enter to stop the game..." << std::endl;
	std::cin.get(); // Wait for user input

	

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
