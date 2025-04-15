#ifndef PAYOUTRULE_H
#define PAYOUTRULE_H

#include <vector>
#include <string>
#include "Symbol.h"

/**
 * @brief Represents a single rule for a winning combination and its payout.
 */
struct PayoutRule
{
	// The combination of symbols that trigger this win.
	std::vector<std::string> combination;

	// The amount of credits awarded
	int payoutAmount;

	// Add other fields later if needed, like payline index, wilds or whatevs

	// Constructor for convenience
	PayoutRule(std::vector<std::string> combo, int payout)
		: combination(std::move(combo)), payoutAmount(payout) {}
};

#endif // PAYOUTRULE_H
