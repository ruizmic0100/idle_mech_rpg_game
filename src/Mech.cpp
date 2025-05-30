#include <iostream>
#include <algorithm>
#include <utility>

#include "Mech.h"

// Constructor: Parameterized for creating mechs with initial stats.
Mech::Mech(std::string n, Stats base) {
	name = n;
	base_stats = base;

	std::cout << "Mech '" << this->name << "' created." << std::endl;
}

// --- Getters ---
std::string Mech::getName() const {
	return name;
}

const Stats& Mech::getBaseStats() const {
	return base_stats;
}

// TODO(MSR): getEquipment()

double Mech::getCurrentHp() const {
	return current_hp;
}

double Mech::getCurrentEnergyShield() const {
	return current_energy_shield;
}

double Mech::getCurrentEnergy() const {
	return current_energy;
}

