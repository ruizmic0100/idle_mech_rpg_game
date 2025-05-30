#include <iostream>

#include "Game.h"

Game::Game() {

	std::cout << "Game Object constructed!" << std::endl;

	// Initialize player mech with some basic default stats
	Stats player_base_stats = {
		{StatType::HEALTH, 200},
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
	player_mech = Mech("Player", player_base_stats);
	std::cout << "`player_mech` initialized with `player_base_stats`." << std::endl;

	// Initialize a test enemy mech with default constructor stats
	Stats test_enemy_stats = { 
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
	current_enemy = Mech("Test Enemy", test_enemy_stats);
	std::cout << "`current_enemy` initialized with `test_enemy_stats`." << std::endl;

}

Game::~Game() {
	std::cout << "Game Object destructed!" << std::endl;
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
