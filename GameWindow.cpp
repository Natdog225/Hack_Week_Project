#include "GameWindow.h"
#include "SlotMachine.h"
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>
#include <iostream>
#include <stdexcept>

// Constructor
GameWindow::GameWindow(SlotMachine *machineLogic, QWidget *parent)
	: QWidget(parent), slotMachine(machineLogic)
{
	if (!slotMachine)
	{
		throw std::runtime_error("Requires Valid SlotMachine Instance");
	}
	setupUI();
	updateDisplay(); // Set initial display state correctly
}

// Helper function to create and arrange UI elements
void GameWindow::setupUI()
{
	// --- Create Widgets ---
	reelLayout = new QHBoxLayout();										// Layout for reels
	const int numReels = slotMachine ? slotMachine->getReelCount() : 3; // Get actual reel count
	if (numReels > 3)
	{
		std::cerr << "Warning: currently only supports displaying 3 reels." << std::endl;
	}

	for (int i = 0; i < numReels && i < 3; ++i)
	{
		reelLabels[i] = new QLabel("---");			  // Placeholder
		reelLabels[i]->setFixedSize(100, 100);		  // Give reels a fixed size
		reelLabels[i]->setAlignment(Qt::AlignCenter); // Center text
		reelLabels[i]->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		reelLabels[i]->setLineWidth(2);
		reelLayout->addWidget(reelLabels[i]);
	}

	spinButton = new QPushButton("Spin");
	creditsLabel = new QLabel("Credits: ???");		   // Will be updated by updateDisplay
	messageLabel = new QLabel("Welcome! Good Luck! Press Spin to Play.");
	messageLabel->setAlignment(Qt::AlignCenter);

	// --- Create Main Layout ---
	mainLayout = new QVBoxLayout(this);	 // Main layout
	mainLayout->addLayout(reelLayout);	 // Add the reel layout first
	mainLayout->addWidget(spinButton);	 // Add the spin button
	mainLayout->addWidget(creditsLabel); // Add the credits label
	mainLayout->addWidget(messageLabel); // Add the message label

	// --- Set Window Properties ---
	setWindowTitle("Slot Machine MVP");

	// --- Connect Signals and Slots ---
	connect(spinButton, &QPushButton::clicked, this, &GameWindow::handleSpinButtonClicked);
}

// Slot - This is the core logic
void GameWindow::handleSpinButtonClicked()
{
	if (!slotMachine)
		return; // Safety check

	std::cout << "Good Luck!" << std::endl;

	// Attempt to spin the reels
	if (slotMachine->spinReels())
	{
		// Spin was successful (enough credits)
		bool won = slotMachine->checkWinCondition(); // Check for wins

		// Update the entire display based on the new state
		updateDisplay();

		// Set the message based on win/loss
		if (won)
		{
		messageLabel->setText("WINNER!");
			// Add logic to flash the message or play a sound later
		}
		else
		{
			messageLabel->setText("Try Again Loser");
		}
	}
	else
	{
		// Spin failed (broke)
		messageLabel->setText("Not enough credits!");
		// Optional future feature: Disable spin button until credits are added like real machines
		// spinButton->setEnabled(false);
	}
}

// Helper function to update UI elements
void GameWindow::updateDisplay()
{
	if (!slotMachine)
		return;

	// Update reel labels based on the current symbols in SlotMachine
	int reelCount = slotMachine->getReelCount();
	for (int i = 0; i < reelCount && i < 3; ++i)
	{
		if (reelLabels[i])
		{
			try
			{
				Symbol currentSymbol = slotMachine->getSymbolAt(i);
				// Update text - This is where Deshon would load images later
				reelLabels[i]->setText(QString::fromStdString(currentSymbol.id));
			}
			catch (const std::exception &e)
			{
				// Handle cases where getting symbol fails
				std::cerr << "Error updating reel display " << i << ": " << e.what() << std::endl;
				reelLabels[i]->setText("ERR");
			}
		}
	}

	// Update credits label
	creditsLabel->setText("Credits: " + QString::number(slotMachine->getCredits()));

}
