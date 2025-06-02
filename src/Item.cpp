#include <iostream>
#include <random>
#include <chrono> // For seeding random

#include "Item.h"

// --- Getters ---
std::string Item::getName() const {
	return item_template->name;
}

std::string Item::getDescription() const {
	return item_template->description;
}

EquipmentSlot Item::getSlot() const {
	return item_template->slot;
}

Rarity Item::getRarity() const {
	return rarity;
}

const Stats& Item::getStats() const {
	return instance_stats;
}

int Item::getRequiredTech() const {
	return getStat(item_template->base_stats, StatType::TECHNOLOGY);
}

std::string Item::getId() const {
	return item_template->id;
}


// --- Setters ---

// --- Constructors ---
// Takes a shared pointer to a const ItemTemplate and a Rarity
Item::Item(std::shared_ptr<const ItemTemplate> t, Rarity r) : item_template(t), rarity(r) {
	if (!item_template) {
		// This should ideally not happen if item loading and generation logic is correct.
		// To handle error will be throwing and exception
		throw std::runtime_error("Item constructor: ItemTemplate pointer is null.");
	}

	// After initializing the tempalte and rarity, generate the specific stats for this instance
	generateInstanceStats();

	std::cout << "Created Item: " << getName() << " (" << rarityToString(getRarity()) << ")" << std::endl;
}


void Item::generateInstanceStats() {
	instance_stats = item_template->base_stats; // Start with base

	double variation_percent = 0.0;
	double flat_bonus_mult = 1.0; // Multiplier for base stats

	switch (rarity) {
		case Rarity::COMMON:	variation_percent = 0.10; break; // +/- 10%
		case Rarity::UNCOMMON:  variation_percent = 0.25; flat_bonus_mult = 1.25; break; // +/- 25% + 25% base
		case Rarity::RARE:		variation_percent = 0.50; flat_bonus_mult = 1.75; break; // +/- 50% + 75% base
		case Rarity::LEGENDARY: variation_percent = 1.00; flat_bonus_mult = 2.25; break; // +/- 100%, +125% base
	}

	Stats final_stats;
	for (const auto& pair : item_template->base_stats) {
		StatType type = pair.first;
		double base_value = pair.second;

		// Apply flat bonus multiplier first (only for Uncommon+)
		double modified_base = (rarity == Rarity::COMMON) ? base_value : base_value * flat_bonus_mult;

		// Apply percentage variation
		double variation = myRandomDouble(-variation_percent, variation_percent); // e.g., random between -0.1 and 0.1
		double final_value = modified_base * (1.0 + variation);

		// Ensure stats don't go negative if base was non-negative
		if (base_value >= 0 && final_value < 0) {
			final_value = 0;
		}
		// Armor should probably be capped (e.g., 0% to 90%)
		if (type == StatType::ARMOR) {
			final_value = std::max(0.0, std::min(0.9, final_value)); // Example cap 0-90%
		}

		final_stats[type] = final_value;
	}
	instance_stats = final_stats; // Assign calculated stats
}
