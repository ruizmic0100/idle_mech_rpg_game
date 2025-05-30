#ifndef STATS_H
#define STATS_H

#include <map>
#include <string>

enum class StatType {
	HEALTH, ARMOR, ENERGY_SHIELD, ATTACK,
	ATTACK_SPEED, MOBILITY, ENERGY, ENERGY_RECOVERY,
	REPAIR, TECHNOLOGY
};

// Using a map for flexibility, though a struct could be faster if stats are fixed
using Stats = std::map<StatType, double>;

// Helper to safely get a state value (returns 0 if not present)
inline double getStat(const Stats& stats, StatType type) {
	auto it = stats.find(type);
	return (it != stats.end()) ? it->second : 0.0;
}

// Helper to add/update a stat
inline void addStat(Stats& stats, StatType type, double value) {
	stats[type] += value; // operator[] creates if not exists, initialized to 0
}

enum class EquipmentSlot {
	HEAD, CHEST, ARMS, LEGS, GENERATOR,
	LEFT_ARM_WEAPON, RIGHT_ARM_WEAPON,
	LEFT_SHOULDER_WEAPON, RIGHT_SHOULDER_WEAPON,
	NONE
};

enum class Rarity {
	COMMON, UNCOMMON, RARE, LEGENDARY
};

std::string rarityToString(Rarity r);
Rarity stringToRarity(const std::string& s);

#endif // STATS_H
