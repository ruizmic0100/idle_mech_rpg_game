#ifndef GAMECLASSES_H
#define GAMECLASSES_H

#include <string>
#include <iostream>

#include "Mech.h"

enum class ClassArchetype {
	Bulwark,
	Ace,
	Technocrat,
	None
};

struct ClassPassive {
   std::string id;
   std::string description;
};

struct PilotClass {
   std::string id;
   ClassArchetype archetype;
   Stats stats;
   ClassPassive passive;
};


class PilotClassFactory {
public:
   static PilotClass createPilotClass(std::string classId) {
	PilotClass pC;
	pC.id = classId;
	pC.archetype = ClassArchetype::None;

	// --- 1. THE BULWARK (Tank) ---
	if (classId == "bulwark") {
	   pC.archetype = ClassArchetype::Bulwark;
	   pC.stats = {
		{StatType::HEALTH, 250},
		{StatType::ARMOR, 10},
		{StatType::ENERGY_SHIELD, 50},
		{StatType::ATTACK, 2},
		{StatType::ATTACK_SPEED, 1},
		{StatType::MOBILITY, 1},
		{StatType::ENERGY, 1},
		{StatType::ENERGY_RECOVERY, 1},
		{StatType::REPAIR, 5},
		{StatType::TECHNOLOGY, 1},
	   };

	   pC.passive.id = "TANK MODE";
	   pC.passive.description = "I AM TANKKKKK!!!";
	}
	// --- 2. THE ACE (DPS/Speed) ---
	else if (classId == "ace") {
	   pC.archetype = ClassArchetype::Ace;
	   pC.stats = {
		{StatType::HEALTH, 150},
		{StatType::ARMOR, 4},
		{StatType::ENERGY_SHIELD, 30},
		{StatType::ATTACK, 5},
		{StatType::ATTACK_SPEED, 2.5},
		{StatType::MOBILITY, 8},
		{StatType::ENERGY, 20},
		{StatType::ENERGY_RECOVERY, 2},
		{StatType::REPAIR, 1},
		{StatType::TECHNOLOGY, 2},
	   };

	   pC.passive.id = "OVERCLOCK";
	   pC.passive.description = "Perfect accuracy; no failure.";
	}
	// --- 3. THE TECHNOCRAT (Utility/Scaling) ---
	else if (classId == "technocrat") {
	   pC.archetype = ClassArchetype::Technocrat;
	   pC.stats = {
		{StatType::HEALTH, 120},
		{StatType::ARMOR, 2},
		{StatType::ENERGY_SHIELD, 100},
		{StatType::ATTACK, 3},
		{StatType::ATTACK_SPEED, 1.2},
		{StatType::MOBILITY, 3},
		{StatType::ENERGY, 50},
		{StatType::ENERGY_RECOVERY, 5},
		{StatType::REPAIR, 2},
		{StatType::TECHNOLOGY, 10},
	   };

	   pC.passive.id = "NANOBOTS";
	   pC.passive.description = "Logic beats brute force.";
	}

	return pC;
   }
};
#endif // GAMECLASSES_H 
