#include "Reel.h"
#include <stdexcept>
#include <chrono>

// Constructor
Reel::Reel() : currentSymbolIndex(0)
{
	// Seed the random number generator.
	unsigned seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	randomGenerator.seed(seed);
}

// Add a single symbol
void Reel::addSymbol(const Symbol &symbol)
{
	symbolStrip.push_back(symbol);
}

// Set the entire symbol strip
void Reel::setSymbolStrip(const std::vector<Symbol> &strip)
{
	symbolStrip = strip;
	if (!symbolStrip.empty())
	{
		// Reset index if strip was previously empty changed size
		currentSymbolIndex = 0;
	}
	else
	{
		currentSymbolIndex = -1; // empty reel
	}
}

void Reel::spin()
{
	if (symbolStrip.empty())
	{
		currentSymbolIndex = -1; // Or throw an exception/errror?
		return;
	}

	// based on the current size of the symbol strip
	std::uniform_int_distribution<int> distribution(0, symbolStrip.size() - 1);

	// random index
	currentSymbolIndex = distribution(randomGenerator);
}

Symbol Reel::getCurrentSymbol() const
{
	if (currentSymbolIndex < 0 || currentSymbolIndex >= symbolStrip.size())
	{
		return Symbol("INVALID", "");
	}
	return symbolStrip.at(currentSymbolIndex); // .at() provides bounds checking
}
Symbol Reel::getSymbolAbove() const
{
	return getSymbolAtOffset(-1);
}

// Gets the symbol visually below the current one (index + 1, wraps around)
Symbol Reel::getSymbolBelow() const
{
	return getSymbolAtOffset(1);
}

// Helper to get symbol at a specific offset from current (handles wrap)
Symbol Reel::getSymbolAtOffset(int offset) const
{
	if (symbolStrip.empty() || currentSymbolIndex < 0)
	{
		// Return a default/empty symbol or throw
		// std::cerr << "Attempted to get symbol from empty or unspun reel." << std::endl;
		return Symbol("INVALID", ""); // Or throw std::out_of_range("Reel empty or index invalid");
	}

	int stripSize = static_cast<int>(symbolStrip.size());
	// Calculate the target index with wrap-around using modulo
	// Adding stripSize before modulo handles negative results correctly
	int targetIndex = (currentSymbolIndex + offset + stripSize) % stripSize;

	return symbolStrip.at(targetIndex); // .at() provides bounds checking (though modulo should prevent issues)
}

const std::vector<Symbol> &Reel::getSymbolStrip() const
{
	return symbolStrip;
}
