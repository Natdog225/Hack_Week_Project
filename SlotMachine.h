#ifndef SLOTMACHINE_H
#define SLOTMACHINE_H

#include "Reel.h"
#include "Symbol.h"
#include <vector>

/**
 * @brief Manages the overall state and logic of the slot machine game.
 */
class SlotMachine
{
public:
	// Initializes the machine with a specific number of reels and starting credits. Which we'll adjust as needed
	explicit SlotMachine(int numReels = 3, int startingCredits = 100);

	// Spin all the reels
	// Returns true if the spin had enough money, false otherwise.
	bool spinReels();

	// Check if the current combination on the payline is a winning one
	bool checkWinCondition();

	// Get current credit balance
	int getCredits() const;

	// Get the symbol currently showing on a specific reel's payline
	Symbol getSymbolAt(int reelIndex) const;

	// Get the number of reels
	int getReelCount() const;

private:
	// Helper function to load default symbols onto the reels
	void initializeReels();

	std::vector<Reel> reels;   // individual Reel objects
	int credits;			   // current credit balance
	int currentBet;			   // Cost per spin (Fixed for now, but it'd be cool to allow custom betting)
	const int WIN_AMOUNT = 10; // Amount awarded for a simple win (fixed for MVP, again, future customization)
};

#endif // SLOTMACHINE_H
