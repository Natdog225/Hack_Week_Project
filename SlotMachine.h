#ifndef SLOTMACHINE_H
#define SLOTMACHINE_H

#include "Reel.h"
#include "Symbol.h"
#include <vector>
#include <string>
#include <numeric>
#include <map>     // Potentially useful for paytable structure later

struct PayoutRule {
	std::vector<std::string> combination;
	int payoutAmount; // Payout for a 1-credit bet
	PayoutRule(std::vector<std::string> combo, int payout)
		: combination(std::move(combo)), payoutAmount(payout) {}
};


class SlotMachine {
public:
	// Constructor
	explicit SlotMachine(int numReels = 3, int startingCredits = 100);

	// --- Betting ---
	// Set the bet amount for the next spin.
	bool setSelectedBet(int bet);
	int getSelectedBet() const;
	// Get the list of allowed bet amounts (for GUI).
	std::vector<int> getAllowedBets() const;

	// --- Gameplay ---
	// Spin all the reels (deducts selectedBet).
	bool spinReels();
	bool checkWinCondition();

	// --- State Accessors ---
	int getCredits() const;
	Symbol getSymbolAt(int reelIndex) const;
	int getReelCount() const;

private:
	void initializeReels(); // Helper to load symbols
	void initializePayoutTable(); // Helper to setup paytable rules

	std::vector<Reel> reels;
	int credits;
	int selectedBet; // Changed from currentBet
	std::vector<int> allowedBets; // Store allowed bet values
	std::vector<PayoutRule> payoutTable; // Store the payout rules
};

#endif // SLOTMACHINE_H
