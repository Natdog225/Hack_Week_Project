#include "SlotMachine.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // For std::find

const std::string SlotMachine::WILD_SYMBOL_ID = "RICKROLL";
// Constructor
SlotMachine::SlotMachine(int numReels, int startingCredits)
	: reels(numReels),
	  credits(startingCredits),
	  selectedBet(1)
{
	if (numReels <= 0)
	{
		throw std::invalid_argument("Number of reels must be positive.");
	}
	if (numReels != 3)
	{
		throw std::invalid_argument("Mvp currently goes for 3 reels");
	}

	// Define allowed bets
	allowedBets = {1, 5, 10, 15, 20, 25, 50, 100, 250, 500, 750, 1000};
	selectedBet = allowedBets[0]; // Default to the lowest bet

	initializeReels();		 // Load symbols onto the reels
	initializePayoutTable(); // Setup paytable rules
	// SORTING LOGIC 
	std::sort(payoutTable.begin(), payoutTable.end(),
	[](const PayoutRule& a, const PayoutRule& b) {
		// Sort highest payout first
		return a.payoutAmount > b.payoutAmount;
	});
std::cout << "Payout table sorted by payout (descending)." << std::endl;
}

// Helper to load final symbols onto the reels
void SlotMachine::initializeReels()
{
	// Define the final 16 symbols
	std::vector<Symbol> finalSymbolStrip = {
		// Common Symbols (Example: appear multiple times)
		Symbol("COFFEE", "assets/reel-coffee_mug.svg"),
		Symbol("FLOPPY", "assets/reel-floppy.svg"),
		Symbol("FOLDER", "assets/reel-folder_icon.svg"),
		Symbol("WRENCH", "assets/reel-wrench.svg"),
		Symbol("ERASER", "assets/reel-eraser.svg"),
		Symbol("COFFEE", "assets/reel-coffee_mug.svg"),	 // Duplicate for frequency
		Symbol("FLOPPY", "assets/reel-floppy.svg"),		 // Duplicate
		Symbol("FOLDER", "assets/reel-folder_icon.svg"), // Duplicate

		// Intermediate Symbols
		Symbol("COMPUTER", "assets/reel-computer.svg"),
		Symbol("MOBO", "assets/reel-motherboard.svg"),
		Symbol("ROBOT", "assets/reel-robot.svg"),
		Symbol("PACKAGE", "assets/reel-package.svg"),
		Symbol("MAGNIFIER", "assets/reel-magnifier.svg"),
		Symbol("COMPUTER", "assets/reel-computer.svg"), // Duplicate

		// Less Common / Special Symbols
		Symbol("FLY_SWAT", "assets/reel-fly_swat.svg"),
		Symbol("TRAP", "assets/reel-trap.svg"),
		Symbol("FIRE_EXT", "assets/reel-fire_extinguisher.svg"),
		Symbol("BSOD", "assets/reel-blue_screen.svg"),

		// Wild Symbol (Less frequent)
		Symbol("RICKROLL", "assets/reel-rickroll.svg"),

		// Highest Value Symbol (Rarest)
		Symbol("LUMI", "assets/reel-lumi_mascot.svg")};

	// while (finalSymbolStrip.size() < 20) {
	//     finalSymbolStrip.push_back(finalSymbolStrip[rand() % 5]);
	// }

	for (Reel &reel : reels)
	{
		reel.setSymbolStrip(finalSymbolStrip);
		reel.spin(); // Give it an initial random position
	}
	std::cout << "Reels initialized with " << finalSymbolStrip.size() << " symbols per strip." << std::endl;
}

// Helper to setup paytable rules using symbol IDs
void SlotMachine::initializePayoutTable()
{
	// Define payout rules
	payoutTable = {
		// Highest value for LUMI
		PayoutRule({"LUMI", "LUMI", "LUMI"}, 500),

		// Wildcard rule
		PayoutRule({"RICKROLL", "RICKROLL", "RICKROLL"}, 250),

		// Other 3-of-a-kind examples
		PayoutRule({"BSOD", "BSOD", "BSOD"}, 150),
		PayoutRule({"TRAP", "TRAP", "TRAP"}, 125),
		PayoutRule({"ROBOT", "ROBOT", "ROBOT"}, 100),
		PayoutRule({"MOBO", "MOBO", "MOBO"}, 75),
		PayoutRule({"COMPUTER", "COMPUTER", "COMPUTER"}, 60),
		PayoutRule({"PACKAGE", "PACKAGE", "PACKAGE"}, 50),
		PayoutRule({"FIRE_EXT", "FIRE_EXT", "FIRE_EXT"}, 40),
		PayoutRule({"COFFEE", "COFFEE", "COFFEE"}, 35),
		PayoutRule({"FLOPPY", "FLOPPY", "FLOPPY"}, 30),
		PayoutRule({"FOLDER", "FOLDER", "FOLDER"}, 25),
		PayoutRule({"WRENCH", "WRENCH", "WRENCH"}, 20),
		PayoutRule({"ERASER", "ERASER", "ERASER"}, 15),
		PayoutRule({"FLY_SWAT", "FLY_SWAT", "FLY_SWAT"}, 10),
		PayoutRule({"MAGNIFIER", "MAGNIFIER", "MAGNIFIER"}, 5)};
	std::cout << "SlotMachine initialized with " << payoutTable.size() << " payout rules using IDs." << std::endl;
}

