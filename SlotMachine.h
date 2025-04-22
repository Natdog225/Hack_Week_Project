#ifndef SLOTMACHINE_H
#define SLOTMACHINE_H

#include "Reel.h"
#include "Symbol.h"
#include <vector>
#include <string>
#include <numeric>
#include <map>

struct PayoutRule
{
	std::vector<std::string> combination;
	int payoutAmount; // Payout for a 1-credit bet
	PayoutRule(std::vector<std::string> combo, int payout)
		: combination(std::move(combo)), payoutAmount(payout) {}
};

class SlotMachine
{
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
	int checkWinCondition();

	// --- State Accessors ---
	int getCredits() const;
	Symbol getSymbolAt(int reelIndex) const; // Gets Center symbol
	int getReelCount() const;
	// Gets the 3 visible symbols (above, current, below) for a specific reel.
	std::vector<Symbol> getVisibleSymbols(int reelIndex) const;

private:
	void initializeReels();
	void initializePayoutTable();
	int checkLineAgainstRules(const std::vector<std::string>& lineSymbolIds) const; //Checks a single line against the paytable.
	static const std::string WILD_SYMBOL_ID;

	std::vector<Reel> reels;
	int credits;
	int selectedBet;					 // Changed from currentBet
	std::vector<int> allowedBets;		 // Store allowed bet values
	std::vector<PayoutRule> payoutTable; // Store the payout rules
};

#endif // SLOTMACHINE_H
