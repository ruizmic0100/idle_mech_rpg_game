#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <memory>
#include "Stats.h"

struct ItemTemplate { // Data loaded from JSON
	std::string id;
	std::string name;
	std::string description;
	EquipmentSlot slot = EquipmentSlot::NONE;
	Stats base_stats;
	int required_tech = 0; // Technology requirement

};

class Item {
public:
	Item(std::shared_ptr<const ItemTemplate> t, Rarity r);

	std::string getName() const;
	std::string getDescription() const;
	EquipmentSlot getSlot() const;
	Rarity getRarity() const;
	const Stats& getStats() const;
	int getRequiredTech() const;
	std::string getId() const; // Returns template ID

	void generateInstanceStats(); // Applies rarity modifiers

private:
	std::shared_ptr<const ItemTemplate> item_template;
	Rarity rarity;
	Stats instance_stats; // The actual stats after rarity roll
};

#endif // ITEM_H
