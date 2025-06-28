#include <iostream>
#include <algorithm>
#include <utility>

#include "Mech.h"

// Constructor: Parameterized for creating mechs with initial stats.
Mech::Mech(std::string n, Stats base) {
	name = n;
	base_stats = base;
	equipment = std::make_unique<Equipment>(); // RAII empty equipment

	resetCombatState();

	std::cout << "Mech '" << this->name << "' created." << std::endl;
}

// --- Getters ---
std::string Mech::getName() const {
	return name;
}

const Stats& Mech::getBaseStats() const {
	return base_stats;
}

double Mech::getCurrentHp() const {
	return current_hp;
}

double Mech::getCurrentEnergyShield() const {
	return current_energy_shield;
}

double Mech::getCurrentEnergy() const {
	return current_energy;
}

// --- Setters ---
void Mech::setName(const std::string& n) {
	this->name = n;
}

void Mech::setBaseStats(const Stats& s) {
	this->base_stats = s;
}

// Returns a reference to the Equipment Manager
// the unique_ptr 'equipment' should have been initialized by constructors
Equipment& Mech::getEquipment() {
	if (!equipment) {
		// This case should ideally not be hit if constructors always init equipment
	}
	return *equipment; // Dereference the unique_ptr
}

// --- Core Logic ---

// Calculates: total stats by combining base_stats with stats from equipped items
Stats Mech::getTotalStats() const {
	Stats total_s = base_stats; // Copy of base stats

	if (equipment) { // Check if the equipment manager exists
		Stats equipment_s = equipment->getTotalStats(); // Get summed stats from all equipped items
		for (const auto& pair : equipment_s) {
			addStat(total_s, pair.first, pair.second);
		}
	}

	return total_s;
}

// Resets current HP, Shield, and Energy to their maximum values based on current total stats
void Mech::resetCombatState() {
	Stats current_total_stats = getTotalStats();

	current_hp = getStat(current_total_stats, StatType::HEALTH);
	current_energy_shield = getStat(current_total_stats, StatType::ENERGY_SHIELD);
	current_energy = getStat(current_total_stats, StatType::ENERGY);

	// Ensure combat stats are not negative
	current_hp = std::max(0.0, current_hp);
	current_energy_shield = std::max(0.0, current_energy_shield);
	current_energy = std::max(0.0, current_energy);
}

// Applies incoming damage, considering armor and shields
void Mech::takeDamage(double incoming_damage) {
	if (incoming_damage <= 0 || !isAlive()) {
		return; // No damage to take or already defeated
	}

	Stats current_total_stats = getTotalStats();


	// Apply damage to energy shield first
	double damage_after_shield = incoming_damage;
	if (current_energy_shield > 0) {
		double damage_to_shield = std::min(current_energy_shield, incoming_damage);
		current_energy_shield -= damage_to_shield;
		damage_after_shield -= damage_to_shield;
	}
	
	double damage_after_armor = 0;
	if (current_energy_shield == 0) {

		double armor_value = getStat(current_total_stats, StatType::ARMOR);

		// Ensure armor is within limits (1 to 9999)
		armor_value = std::max(1.0, std::min(9999.0, armor_value));

		damage_after_armor = damage_after_shield * (1 - (armor_value / 9999)); // 9999 being the max armor mitigation one could have
	}
	
	// Apply remaining damage after mitigations
	if (damage_after_armor > 0) {
		current_hp -= damage_after_armor;
	}
	
	// Ensure hp doesn't go below zero
	if (current_hp < 0) {
		current_hp = 0;
	}

	std::cout << getName() + " after damage HP: " + std::to_string(getCurrentHp()) + " EN_SHIELD: " + std::to_string(getCurrentEnergyShield()) << std::endl; 
}

// Regenerates health and energy over time
void Mech::regenerate(double delta_time) {
	if (!isAlive() && getStat(getTotalStats(), StatType::REPAIR) <= 0) { // No self-repair if truly dead and no passive repair
		return;
	}

	Stats current_total_stats = getTotalStats();
	double max_hp = getStat(current_total_stats, StatType::HEALTH);
	double max_energy = getStat(current_total_stats, StatType::ENERGY);

	// Health Regeneration (Repair)
	double repair_amount = getStat(current_total_stats, StatType::REPAIR) * delta_time;
	if (repair_amount > 0 && current_hp < max_hp) {
		current_hp += repair_amount;
		if (current_hp > max_hp) {
			current_hp = max_hp;
		}
	}

	// Energy Regeneration
	double energy_regen_amount = getStat(current_total_stats, StatType::ENERGY_RECOVERY) * delta_time;
	if (energy_regen_amount > 0 && current_energy < max_energy) {
		current_energy += energy_regen_amount;
		if (current_energy > max_energy) {
			current_energy = max_energy;
		}
	}
}

