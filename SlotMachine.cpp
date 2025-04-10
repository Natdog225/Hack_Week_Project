#include "SlotMachine.h"
#include <stdexcept>
#include <iostream>

// Constructor
SlotMachine::SlotMachine(int numReels, int startingCredits)
	: reels(numReels), // numReels default Reel objects
	  credits(startingCredits),
	  currentBet(1) // Fixed bet for MVP
{
	if (numReels <= 0) {
		 throw std::invalid_argument("Number of reels must be positive.");
	}
	initializeReels();
}

// Helper to load symbols
void SlotMachine::initializeReels() {
	// Define the symbols available
	std::vector<Symbol> mvpSymbols = {
		Symbol("CHERRY", "path/to/cherry.png"), // placeholders
		Symbol("LEMON", "path/to/lemon.png"),
		Symbol("SEVEN", "path/to/seven.png")
		// Add more symbols to the strip later on
		// Symbol("BAR", "path/to/bar.png"),
		// Symbol("BELL", "path/to/bell.png") 
		//So on, so forth
	};

	// Load these symbols onto each reel's strip
	for (Reel& reel : reels) {
		reel.setSymbolStrip(mvpSymbols);
	}
}

// Spin all reels if khajit has the coin
bool SlotMachine::spinReels() {
	if (credits < currentBet) {
		std::cerr << "Not enough credits to spin!" << std::endl; // Use cerr for errors cause fuck c++
		return false; // YABROKE
	}

	credits -= currentBet;

	// Spin reels
	for (Reel& reel : reels) {
		reel.spin();
	}
	return true;
}

// Check for a win (simple beginning rule: all reels show the same symbol)
bool SlotMachine::checkWinCondition() {
	if (reels.empty()) {
		return false; // No reels to check
	}

	// Get the symbol from the first reel
	Symbol firstSymbol = getSymbolAt(0);
	if (firstSymbol.id == "INVALID") return false; // Handle invalid state

	// Check if all other reels match the first one
	for (int i = 1; i < reels.size(); ++i) {
		if (getSymbolAt(i) != firstSymbol) {
			return false; // Mismatch found, YOULOSE
		}
	}

	// If we get here, all symbols matched OHMYGOD! It's a win.
	std::cout << "WINNER! All symbols match: " << firstSymbol.id << std::endl; // Debug output
	credits += WIN_AMOUNT; // WOO MONEY
	return true;
}

// Get current credits
int SlotMachine::getCredits() const {
	return credits;
}

// Get symbol from a specific reel
Symbol SlotMachine::getSymbolAt(int reelIndex) const {
	if (reelIndex < 0 || reelIndex >= reels.size()) {
		 throw std::out_of_range("Reel index out of bounds.");
		// Or return a default invalid
	}
	return reels.at(reelIndex).getCurrentSymbol();
}

 // Get the number of reels
int SlotMachine::getReelCount() const {
	return reels.size();
}

