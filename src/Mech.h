#ifndef MECH_H
#define MECH_H

#include <string>
#include <memory>
#include <map>
#include "Stats.h"
#include "Equipment.h"
#include "GameClasses.h"

class Mech {
public:
	Mech(std::string n, Stats base);
	Mech() = default; // Needed for placeholder enemy

	std::string getName() const;
	const Stats& getBaseStats() const;
	Stats getTotalStats() const; // Combines base + equipment
	Equipment& getEquipment(); // Non-const access to manage equipment

	// Current combat state
	double getCurrentHp() const;
	double getCurrentEnergyShield() const;
	double getCurrentEnergy() const;

	void takeDamage(double incoming_damage);
	void regenerate(double delta_time); // Applies repair and energy recovery
	void resetCombatState(); // Resets HP, Shield, Energy to max based on total stats
	bool isAlive() const;

	// Simplified Combat Action
	double calculateAttackDamage() const; // Returns damage for one attack 'tick'

	// Placeholder for Tech requirement check later
	bool canEquip(const Item& item) const;

	void setName(const std::string& n); // For bosses/enemies
	void setBaseStats(const Stats& s); // For bosses/enemies

	// Print current equipment
	void printCurrentEquipment() const;
	
	// Getter for debugging
	const std::unique_ptr<Equipment>& getEquipmentInternalPtr() const { return equipment; }

	// -- Inventory Methods --
	void addToInventory(std::shared_ptr<Item> item);
	std::shared_ptr<Item> getItemFromInventory(int index);

	// Remove item at index from inventor vector
	void removeFromInventory(int index);
	const std::vector<std::shared_ptr<Item>>& getInventory() const;

	// Experience methods
	void addExperience(int amount, std::map<std::string, std::map<int, int>> level_requirements, std::string pc_id);
	int getCurrentExperience() const { return current_exp; }
	int getLevel() const { return level; }

private:
	std::string name;
	Stats base_stats;
	std::unique_ptr<Equipment> equipment; // Using unique_ptr for ownership

	// Storage for unequipped items
	std::vector<std::shared_ptr<Item>> inventory;

	// Live stats
	double current_hp = 0;
	double current_energy_shield = 0;
	double current_energy = 0;

	// EXP
	int current_exp = 0;
	int level = 0;
	std::string player_class = "NONE"; // Default, should be set on creation
};


#endif // MECH_H
