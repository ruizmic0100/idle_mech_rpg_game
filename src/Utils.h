#ifndef UTILS_H
#define UTILS_H

#include <random> // For std::mt19937, std::uniform_real_distribution
#include <chrono> // For seeding the random number generator
#include <stdexcept> // For std::runtime_error

#include "Stats.h"

// Generates a random doublewithin the range [min, max] (inclusive)
inline double myRandomDouble(double min, double max) {
	// Static ensures the random engine and distribution are initialized only once
	//for the lifetime of the program,which is generally more efficient
	// and provides better random sequences than re-initializing on every call.
	// Thread_local makes it safe if this function could be called from multiple threads
	// concurrently, giving each thread its own RNG state. If you're sure it's
	// single-threaded access or you handle synchronization elsewhere, 'static' is fine.
	thread_local static std::mt19937 rng(
		static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count())
	);

	// if min > max, swap them to ensure correct distribution behaviour
	if (min > max) {
		std::swap(min, max);
	}

	std::uniform_real_distribution<double> dist(min, max);
	return dist(rng);
}

inline std::string rarityToString(Rarity r) {
	switch (r) {
		case Rarity::COMMON:
			return std::string("COMMON");
		case Rarity::UNCOMMON:
			return std::string("UNCOMMON");
		case Rarity::RARE:
			return std::string("RARE");
		case Rarity::LEGENDARY:
			return std::string("LEGENDARY");
		default:
			return std::string("UNKNOWN");
	}
}

inline Rarity stringToRarity(const std::string& s) {
	if (s == "COMMON") return Rarity::COMMON;
	if (s == "UNCOMMON") return Rarity::UNCOMMON;
	if (s == "RARE") return Rarity::RARE;
	if (s == "LEGENDARY") return Rarity::LEGENDARY;
	throw std::runtime_error("Unknown Rarity string: " + s);
}


#endif // UTILS_H