// Checks if the mech is still operational
bool Mech::isAlive() const {
	return current_hp > 0;
}

// Calculates the base damage for a single attack action.
// Attack speed will determine how often this is called by the game loop
double Mech::calculateAttackDamage() const {
	return getStat(getTotalStats(), StatType::ATTACK);
}

// Checks if the mech meets the technology requirement for an item
bool Mech::canEquip(const Item& item) const {
	// This is a simplified check. A real system might check slot availability, item type restrictions, etc.
	return getStat(getTotalStats(), StatType::TECHNOLOGY) >= item.getRequiredTech();
}

// Prints current equipment
void Mech::printCurrentEquipment() const {
	std::cout << "\n--- Current Player Loadout ---" << std::endl;
	std::string head_slot = "NONE";
	std::string chest_slot = "NONE";
	std::string arms_slot = "NONE";
	std::string legs_slot = "NONE";
	std::string generator_slot = "NONE";
	std::string left_arm_weapon_slot = "NONE";
	std::string right_arm_weapon_slot = "NONE";
	std::string left_shoulder_weapon_slot = "NONE";
	std::string right_shoulder_weapon_slot = "NONE";

	if (equipment->getItem(EquipmentSlot::HEAD)) {
		head_slot = equipment->getItem(EquipmentSlot::HEAD)->getName();
	}
	if (equipment->getItem(EquipmentSlot::CHEST)) {
		chest_slot = equipment->getItem(EquipmentSlot::CHEST)->getName();
	}
	if (equipment->getItem(EquipmentSlot::ARMS)) {
		arms_slot = equipment->getItem(EquipmentSlot::ARMS)->getName();
	}
	if (equipment->getItem(EquipmentSlot::LEGS)) {
		legs_slot = equipment->getItem(EquipmentSlot::LEGS)->getName();
	}
	if (equipment->getItem(EquipmentSlot::GENERATOR)) {
		generator_slot = equipment->getItem(EquipmentSlot::GENERATOR)->getName();
	}
	if (equipment->getItem(EquipmentSlot::LEFT_ARM_WEAPON)) {
		left_arm_weapon_slot = equipment->getItem(EquipmentSlot::LEFT_ARM_WEAPON)->getName();
	}
	if (equipment->getItem(EquipmentSlot::RIGHT_ARM_WEAPON)) {
		right_arm_weapon_slot = equipment->getItem(EquipmentSlot::RIGHT_ARM_WEAPON)->getName();
	}
	if (equipment->getItem(EquipmentSlot::LEFT_SHOULDER_WEAPON)) {
		left_shoulder_weapon_slot = equipment->getItem(EquipmentSlot::LEFT_SHOULDER_WEAPON)->getName();
	}
	if (equipment->getItem(EquipmentSlot::RIGHT_SHOULDER_WEAPON)) {
		right_shoulder_weapon_slot = equipment->getItem(EquipmentSlot::RIGHT_SHOULDER_WEAPON)->getName();
	}

	std::cout << "HEAD: " << head_slot << std::endl;
	std::cout << "CHEST: " << chest_slot << std::endl;
	std::cout << "ARMS: " << arms_slot << std::endl;
	std::cout << "LEGS: " << legs_slot << std::endl;
	std::cout << "GENERATOR: " << generator_slot << std::endl;
	std::cout << "LEFT_ARM_WEAPON: " << left_arm_weapon_slot << std::endl;
	std::cout << "RIGHT_ARM_WEAPON: " << right_arm_weapon_slot << std::endl;
	std::cout << "LEFT_SHOULDER_WEAPON: " << left_shoulder_weapon_slot << std::endl;
	std::cout << "RIGHT_SHOULDER_WEAPON: " << right_shoulder_weapon_slot << std::endl;
	std::cout << "---" << std::endl;
}
