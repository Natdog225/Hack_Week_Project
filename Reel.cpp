#include "Reel.h"
#include <stdexcept>
#include <chrono>

// Constructor
Reel::Reel(QObject *parent) : QObject(parent), currentSymbolIndex(0), spinDuration(2000), elapsedSpinTime(0)
{
	// Seed the random number generator.
	unsigned seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	randomGenerator.seed(seed);

	// Connect the timer to updateSpin
	connect(&spinTimer, &QTimer::timeout, this, &Reel::updateSpin);
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

void Reel::startSpin()
{
	if (symbolStrip.empty())
	{
		return;
	}
	elapsedSpinTime = 0;
	spinTimer.start(50); // Update every 50ms
}

void Reel::updateSpin()
{
	if (elapsedSpinTime >= spinDuration)
	{
		spinTimer.stop();
		return;
	}

	std::uniform_int_distribution<int> dist(0, symbolStrip.size() - 1);
	currentSymbolIndex = dist(randomGenerator);

	emit symbolChange(symbolStrip.at(currentSymbolIndex));
	elapsedSpinTime += 50; // Increment elapsed time
}

Symbol Reel::getCurrentSymbol() const
{
	if (currentSymbolIndex < 0 || currentSymbolIndex >= symbolStrip.size())
	{
		return Symbol("INVALID", "");
	}
	return symbolStrip.at(currentSymbolIndex); // .at() provides bounds checking
}

const std::vector<Symbol> &Reel::getSymbolStrip() const
{
	return symbolStrip;
}
