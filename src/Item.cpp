#include <random>
#include <chrono> // For seeding random

#include "Item.h"

inline double randomDouble(double min, double max)
{
	static std::mt19937 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	std::uniform_real_distribution<double> dist(min, max);
	return dist(rng);
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
		double variation = randomDouble(-variation_percent, variation_percent); // e.g., random between -0.1 and 0.1
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
