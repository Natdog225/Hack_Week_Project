#include "GameWindow.h"
#include "SlotMachine.h"
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>
#include <QPixmap> 
#include <QDir>
#include <QDebug>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <map>
#include <string>

// Constructor
GameWindow::GameWindow(SlotMachine *machineLogic, QWidget *parent)
	: QWidget(parent), slotMachine(machineLogic), currentBetIndex(0) // Initialize index to 0
{
	if (!slotMachine)
	{
		throw std::runtime_error("Requires Valid Instance");
	}
	// Ensure selectedBet in logic matches initial index
	const auto &bets = slotMachine->getAllowedBets();
	if (!bets.empty())
	{
		slotMachine->setSelectedBet(bets[currentBetIndex]);
	}
	else
	{
		std::cerr << "Warning: SlotMachine provided no allowed bets." << std::endl;
		currentBetIndex = -1;
	}

	setupUI();
	loadSymbolImages();
	updateDisplay(); // Set initial display
}

// Helper function to create and arrange UI elements
void GameWindow::setupUI()
{
	// --- Create Reel Widgets & Layout ---
	reelLayout = new QHBoxLayout();
	const int numReels = slotMachine ? slotMachine->getReelCount() : 3;
	if (numReels > 3)
	{
		std::cerr << "Warning: currently only supports displaying 3 reels." << std::endl;
	}

	for (int i = 0; i < numReels && i < 3; ++i)
	{
		reelLabels[i] = new QLabel("---");
		reelLabels[i]->setFixedSize(100, 100);		  // Give reels a fixed size
		reelLabels[i]->setAlignment(Qt::AlignCenter); // Center text
		reelLabels[i]->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		reelLabels[i]->setLineWidth(2);
		reelLabels[i]->setStyleSheet("background-color: lightgray;"); //In case images dont load
		reelLayout->addWidget(reelLabels[i]);
	}

	// --- Create Bet Control Widgets & Layout ---
	decreaseBetButton = new QPushButton("-");
	increaseBetButton = new QPushButton("+");
	currentBetLabel = new QLabel("Bet: ???");
	currentBetLabel->setAlignment(Qt::AlignCenter);
	currentBetLabel->setMinimumWidth(50);

	betControlLayout = new QHBoxLayout();
	betControlLayout->addStretch();
	betControlLayout->addWidget(new QLabel("Current Bet:"));
	betControlLayout->addWidget(decreaseBetButton);
	betControlLayout->addWidget(currentBetLabel);
	betControlLayout->addWidget(increaseBetButton);
	betControlLayout->addStretch();

	// --- Create Other Widgets ---
	spinButton = new QPushButton("Spin");
	creditsLabel = new QLabel("Credits: ???");
	// Welcome message
	messageLabel = new QLabel("Welcome! Good Luck! Press Spin to Play.");
	messageLabel->setAlignment(Qt::AlignCenter);

	// --- Create Main Layout ---
	mainLayout = new QVBoxLayout(this);		 // Main layout
	mainLayout->addLayout(reelLayout);		 // Reels
	mainLayout->addLayout(betControlLayout); // Bet controls
	mainLayout->addWidget(spinButton);		 // Spin button
	mainLayout->addWidget(creditsLabel);	 // Credits label
	mainLayout->addWidget(messageLabel);	 // Message label

	// --- Set Window Properties ---
	setWindowTitle("Money-Taker 3000");

	// --- Connect Signals and Slots ---
	connect(spinButton, &QPushButton::clicked, this, &GameWindow::handleSpinButtonClicked);
	// Connect bet buttons
	connect(decreaseBetButton, &QPushButton::clicked, this, &GameWindow::decreaseBet);
	connect(increaseBetButton, &QPushButton::clicked, this, &GameWindow::increaseBet);
}
// *** Load symbol images ***
void GameWindow::loadSymbolImages()
{
	if (!slotMachine || reelLabels[0] == nullptr)
	{
		qWarning() << "Cannot load images: SlotMachine or reelLabels not ready.";
		return;
	}

	// List of symbols to load (ensure IDs match SlotMachine)
	std::vector<Symbol> symbolsToLoad = {
		Symbol("COFFEE", ":/reel-coffee_mug.svg"), Symbol("FLOPPY", ":/reel-floppy.svg"),
		Symbol("FOLDER", ":/reel-file_folder.svg"), Symbol("WRENCH", ":/reel-wrench.svg"),
		Symbol("ERASER", ":/reel-eraser.svg"), Symbol("COMPUTER", ":/reel-computer.svg"),
		Symbol("MOBO", ":/reel-motherboard.svg"), Symbol("ROBOT", ":/reel-robot.svg"),
		Symbol("PACKAGE", ":/reel-package.svg"), Symbol("MAGNIFIER", ":/reel-magnifier.svg"),
		Symbol("FLY_SWAT", ":/reel-fly_swat.svg"), Symbol("TRAP", ":/reel-trap.svg"),
		Symbol("FIRE_EXT", ":/reel-fire_extinguisher.svg"), Symbol("BSOD", ":/reel-blue_screen.svg"),
		Symbol("RICKROLL", ":/reel-rickroll.svg"), Symbol("LUMI", ":/reel-lumi_mascot.svg")};

	QSize labelSize = reelLabels[0]->size();

	qInfo() << "Loading symbol images from resources...";
	symbolPixmaps.clear(); // Clear any previous attempts

	for (const auto &symbol : symbolsToLoad)
	{
		if (symbol.id.empty() || symbol.imagePath.empty())
			continue;

		QPixmap pixmap;
		QString resourcePath = QString::fromStdString(symbol.imagePath);

		// Try loading from Qt Resource System
		if (!pixmap.load(resourcePath))
		{
			qWarning() << "Failed to load image for symbol" << QString::fromStdString(symbol.id) << "from resource path:" << resourcePath;
			continue; // Skip this symbol if failed
		}

		// Scale pixmap
		pixmap = pixmap.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

		// Store the loaded pixmap
		symbolPixmaps[symbol.id] = pixmap;
		qInfo() << "Loaded and cached image for" << QString::fromStdString(symbol.id);
	}
	qInfo() << "Finished loading images. Cached:" << symbolPixmaps.size();
}

