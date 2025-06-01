#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <map>

#include "Mech.h"
#include "Item.h"
#include "json.hpp" // nlohmann/json

/* Implementation Highlights
	startGameLoop(): Sets `game_running` to true and creates `game_thread` calling `gameLoop`.

	stopGameLoop(): Sets `game_running` to false and `join()`s the `game_thread`

	gameLoop(): Contains a `while(game_running)` loop. Inside, calculate `delta_time` (time since last iteration) using `<chrono>`. Call `gameTick(delta_time)`. Sleep for a short duration (e.g., `std::this_thread::sleep_for(std::chrono::milliseconds(50));`) to avoid busy-waiting.	
	
	gameTick():
		- Lock the `game_state_mutex`.
		- If `combat_phase == IDLE`, call `startCombat()`.
		- If combat is active, call `handleCombat(delta_time)`.
		- If

*/

using json = nlohmann::json;

struct BossData {
	std::string name;
	Stats stats;
};

struct GameStateForWeb { // Data structure to send to frontend
	// Player Info
	std::string player_name;
	double player_hp;
	double player_max_hp;
	double player_shield;
	double player_max_shield;
	Stats player_total_stats;
	std::map<EquipmentSlot, std::string> player_equipment_names; // Simple view

	// Enemy Info
	std::string enemy_name;
	double enemy_hp;
	double enemy_max_hp;
	double enemy_shield;
	double enemy_max_shield;
	bool enemy_is_boss;

	// Game Progress
	int current_floor;
	int enemies_defeated_on_floor;

	// Log/Events
	std::vector<std::string> recent_log;
};

class Game {
public:
	Game();
	~Game();

	void loadData(const std::string& item_file, const std::string& boss_file);
	void startGameLoop();
	void stopGameLoop();

	GameStateForWeb getGameState(); // Thread-safe getter for web server


	// Debug methods
	void print_player_mech_stats();
	void print_enemy_mech_stats();

	Mech player_mech;
	Mech current_enemy;

private:
	void gameLoop(); // The function that runs in a separate thead
	void gameTick(double delta_time); // Logic for one update cycle
	void startCombat();
	void handleCombat(double delta_time);
	void awardLoot();
	void spawnNextEnemy();
	void spawnBoss();
	std::shared_ptr<Item> generateRandomItem(); // Creates an item drop
	void logEvent(const std::string& message);

	bool is_enemy_boss = false;

	int current_floor = 1;
	int enemies_defeated_on_floor = 0;
	const int ENEMIES_PER_FLOOR = 20; // Enemies before boss

	// Data loaded from JSON
	std::vector<std::shared_ptr<const ItemTemplate>> item_templates;
	std::map<int, BossData> boss_data; // Floor -> BossData

	// Game loop control
	std::thread game_thread;
	std::atomic<bool> game_running{false};
	std::mutex game_state_mutex; // Protects access to shared game state

	// Combat state
	enum class CombatPhase { IDLE, PLAYER_TURN, ENEMY_TURN, BETWEEN_TURNS, ENEMY_DEFEATED };
	CombatPhase combat_phase = CombatPhase::IDLE;
	double time_since_last_action = 0.0;
	double turn_delay = 0.5; // Base time between turns/actions

	// Logging
	std::vector<std::string> game_log;
	const size_t MAX_LOG_SIZE = 20;
};

#endif // GAME_H
