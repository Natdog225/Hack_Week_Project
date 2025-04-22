#include "SlotMachine.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // For std::find
#include <functional>

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
	allowedBets = {1, 5, 10, 15, 20, 25, 50, 75, 100, 250, 500, 750, 1000};
	selectedBet = allowedBets[0]; // Default to the lowest bet

	initializeReels();		 // Load symbols onto the reels
	initializePayoutTable(); // Setup paytable rules
	// SORTING LOGIC
	std::sort(payoutTable.begin(), payoutTable.end(),
			  [](const PayoutRule &a, const PayoutRule &b)
			  {
				  // Sort highest payout first
				  return a.payoutAmount > b.payoutAmount;
			  });
	std::cout << "Payout table sorted by payout (descending)." << std::endl;
}

// Helper to load final symbols onto the reels
void SlotMachine::initializeReels()
{
	// Define symbols
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
int SlotMachine::checkLineAgainstRules(const std::vector<std::string> &lineSymbolIds) const
{
	// Iterate through the payout rules (ASSUMED SORTED high-to-low payout)
	for (const PayoutRule &rule : payoutTable)
	{
		// Skip rule if its combination length doesn't match the line length
		if (rule.combination.size() != lineSymbolIds.size())
		{
			continue;
		}
		// Comparison logic with wildcard
		bool ruleMatches = true;
		for (size_t i = 0; i < lineSymbolIds.size(); ++i)
		{
			const std::string &currentSymbolId = lineSymbolIds[i];
			const std::string &requiredSymbolId = rule.combination[i];
			if (currentSymbolId != requiredSymbolId && currentSymbolId != WILD_SYMBOL_ID)
			{
				ruleMatches = false;
				break;
			}
		}

		// If rule matches, return the base payout amount for this line
		if (ruleMatches)
		{
			std::cout << "  Line Win! Matched Rule: [" << rule.combination[0] << ", ...]. Base Payout: " << rule.payoutAmount << std::endl;
			return rule.payoutAmount; // Return the payout for this line
		}
	}
	return 0; // No rule matched this line
}

// Check for a win - *** WITH WILDCARD LOGIC ***
int SlotMachine::checkWinCondition()
{
	const int numReelsToCheck = 3; // Assuming 3 reels for diagonal logic
	if (reels.size() != numReelsToCheck)
	{
		std::cerr << "Error: checkWinCondition assumes " << numReelsToCheck << " reels." << std::endl;
		return 0; // Return 0 payout on error
	}

	int totalBasePayout = 0;									   // Changed variable name for clarity
	std::vector<std::vector<Symbol>> visibleGrid(numReelsToCheck); // Store the full 3x3 grid Symbols

	// Get symbols for the entire 3x3 grid
	try
	{
		for (int r = 0; r < numReelsToCheck; ++r)
		{										   // For each reel
			visibleGrid[r] = getVisibleSymbols(r); // Get [above, current, below]
			if (visibleGrid[r].size() != 3)
			{
				std::cerr << "Error: getVisibleSymbols returned unexpected size for reel " << r << std::endl;
				return 0; // Error state, return 0 payout
			}
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error getting symbols for win check: " << e.what() << std::endl;
		return 0; // Return 0 payout on error
	}

	// --- Check Horizontal Lines ---
	std::vector<std::string> lineSymbolIds(numReelsToCheck);

	// Top Line (row 0)
	for (int r = 0; r < numReelsToCheck; ++r)
		lineSymbolIds[r] = visibleGrid[r][0].id;
	std::cout << "Checking Top Line:    [" << lineSymbolIds[0] << ", " << lineSymbolIds[1] << ", " << lineSymbolIds[2] << "]" << std::endl;
	totalBasePayout += checkLineAgainstRules(lineSymbolIds);

	// Middle Line (row 1)
	for (int r = 0; r < numReelsToCheck; ++r)
		lineSymbolIds[r] = visibleGrid[r][1].id;
	std::cout << "Checking Middle Line: [" << lineSymbolIds[0] << ", " << lineSymbolIds[1] << ", " << lineSymbolIds[2] << "]" << std::endl;
	totalBasePayout += checkLineAgainstRules(lineSymbolIds);

	// Bottom Line (row 2)
	for (int r = 0; r < numReelsToCheck; ++r)
		lineSymbolIds[r] = visibleGrid[r][2].id;
	std::cout << "Checking Bottom Line: [" << lineSymbolIds[0] << ", " << lineSymbolIds[1] << ", " << lineSymbolIds[2] << "]" << std::endl;
	totalBasePayout += checkLineAgainstRules(lineSymbolIds);

	// --- Check Diagonal Lines ---

	// Diagonal 1 (Top-Left to Bottom-Right)
	lineSymbolIds[0] = visibleGrid[0][0].id;
	lineSymbolIds[1] = visibleGrid[1][1].id;
	lineSymbolIds[2] = visibleGrid[2][2].id;
	std::cout << "Checking Diagonal TL-BR: [" << lineSymbolIds[0] << ", " << lineSymbolIds[1] << ", " << lineSymbolIds[2] << "]" << std::endl;
	totalBasePayout += checkLineAgainstRules(lineSymbolIds);

	// Diagonal 2 (Top-Right to Bottom-Left)
	lineSymbolIds[0] = visibleGrid[2][0].id;
	lineSymbolIds[1] = visibleGrid[1][1].id;
	lineSymbolIds[2] = visibleGrid[0][2].id;
	std::cout << "Checking Diagonal TR-BL: [" << lineSymbolIds[0] << ", " << lineSymbolIds[1] << ", " << lineSymbolIds[2] << "]" << std::endl;
	totalBasePayout += checkLineAgainstRules(lineSymbolIds);

	// Apply bet multiplier and award credits
	if (totalBasePayout > 0)
	{
		int finalAward = totalBasePayout * selectedBet;
		std::cout << "WINNER!!! Total Base Payout (All Lines): " << totalBasePayout << ", Bet: " << selectedBet << ", Total Award: " << finalAward << std::endl;
		credits += finalAward; // MONEY
		std::cout << "New credit balance: " << credits << std::endl;
		return finalAward; // Return the actual amount awarded
	}
	else
	{
		// Not winning
		std::cout << "You are a loser and you bring shame to your family." << std::endl;
		return 0; // Return 0 payout
	}
}

// State Accessors (getCredits, getReelCount same)
int SlotMachine::getCredits() const { return credits; }
int SlotMachine::getReelCount() const { return reels.size(); }
// getSymbolAt still returns the CENTER symbol
Symbol SlotMachine::getSymbolAt(int reelIndex) const
{
	if (reelIndex < 0 || reelIndex >= reels.size())
	{
		throw std::out_of_range("Reel index out of bounds.");
	}
	try
	{
		return reels.at(reelIndex).getCurrentSymbol();
	} // getCurrentSymbol uses offset 0
	catch (const std::out_of_range &e)
	{
		std::cerr << "Error accessing symbol on reel " << reelIndex << ": " << e.what() << std::endl;
		throw;
	}
}

std::vector<Symbol> SlotMachine::getVisibleSymbols(int reelIndex) const
{
	if (reelIndex < 0 || reelIndex >= reels.size())
	{
		throw std::out_of_range("Reel index out of bounds for getVisibleSymbols.");
	}
	try
	{
		// Get the symbols using the new Reel methods
		Symbol above = reels.at(reelIndex).getSymbolAbove();
		Symbol current = reels.at(reelIndex).getCurrentSymbol();
		Symbol below = reels.at(reelIndex).getSymbolBelow();
		return {above, current, below};
	}
	catch (const std::exception &e)
	{
		// Handle potential errors from Reel methods
		std::cerr << "Error getting visible symbols for reel " << reelIndex << ": " << e.what() << std::endl;
		// Return a vector of invalid symbols or re-throw
		return {Symbol("ERR", ""), Symbol("ERR", ""), Symbol("ERR", "")};
	}
}