// --- Slot Implementations ---

// Slot - core logic for spin button
void GameWindow::handleSpinButtonClicked()
{
	if (!slotMachine)
		return; // Safety check

	std::cout << "Good Luck!" << std::endl;

	// Attempt to spin the reels
	if (slotMachine->spinReels())
	{
		// Spin successful
		bool won = slotMachine->checkWinCondition(); // Check for wins

		// Update the display
		updateDisplay();

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
		updateDisplay(); // Update display even on failure
		// Using user's message
		messageLabel->setText("Get More Money!");
		// Optional future feature: Disable spin button until credits are added like real machines
		// spinButton->setEnabled(false);
	}
}

// Slot to decrease the bet
void GameWindow::decreaseBet()
{
	if (!slotMachine || currentBetIndex < 0)
		return;
	const auto &bets = slotMachine->getAllowedBets();
	if (bets.empty() || currentBetIndex <= 0)
	{
		return;
	}
	currentBetIndex--;
	slotMachine->setSelectedBet(bets[currentBetIndex]);
	updateDisplay();
}

// Slot to increase the bet
void GameWindow::increaseBet()
{
	if (!slotMachine || currentBetIndex < 0)
		return;
	const auto &bets = slotMachine->getAllowedBets();
	if (bets.empty() || currentBetIndex >= static_cast<int>(bets.size()) - 1)
	{
		return;
	}
	currentBetIndex++;
	slotMachine->setSelectedBet(bets[currentBetIndex]);
	updateDisplay();
}

// Helper function to update UI elements
void GameWindow::updateDisplay()
{
	if (!slotMachine)
		return;

	// Update reel labels with images
	int reelCount = slotMachine->getReelCount();
	for (int i = 0; i < reelCount && i < 3; ++i)
	{ // Limit display to array size (3)
		if (reelLabels[i])
		{
			try
			{
				Symbol currentSymbol = slotMachine->getSymbolAt(i);
				// Find the pre-loaded pixmap
				auto it = symbolPixmaps.find(currentSymbol.id);
				if (it != symbolPixmaps.end())
				{
					// Found: Set the pixmap on the label
					reelLabels[i]->setPixmap(it->second); // it->second is the QPixmap
					reelLabels[i]->setText("");			  // Clear any text
				}
				else
				{
					// Not found: Image wasn't loaded or ID mismatch
					qWarning() << "Pixmap not found for symbol ID:" << QString::fromStdString(currentSymbol.id);
					reelLabels[i]->clear();		 // Clear pixmap
					reelLabels[i]->setText("?"); // Show placeholder text
				}
			}
			catch (const std::exception &e)
			{
				// Handle cases where getting symbol fails
				std::cerr << "Error updating reel display " << i << ": " << e.what() << std::endl;
				reelLabels[i]->clear();
				reelLabels[i]->setText("ERR");
			}
		}
	}

	// Update credits label
	creditsLabel->setText("Credits: " + QString::number(slotMachine->getCredits()));

	// Update current bet label
	currentBetLabel->setText(QString::number(slotMachine->getSelectedBet()));

	// Update bet button states
	const auto &bets = slotMachine->getAllowedBets();
	bool canDecrease = !bets.empty() && currentBetIndex > 0;
	bool canIncrease = !bets.empty() && currentBetIndex < static_cast<int>(bets.size()) - 1;
	decreaseBetButton->setEnabled(canDecrease);
	increaseBetButton->setEnabled(canIncrease);
}
