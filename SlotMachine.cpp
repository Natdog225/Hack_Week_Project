#include "SlotMachine.h"
#include <stdexcept>
#include <iostream>

SlotMachine::SlotMachine(int numReels, int startingCredits)
	: reels(numReels),
	  credits(startingCredits),
	  currentBet(1) // Fixed bet for MVP
{
	if (numReels <= 0)
	{
		throw std::invalid_argument("Number of reels must be positive.");
	}
	if (numReels != 3)
	{
		throw std::invalid_argument("Mvp currently assumes 3 reels");
	}
	initializeReels();

	// --- Payout Rules ---
	// IMPORTANT: Replace "SEVEN", "CHERRY", "LEMON" with the actual Symbol::id strings
	// finalized by Chepe once available!
	payoutTable = {
		// Rule 1: Three SEVENs pay 100
		PayoutRule({"SEVEN", "SEVEN", "SEVEN"}, 100),
		// Rule 2: Three CHERRYs pay 20
		PayoutRule({"CHERRY", "CHERRY", "CHERRY"}, 20),
		// Rule 3: Three LEMONs pay 10
		PayoutRule({"LEMON", "LEMON", "LEMON"}, 10)
		// Rule 4: Wild card symbol for like 2 lemon + wild = 3 Lemon one.
		// PayoutRule({"*", "*", "*"}, 5) // Requires wildcard handling
	};
	std::cout << "SlotMachine initialized with " << payoutTable.size() << " payout rules." << std::endl;
}

// Helper to load symbols
void SlotMachine::initializeReels()
{
	// Define the symbols available
	std::vector<Symbol> mvpSymbols = {
		Symbol("CHERRY", "path/to/cherry.png"), // placeholders
		Symbol("LEMON", "path/to/lemon.png"),
		Symbol("SEVEN", "path/to/seven.png")
		// Add more symbols to the strip later on
		// Symbol("BAR", "path/to/bar.png"),
		// Symbol("BELL", "path/to/bell.png")
		// So on, so forth
	};

	// Load these symbols onto each reel's strip
	for (Reel &reel : reels)
	{
		reel.setSymbolStrip(mvpSymbols);
	}
}

// Spin all reels if khajit has the coin
bool SlotMachine::spinReels()
{
	if (credits < currentBet)
	{
		std::cerr << "Not enough credits to spin!" << std::endl; // Use cerr for errors cause fuck c++
		return false;											 // YABROKE
	}

	credits -= currentBet;
	std::cout << "Good luck, Credits Remaining: " << credits << std::endl;

	// Spin reels
	for (Reel &reel : reels)
	{
		reel.spin();
	}
	return true;
}

// Check for a win
bool SlotMachine::checkWinCondition()
{
	if (reels.size() != 3)
	{ // Assuming 3 reels for MVP paytable logic
		std::cerr << "Error: checkWinCondition assumes 3 reels for MVP." << std::endl;
		return false;
	}

	// Get the symbols on the center payline
	std::vector<std::string> currentCombinationIds;
	try
	{
		currentCombinationIds.push_back(getSymbolAt(0).id);
		currentCombinationIds.push_back(getSymbolAt(1).id);
		currentCombinationIds.push_back(getSymbolAt(2).id);
	}
	catch (const std::out_of_range &e)
	{
		std::cerr << "Error getting symbols for win : " << e.what() << std::endl;
		return false;
	}

	// Debug: Print current combination
	std::cout << "Checking combination: [" << currentCombinationIds[0] << ", " << currentCombinationIds[1] << ", " << currentCombinationIds[2] << "]" << std::endl;

	// Iterate through the defined payout rules
	for (const PayoutRule &rule : payoutTable)
	{
		// Check if the rule's combination size matches the number of reels
		if (rule.combination.size() != currentCombinationIds.size())
		{
			continue; // Skip rule if size doesn't match
		}

		// Compare the current combination with the win combo
		if (currentCombinationIds == rule.combination)
		{
			// Found a winning match!
			std::cout << "Winner!!! Payout: " << rule.payoutAmount << std::endl;
			credits += rule.payoutAmount; // MONEY
			std::cout << "New credit balance: " << credits << std::endl;
			return true; // Return true indicating a win
		}
		// PLACEHOLDER FOR MORE COMPLEX LOGIC. LIKE WILDCARDS.
	}

	// Not winning
	std::cout << "You are a loser and you bring shame to your family." << std::endl;
	return false;
}

// Get current credits
int SlotMachine::getCredits() const
{
	return credits;
}

// Get symbol from a specific reel
Symbol SlotMachine::getSymbolAt(int reelIndex) const
{
	if (reelIndex < 0 || reelIndex >= reels.size())
	{
		throw std::out_of_range("Reel index out of bounds.");
	}
	// Ensure the reel isn't in an invalid state (e.g., empty)
	try
	{
		return reels.at(reelIndex).getCurrentSymbol(); // .at() = bounds checking
	}
	catch (const std::out_of_range &e)
	{
		std::cerr << "Error accessing symbol on reel " << reelIndex << ": " << e.what() << std::endl;
		// Re-throw or invalid symbol
		throw;
	}
}

// Get the number of reels
int SlotMachine::getReelCount() const
{
	return reels.size();
}
