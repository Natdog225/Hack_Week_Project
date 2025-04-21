#include "SlotMachine.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // For std::find

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
		// Using user's message here
		throw std::invalid_argument("Mvp currently goes for 3 reels");
	}

	// Define allowed bets (kept from previous logic)
	allowedBets = {1, 5, 10, 15, 20, 25, 50, 100};
	selectedBet = allowedBets[0]; // Default to the lowest bet

	initializeReels();		 // Load symbols onto the reels
	initializePayoutTable(); // Setup paytable rules
}

// Helper to load symbols (Using user's symbol list example)
void SlotMachine::initializeReels()
{
	// Using placeholders for now. Ensure "RICK_ROLL" and "LUMI_MASCOT" are included.
	std::vector<Symbol> currentSymbols = {
		Symbol("CHERRY"), Symbol("LEMON"), Symbol("SEVEN"),
		Symbol("COMPUTER"), Symbol("FLOPPY_DISK"), Symbol("FOLDER_ICON"),
		Symbol("MOTHERBOARD"), Symbol("COFFEE"), Symbol("ROBOT"),
		Symbol("FLY_SWATTER"), Symbol("PACKAGE"), Symbol("WRENCH"),
		Symbol("MAGNIFYING_GLASS"), Symbol("ERASER"), Symbol("FIRE_EXTINGUISHER"),
		Symbol("TRAP"), Symbol("BSOD"), Symbol("LUMI_MASCOT"), Symbol("RICK_ROLL") // Wild
		// Adjust frequencies by adding duplicates as needed
	};

	// Load these symbols onto each reel's strip
	for (Reel &reel : reels)
	{
		reel.setSymbolStrip(currentSymbols);
	}
}

// Helper to setup paytable rules (Using user's payout rule examples)
void SlotMachine::initializePayoutTable()
{
	// IMPORTANT: Update rules based on final 16 symbols and desired payouts!
	// Use placeholder IDs for now. Ensure they match initializeReels placeholders.
	payoutTable = {
		// Highest value for LUMI_MASCOT
		PayoutRule({"LUMI_MASCOT", "LUMI_MASCOT", "LUMI_MASCOT"}, 250), // Example high payout
		// Other examples
		PayoutRule({"SEVEN", "SEVEN", "SEVEN"}, 100),
		PayoutRule({"COMPUTER", "COMPUTER", "COMPUTER"}, 50),
		PayoutRule({"CHERRY", "CHERRY", "CHERRY"}, 20),
		PayoutRule({"LEMON", "LEMON", "LEMON"}, 10)
		// Add rules for all 16 symbols, potentially 2-symbol combos etc.
		// Add rule for 3 Wilds ("RICK_ROLL")
		// PayoutRule({"RICK_ROLL", "RICK_ROLL", "RICK_ROLL"}, 150),
	};
	std::cout << "SlotMachine initialized with " << payoutTable.size() << " payout rules." << std::endl;
}

// --- Betting Methods ---
bool SlotMachine::setSelectedBet(int bet)
{
	// Check if the chosen bet is in the allowed list
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

int SlotMachine::getSelectedBet() const
{
	return selectedBet;
}

std::vector<int> SlotMachine::getAllowedBets() const
{
	return allowedBets;
}

// --- Gameplay Methods ---
// Spin all reels if khajit has the coin
bool SlotMachine::spinReels()
{
	if (credits < selectedBet)
	{
		std::cerr << "Not enough credits to spin!" << std::endl; // Use cerr for errors cause fuck c++
		return false;											 // YABROKE
	}

	credits -= selectedBet; // Use selectedBet
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
		// Using user's message
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

		// Compare the current combination with the win combo (Simple comparison for now)
		if (currentCombinationIds == rule.combination)
		{
			// Found a winning match!
			int payout = rule.payoutAmount * selectedBet; // Keep scaled payout logic
			// Using user's message
			std::cout << "Winner!!! Base Payout: " << rule.payoutAmount << ", Bet: " << selectedBet << ", Total Payout: " << payout << std::endl;
			credits += payout; // MONEY
			std::cout << "New credit balance: " << credits << std::endl;
			return true; // Win
		}
		// PLACEHOLDER FOR MORE COMPLEX LOGIC. LIKE WILDCARDS.
	}

	// Not winning
	std::cout << "You are a loser and you bring shame to your family." << std::endl;
	return false;
}

// --- State Accessors ---
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
