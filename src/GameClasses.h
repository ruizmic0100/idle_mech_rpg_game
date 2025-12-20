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
	   pC.passive.description = "I AM TANKKKKK!!!!";
	}
	// --- 2. THE ACE (DPS/Speed) ---
	else if (classId == "ace") {

	}
	// --- 3. THE TECHNOCRAT (Scaling) ---
	else if (classId == "technocrat") {

	}

	return pC;
   }
};
#endif // GAMECLASSES_H 
