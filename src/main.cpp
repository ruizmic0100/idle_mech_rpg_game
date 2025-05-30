#include <iostream>	
#include <string>
#include <vector>
#include "crow_all.h"
#include "Game.h"
#include "json.hpp"

using json = nlohmann::json;

// --- JSON Serialization for GameStateForWeb ---
// Need to tell nlohmann/json how to convert our structs to JSON

// Helper to convert Stats map to JSON object
void to_json(json& j, const Stats& s) {
	j = json::object();
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
