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

	// Get the entire symbol strip for drawing reels
	const std::vector<Symbol> &getSymbolStrip() const;

private:
	std::vector<Symbol> symbolStrip; // The sequence of symbols on this reel
	int currentSymbolIndex;			 // Index of the symbol currently showing

	// Random number generation members
	std::mt19937 randomGenerator;
};

#endif // REEL_H
