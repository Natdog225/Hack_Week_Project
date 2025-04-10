#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>

/**
 * @brief Represents a single symbol on a slot machine reel.
 */
struct Symbol
{
	// identifier for the symbol (e.g., "CHERRY", "LUMI", "SEVEN" or whatever)
	std::string id;

	// Placeholder for the path to image file.
	std::string imagePath;

	// Potential future ideas.
	//  int payoutValue; // How much this symbol pays in certain combinations
	//  int rarityWeight; // To influence how often it appears

	// Default constructor
	Symbol() = default;

	// Constructor for easy initialization
	Symbol(std::string symbolId, std::string path = "") : id(symbolId), imagePath(path) {}

	// Overload equality operator for easy comparison
	bool operator==(const Symbol &other) const
	{
		return id == other.id;
	}
	// Overload inequality operator
	bool operator!=(const Symbol &other) const
	{
		return !(*this == other);
	}
};

#endif
