#ifndef REEL_H
#define REEL_H

#include "Symbol.h"
#include <vector> // To store the symbols
#include <random>

/**
 * @brief Represents a single reel in a slot machine.
 */
class Reel
{
public:
	// Constructor
	Reel();

	// Add a symbol to the reel strip
	void addSymbol(const Symbol &symbol);

	// Set the entire symbol strip at once
	void setSymbolStrip(const std::vector<Symbol> &strip);

	// Spin the reel WOO
	void spin();

	// Get the symbol
	Symbol getCurrentSymbol() const;

	// Gets the symbol visually above the current one (wraps around)
	Symbol getSymbolAbove() const;
	// Gets the symbol visually below the current one (wraps around)
	Symbol getSymbolBelow() const;
	// Helper to get symbol at a specific offset from current (handles wrap)
	Symbol getSymbolAtOffset(int offset) const;

	// Get the entire symbol strip for drawing reels
	const std::vector<Symbol> &getSymbolStrip() const;

private:
	std::vector<Symbol> symbolStrip; // The sequence of symbols on this reel
	int currentSymbolIndex;			 // Index of the symbol currently showing

	// Random number generation members
	std::mt19937 randomGenerator;
};

#endif // REEL_H
