#ifndef REEL_H
#define REEL_H

#include "Symbol.h"
#include <vector> // To store the symbols
#include <random>
#include <QObject>
#include <QTimer>

/**
 * @brief Represents a single reel in a slot machine.
 */
class Reel : public QObject
{

	Q_OBJECT


public:
	// Constructor
	Reel(QObject *parent = nullptr);

	// Add a symbol to the reel strip
	void addSymbol(const Symbol &symbol);

	// Set the entire symbol strip at once
	void setSymbolStrip(const std::vector<Symbol> &strip);

	//ROund and round and round it goes
	void startSpin();

	// Get the symbol
	Symbol getCurrentSymbol() const;

	// Get the entire symbol strip for drawing reels
	const std::vector<Symbol> &getSymbolStrip() const;

signals:
	// Signals when the reel stops to update the UI
	void symbolChange(const Symbol &symbol);

private slots:
	// Slot to update the current symbol
	void updateSpin();

private:
	std::vector<Symbol> symbolStrip; // The sequence of symbols on this reel
	int currentSymbolIndex;			 // Index of the symbol currently showing

	// Random number generation members
	std::mt19937 randomGenerator;

	QTimer spinTimer; // Animation Timer
	int spinDuration; // How long will it spin
	int elapsedSpinTime; // stores the time since the spin started
};

#endif // REEL_H
