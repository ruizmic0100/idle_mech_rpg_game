#ifndef MECH_H
#define MECH_H

#include <string>
#include <memory>
#include "Stats.h"
#include "Equipment.h"

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
	
	// Getter for debugging
	const std::unique_ptr<Equipment>& getEquipmentInternalPtr() const { return equipment; }

private:
	std::string name;
	Stats base_stats;
	std::unique_ptr<Equipment> equipment; // Use unique_ptr for ownership

	// Live stats
	double current_hp = 0;
	double current_energy_shield = 0;
	double current_energy = 0;
};


#endif // MECH_H
