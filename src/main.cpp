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

// Conversion function for InventoryItemWeb so that the frontend can consume it
void to_json(json& j, const InventoryItemWeb& item) {
	j = json{
		{"index", item.index},
		{"name", item.name},
		{"slot", item.slot},
		{"rarity", item.rarity},
		{"tech", item.tech}
	};
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
			{"level", gs.player_level},
			{"exp", gs.player_experience},
			{"exp_needed", gs.player_next_level_experience},
			{"stats", gs.player_total_stats}, // Uses the Stats to_json helper
			{"equipment", player_equip}

		}},
		{"enemy", {
			{"name", gs.enemy_name},
			{"hp", gs.enemy_hp},
			{"max_hp", gs.enemy_max_hp},
			{"shield", gs.enemy_shield},
			{"max_shield", gs.enemy_max_shield},
			{"is_boss", gs.enemy_is_boss},
			{"stats", gs.enemy_stats}
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
	
	// Load in files from data/
	try {
		game_instance.loadData("data/items.json", "data/bosses.json", "data/levels.json");
	} catch (const std::exception& e) {
		std::cerr << "Error loading game data: " << e.what() << std::endl;
		return 1;
	}

	
	// TODO(MSR): Move this to Game.cpp	
	// Creating player_mech json stats file
//	json player_mech_j;
//	mech_stats_to_json(player_mech_j, game_instance.player_mech);
//	std::ofstream p_o("player_mech.json");
//	p_o << player_mech_j.dump(4);
//	p_o.close();
//	std::cout << "Created player_mech json stats file" << std::endl;


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

	// API endpoint to start the game
	CROW_ROUTE(app, "/api/startgame").methods(crow::HTTPMethod::Post) // POST for actions
	([&game_instance]() {
		if (!game_instance.isClassSelected()) {
			return crow::response(403, "System Error: No pilot class initialized.");
		}
		if (game_instance.startGame()) {
			return crow::response(200, "Game started successfully.");
		} else {
			return crow::response(409, "Game already running or failed to start."); // 409 conflict
		}
	 });

	// Equip API
	CROW_ROUTE(app, "/api/equip").methods(crow::HTTPMethod::Post)
	([&game_instance](const crow::request& req) {
		try {
			auto body = json::parse(req.body);
			if (!body.contains("index")) return crow::response(400, "Missing index");

			int idx = body["index"].get<int>();
			if (game_instance.playerEquipItem(idx)) {
				return crow::response(200, "Equipped");
			} else {
				return crow::response(400, "Failed to equip");
			}
		} catch(...) {
			return crow::response(500, "Server Error");
		}
	});

	// Simple route to serve the HTML file (adjust path if needed)
	CROW_ROUTE(app, "/")
	([]() {
		// Might want a more robust way to find/serve files
		// Crow has crow::mustche for templating, but simple serve is used for now
		//auto page = crow::mustache::load("index.html");
		auto page = crow::mustache::load("main_menu.html");
		
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

	CROW_ROUTE(app, "/web/media/<string>")
	([&](const crow::request& req, crow::response& res, std::string filename) {
		std::string imagePath = "web/media/" + filename;
		std::ifstream file(imagePath, std::ios::binary);

		if (file.is_open()) {
			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

			res.set_header("Content-Type", "image/jpeg");

			res.write(content);
			res.end();
		} else {
			res.code = 404;
			res.write("Image not found");
			res.end();
		}
	 });
	
	// Serve the class Selection Screen
	CROW_ROUTE(app, "/new_game")([](){
		crow::mustache::context ctx;
		return crow::mustache::load("class_selection.html").render(ctx);	
	});

	// This will be called when the player clicks a specific class card
	CROW_ROUTE(app, "/init_game")
	([&game_instance](const crow::request& req) {
		// 1. Get the class from the URL parameters
		auto selected_class = req.url_params.get("class");

		if (!selected_class) {
			return crow::response(400, "Error: No class selected.");
		}
		
		std::string class_id = selected_class;
		std::cout << "-----" << std::endl;
		std::cout << "SELECTED_CLASS = " << class_id << std::endl;
		std::cout << "-----" << std::endl;

		// 2. Initialize the player with the chosen class
		// This method will:
		// - Create the PilotClass
		// - Instantiate the Mech with class stats
		// - Set the class_selected flag
		if (!game_instance.initPlayerClass(class_id)) {
			return crow::response(400, "Error: Invalid class selected.");
		}

		std::cout << "Game initialized for class: " << class_id << std::endl;
			
		crow::response res;
		res.redirect("/game_dashboard");
		return res;
	 });

	// The main game dashboard
	CROW_ROUTE(app, "/game_dashboard")([](){
		auto page = crow::mustache::load("index.html");

		return page.render();
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