// --- Betting Methods ---
bool SlotMachine::setSelectedBet(int bet)
{
	bool allowed = (std::find(allowedBets.begin(), allowedBets.end(), bet) != allowedBets.end());
	if (allowed)
	{
		selectedBet = bet;
		std::cout << "Bet set to: " << selectedBet << std::endl;
		return true;
	}
	else
	{
		std::cerr << "Invalid bet amount: " << bet << std::endl;
		return false;
	}
}

int SlotMachine::getSelectedBet() const { return selectedBet; }
std::vector<int> SlotMachine::getAllowedBets() const { return allowedBets; }

// --- Gameplay Methods ---
// Spin all reels if khajit has the coin
bool SlotMachine::spinReels()
{
	if (credits < selectedBet)
	{
		std::cerr << "Not enough credits to spin!" << std::endl; // Use cerr for errors cause fuck c++
		return false;											 // YABROKE
	}
	credits -= selectedBet;
	std::cout << "Good luck, Credits Remaining: " << credits << std::endl;
	for (Reel &reel : reels)
	{
		reel.spin();
	}
	return true;
}

// Check for a win - *** WITH WILDCARD LOGIC ***
bool SlotMachine::checkWinCondition()
{
	if (reels.size() != 3)
	{ // Assuming 3 reels. And we'll probably stick with 3 for simplicity 
		std::cerr << "Error: checkWinCondition assumes 3 reels for MVP." << std::endl;
		return false;
	}

	// Get the symbols currently showing on the center payline
	std::vector<Symbol> currentSymbols;
	try
	{
		currentSymbols.push_back(getSymbolAt(0));
		currentSymbols.push_back(getSymbolAt(1));
		currentSymbols.push_back(getSymbolAt(2));
	}
	catch (const std::out_of_range &e)
	{
		std::cerr << "Error getting symbols for win check: " << e.what() << std::endl;
		return false;
	}

	// Extract the IDs for easier comparison
	std::vector<std::string> currentCombinationIds;
	currentCombinationIds.push_back(currentSymbols[0].id);
	currentCombinationIds.push_back(currentSymbols[1].id);
	currentCombinationIds.push_back(currentSymbols[2].id);

	std::cout << "Checking combination: [" << currentCombinationIds[0] << ", " << currentCombinationIds[1] << ", " << currentCombinationIds[2] << "]" << std::endl;

	// Iterate through the payout rules (SORTED high-to-low payout)
	for (const PayoutRule &rule : payoutTable)
	{
		// Skip if its combination length doesn't match reels
		if (rule.combination.size() != currentCombinationIds.size())
		{
			continue;
		}

		// --- Comparison logic with wildcard ---
		bool ruleMatches = true; // Assume match initially
		for (size_t i = 0; i < currentCombinationIds.size(); ++i)
		{
			const std::string &currentSymbolId = currentCombinationIds[i];
			const std::string &requiredSymbolId = rule.combination[i];

			// Check if the current symbol matches the required symbol OR if it's a wild
			if (currentSymbolId != requiredSymbolId && currentSymbolId != WILD_SYMBOL_ID)
			{
				// If it's neither it dont work
				ruleMatches = false;
				break; // No need to check further
			}
		}
		if (ruleMatches)
		{
			int payout = rule.payoutAmount * selectedBet; // Scaled payout
			std::cout << "Winner!!! [" << rule.combination[0] << ", ...]. Base Payout: " << rule.payoutAmount
					  << ", Bet: " << selectedBet << ", Total Payout: " << payout << std::endl;
			credits += payout; // MONEY
			std::cout << "New credit balance: " << credits << std::endl;
			return true;
		}
	}

	// Loser message
	std::cout << "You are a loser and you bring shame to your family." << std::endl;
	return false;
}

// --- State Accessors ---
int SlotMachine::getCredits() const { return credits; }
Symbol SlotMachine::getSymbolAt(int reelIndex) const
{
	if (reelIndex < 0 || reelIndex >= reels.size())
	{
		throw std::out_of_range("Reel index out of bounds.");
	}
	try
	{
		return reels.at(reelIndex).getCurrentSymbol();
	}
	catch (const std::out_of_range &e)
	{
		std::cerr << "Error accessing symbol on reel " << reelIndex << ": " << e.what() << std::endl;
		throw;
	}
}
int SlotMachine::getReelCount() const { return reels.size(); }
