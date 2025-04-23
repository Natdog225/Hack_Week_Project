#include "GameWindow.h"
#include "SlotMachine.h"
#include "Symbol.h"
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>
#include <QString>
#include <QPixmap>
#include <QDir>
#include <QDebug>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <map>
#include <string>
#include <random>
#include <numeric>

// Constructor
GameWindow::GameWindow(SlotMachine *machineLogic, QWidget *parent)
	: QWidget(parent), slotMachine(machineLogic), currentBetIndex(0), animationTimer(nullptr),
	  isSpinning(false), reelStoppedVisual(NUM_REELS_DISPLAYED, false), spinSoundEffect(nullptr),
	  winSoundEffect(nullptr), betSoundEffect(nullptr), flashTimer(nullptr), flashCount(0), flashOn(false)
{
	if (!slotMachine)
	{
		throw std::runtime_error("Requires Valid SlotMachine Instance");
	}
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
	// Initialize label pointers to null
	for (int r = 0; r < NUM_REELS_DISPLAYED; ++r)
	{
		for (int s = 0; s < SYMBOLS_PER_REEL_DISPLAYED; ++s)
		{
			reelDisplayLabels[r][s] = nullptr;
		}
		reelColumnLayouts[r] = nullptr;
	}
	// Setup Timer
	animationTimer = new QTimer(this);
	connect(animationTimer, &QTimer::timeout, this, &GameWindow::animateReelStep);
	flashTimer = new QTimer(this);
	connect(flashTimer, &QTimer::timeout, this, &GameWindow::flashStep);

	setupUI();
	setupSounds(); // Initialize sound effects
	loadSymbolImages();
	applyStyles();							   // Apply styles after UI is setup
	originalWindowStyle = this->styleSheet();  // Store original style BEFORE applying flash styles
	qDebug() << "Stored original stylesheet."; // DEBUG
	updateDisplay();
	qDebug() << "GameWindow Constructor: Initialization Complete."; // DEBUG
}
// Destructor needed to clean up timer
GameWindow::~GameWindow()
{
	qDebug() << "GameWindow Destructor called."; // Debug
}

// Helper function to create and arrange UI elements
void GameWindow::setupUI()
{
	// --- Create Reel Widgets & Layout (3x3 Grid) ---
	reelsAreaLayout = new QHBoxLayout(); // Horizontal layout for the columns
	reelsAreaLayout->setSpacing(10);	 // Spacing between reel columns

	const int numReels = slotMachine ? slotMachine->getReelCount() : 3;
	if (numReels != NUM_REELS_DISPLAYED)
	{
		std::cerr << "Warning: GameWindow configured for " << NUM_REELS_DISPLAYED
				  << " reels, but SlotMachine has " << numReels << "." << std::endl;
		// Handle mismatch if necessary
	}

	for (int r = 0; r < NUM_REELS_DISPLAYED; ++r)
	{											  // Iterate through reel columns (0, 1, 2)
		reelColumnLayouts[r] = new QVBoxLayout(); // Vertical layout for this column
		reelColumnLayouts[r]->setSpacing(5);	  // Spacing between symbols in a column

		for (int s = 0; s < SYMBOLS_PER_REEL_DISPLAYED; ++s)
		{													 // Iterate through symbols in column (0=top, 1=middle, 2=bottom)
			reelDisplayLabels[r][s] = new QLabel();			 // Create the label
			reelDisplayLabels[r][s]->setFixedSize(100, 100); // Symbol size
			reelDisplayLabels[r][s]->setAlignment(Qt::AlignCenter);
			reelDisplayLabels[r][s]->setFrameStyle(QFrame::Panel | QFrame::Raised);
			reelDisplayLabels[r][s]->setLineWidth(2);
			// Use object names for styling if needed
			reelDisplayLabels[r][s]->setObjectName(QString("reelSymbol_%1_%2").arg(r).arg(s));
			reelColumnLayouts[r]->addWidget(reelDisplayLabels[r][s]); // Add label to column layout
		}
		reelsAreaLayout->addLayout(reelColumnLayouts[r]); // Add the column layout to the main reels area
	}

	// --- Displays (Credits, Bet, Message) ---
	creditsLabel = new QLabel("Credits: ???");
	creditsLabel->setObjectName("displayLabel"); // Object name for styling
	creditsLabel->setAlignment(Qt::AlignCenter);

	currentBetLabel = new QLabel("Bet: ???");
	currentBetLabel->setObjectName("displayLabel"); // Object name for styling
	currentBetLabel->setAlignment(Qt::AlignCenter);

	messageLabel = new QLabel("Welcome! Good Luck! Press Spin to Play.");
	messageLabel->setObjectName("messageLabel"); // Object name for styling
	messageLabel->setAlignment(Qt::AlignCenter);
	messageLabel->setMinimumHeight(30); // Give message label some height

	// Layout for displays
	QHBoxLayout *displayLayout = new QHBoxLayout();
	displayLayout->addWidget(creditsLabel);
	displayLayout->addStretch(); // Add space between credits and bet
	displayLayout->addWidget(currentBetLabel);

	// --- Create Bet Control Widgets ---
	decreaseBetButton = new QPushButton("-");
	decreaseBetButton->setObjectName("betButton");
	decreaseBetButton->setFixedWidth(40);
	increaseBetButton = new QPushButton("+");
	increaseBetButton->setObjectName("betButton");
	increaseBetButton->setFixedWidth(40);
	// Label that shows "Bet: X"
	currentBetLabel = new QLabel("Bet: ???");
	currentBetLabel->setObjectName("displayLabel");
	currentBetLabel->setAlignment(Qt::AlignCenter);
	currentBetLabel->setMinimumWidth(100); // Give it some width

	// --- Create Spin Button ---
	spinButton = new QPushButton("SPIN");
	spinButton->setObjectName("spinButton");
	spinButton->setMinimumHeight(50);

	// --- Layout for Bet Controls --- (Grouped: [Stretch] [-] [Bet: X] [+] [Stretch])
	betControlLayout = new QHBoxLayout();
	betControlLayout->addStretch();
	betControlLayout->addWidget(decreaseBetButton);
	betControlLayout->addWidget(currentBetLabel); // The label showing "Bet: X"
	betControlLayout->addWidget(increaseBetButton);
	betControlLayout->addStretch();

	// --- Main Layout ---
	mainLayout = new QVBoxLayout(this);
	mainLayout->addLayout(reelsAreaLayout); // Reels at top
	mainLayout->addSpacing(10);
	mainLayout->addWidget(creditsLabel); // Credits label directly added
	mainLayout->addWidget(messageLabel); // Message label directly added
	mainLayout->addSpacing(10);
	mainLayout->addLayout(betControlLayout); // Bet controls layout added
	mainLayout->addSpacing(10);
	mainLayout->addWidget(spinButton); // Spin button directly added at bottom

	// --- Set Window Properties ---
	setWindowTitle("Money-Taker 3000");
	setMinimumSize(550, 500); // Adjust size as needed

	// --- Connect Signals and Slots ---
	connect(spinButton, &QPushButton::clicked, this, &GameWindow::handleSpinButtonClicked);
	connect(decreaseBetButton, &QPushButton::clicked, this, &GameWindow::decreaseBet);
	connect(increaseBetButton, &QPushButton::clicked, this, &GameWindow::increaseBet);
}

// *** Setup Sound Effects ***
void GameWindow::setupSounds()
{
	spinSoundEffect = new QSoundEffect(this);
	spinSoundEffect->setSource(QUrl("qrc:/spin_sound.wav"));
	spinSoundEffect->setVolume(0.75); // Adjust volume 0.0 to 1.0

	winSoundEffect = new QSoundEffect(this);
	winSoundEffect->setSource(QUrl("qrc:/win_sound.wav"));
	winSoundEffect->setVolume(0.9);

	betSoundEffect = new QSoundEffect(this);
	betSoundEffect->setSource(QUrl("qrc:/Bet.wav"));
	betSoundEffect->setVolume(0.6); // Adjust volume as needed

	// Check if sounds loaded (optional but good)
	if (!spinSoundEffect->isLoaded())
	{ // Check isLoaded status
		qWarning() << "Spin sound FAILED TO LOAD. Status:" << spinSoundEffect->status() << "Source:" << spinSoundEffect->source().toString();
	}
	else
	{
		qDebug() << "Spin sound loaded successfully.";
	}
	if (!winSoundEffect->isLoaded())
	{
		qWarning() << "Win sound FAILED TO LOAD. Status:" << winSoundEffect->status() << "Source:" << winSoundEffect->source().toString();
	}
	else
	{
		qDebug() << "Win sound loaded successfully.";
		if (!betSoundEffect->isLoaded())
		{
			qWarning() << "Bet sound FAILED TO LOAD. Status:" << betSoundEffect->status() << "Source:" << betSoundEffect->source().toString();
		}
		else
		{
			qDebug() << "Bet sound loaded successfully.";
		}
	}
}

// Apply Qt Style Sheets (QSS) ***
void GameWindow::applyStyles()
{
	QString styleSheet = R"(
        /* Style for the main window (GameWindow) */
        QWidget#GameWindow { /* Use object name if set, otherwise applies to base QWidget */
            background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                              stop: 0 #333344, stop: 1 #555566);
        }

        /* Style for the Reel Labels */
        QLabel[objectName^="reelSymbol"] { /* Select labels whose object name starts with reelSymbol */
            background-color: #111122; /* Dark background for reels */
            border: 2px solid #888899;
            border-radius: 5px;
        }

        /* Style for the Display Labels (Credits, Bet) */
        QLabel#displayLabel { /* Select labels with objectName "displayLabel" */
            background-color: #000000; /* Black background */
            color: #00FF00; /* Bright green text */
            font-size: 16pt;
            font-family: "Courier New", Courier, monospace; /* Monospaced font */
            font-weight: bold;
            border: 1px solid #00FF00;
            border-radius: 4px;
            padding: 5px;
            min-height: 25px;
        }
         QLabel#messageLabel { /* Style for the message label */
            color: #FFFF00; /* Yellow text */
            font-size: 14pt;
            font-weight: bold;
         }
         /* Static label before bet buttons */
         QLabel { /* General labels - careful not to override too much */
            color: #FFFFFF; /* White text for other labels */
            font-size: 12pt;
         }


        /* Style for the Spin Button */
        QPushButton#spinButton {
            background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                              stop: 0 #00AA00, stop: 1 #006600); /* Green gradient */
            color: white;
            font-size: 18pt;
            font-weight: bold;
            border: 2px solid #00FF00;
            border-radius: 10px; /* More rounded */
            padding: 10px;
            min-width: 100px;
        }
        QPushButton#spinButton:hover {
            background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                              stop: 0 #00CC00, stop: 1 #008800); /* Lighter green on hover */
        }
        QPushButton#spinButton:pressed {
            background-color: #006600; /* Darker green when pressed */
        }

        /* Style for the Bet Buttons */
        QPushButton#betButton {
            background-color: #444488; /* Blueish */
            color: white;
            font-size: 14pt;
            font-weight: bold;
            border: 1px solid #AAAAFF;
            border-radius: 5px;
            padding: 5px;
        }
         QPushButton#betButton:hover {
            background-color: #6666AA;
         }
         QPushButton#betButton:pressed {
            background-color: #333366;
         }
         QPushButton#betButton:disabled { /* Style when disabled */
            background-color: #555555;
            color: #999999;
         }

    )";

	this->setStyleSheet(styleSheet);
}

// Load symbol images
void GameWindow::loadSymbolImages()
{
	if (!slotMachine || reelDisplayLabels[0][0] == nullptr)
	{
		qWarning() << "Cannot load images: SlotMachine or reelLabels not ready.";
		return;
	}
	std::vector<Symbol> symbolsToLoad = {Symbol("COFFEE", ":/reel-coffee_mug.svg"), Symbol("FLOPPY", ":/reel-floppy.svg"), Symbol("FOLDER", ":/reel-file_folder.svg"), Symbol("WRENCH", ":/reel-wrench.svg"), Symbol("ERASER", ":/reel-eraser.svg"), Symbol("COMPUTER", ":/reel-computer.svg"), Symbol("MOBO", ":/reel-motherboard.svg"), Symbol("ROBOT", ":/reel-robot.svg"), Symbol("PACKAGE", ":/reel-package.svg"), Symbol("MAGNIFIER", ":/reel-magnifier.svg"), Symbol("FLY_SWAT", ":/reel-fly_swat.svg"), Symbol("TRAP", ":/reel-trap.svg"), Symbol("FIRE_EXT", ":/reel-fire_extinguisher.svg"), Symbol("BSOD", ":/reel-blue_screen.svg"), Symbol("RICKROLL", ":/reel-rickroll.svg"), Symbol("LUMI", ":/reel-lumi_mascot.svg")};
	QSize labelSize = reelDisplayLabels[0][0]->size();
	qInfo() << "Loading symbol images from resources...";
	symbolPixmaps.clear();
	allSymbolIds.clear();
	for (const auto &symbol : symbolsToLoad)
	{
		if (symbol.id.empty() || symbol.imagePath.empty())
			continue;
		allSymbolIds.push_back(symbol.id);
		QPixmap pixmap;
		QString resourcePath = QString::fromStdString(symbol.imagePath);
		if (!pixmap.load(resourcePath))
		{
			qWarning() << "Failed to load image for symbol" << QString::fromStdString(symbol.id) << "from resource path:" << resourcePath;
			continue;
		}
		pixmap = pixmap.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		symbolPixmaps[symbol.id] = pixmap;
		qInfo() << "Loaded and cached image for" << QString::fromStdString(symbol.id);
	}
	qInfo() << "Finished loading images. Cached:" << symbolPixmaps.size();
	qDebug() << "Populated allSymbolIds. Size:" << allSymbolIds.size(); // DEBUG
}

// --- Slot Implementations ---

// Slot to handle main spin button clicks
void GameWindow::handleSpinButtonClicked()
{
	qDebug() << "handleSpinButtonClicked called. Current state: isSpinning=" << isSpinning;
	if (!slotMachine || isSpinning || allSymbolIds.empty())
	{
		qWarning() << "Spin request ignored.";
		return;
	}
	if (slotMachine->getCredits() < slotMachine->getSelectedBet())
	{
		qDebug() << "Spin request ignored: Not enough credits.";
		messageLabel->setText("Not enough credits!");
		return;
	}

	isSpinning = true;
	qDebug() << "Spin allowed. Setting isSpinning=true";
	std::fill(reelStoppedVisual.begin(), reelStoppedVisual.end(), false); // Reset visual stop flags

	spinButton->setEnabled(false);
	increaseBetButton->setEnabled(false);
	decreaseBetButton->setEnabled(false);
	messageLabel->setText("Spinning...");
	qDebug() << "Buttons disabled, message set to Spinning...";

	// *** Play Spin Sound ***
	if (spinSoundEffect && spinSoundEffect->isLoaded())
	{
		spinSoundEffect->play();
	}
	else
	{
		qWarning() << "Spin sound not ready or not loaded.";
	}

	qDebug() << "Calling slotMachine->spinReels()...";
	slotMachine->spinReels(); // Determine final outcome now
	qDebug() << "slotMachine->spinReels() returned.";

	// Start the main animation timer (makes all reels flicker)
	const int animationIntervalMs = 75;
	animationTimer->start(animationIntervalMs);
	qDebug() << "Main animation timer started.";

	// Schedule staggered stops for each reel
	const int stopDelayIncrementMs = 500; // Time between reel stops
	for (int r = 0; r < NUM_REELS_DISPLAYED; ++r)
	{
		int stopDelay = 1000 + (r * stopDelayIncrementMs); // Reel 0 stops at 1000ms, Reel 1 at 1500ms, etc.
		qDebug() << "Scheduling stop for reel" << r << "in" << stopDelay << "ms";
		// Use lambda to capture reel index for the slot call
		QTimer::singleShot(stopDelay, this, [this, r]()
						   { stopReelAnimation(r); });
	}
	qDebug() << "handleSpinButtonClicked finished setup.";
}

// NEW SLOT: Called by animation timer
void GameWindow::animateReelStep()
{
	if (allSymbolIds.empty())
		return;

	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib(0, allSymbolIds.size() - 1);

	// Update only the reels that haven't visually stopped yet
	for (int r = 0; r < NUM_REELS_DISPLAYED; ++r)
	{
		if (!reelStoppedVisual[r])
		{ // Only animate if not stopped
			for (int s = 0; s < SYMBOLS_PER_REEL_DISPLAYED; ++s)
			{
				if (reelDisplayLabels[r][s])
				{
					int randomIndex = distrib(gen);
					const std::string &randomId = allSymbolIds[randomIndex];
					auto it = symbolPixmaps.find(randomId);
					if (it != symbolPixmaps.end())
					{
						reelDisplayLabels[r][s]->setPixmap(it->second);
					}
					else
					{
						reelDisplayLabels[r][s]->setText("?");
					}
				}
			}
		}
	}
}

// Stops animation and shows final result for ONE reel
void GameWindow::stopReelAnimation(int reelIndex)
{
	if (reelIndex < 0 || reelIndex >= NUM_REELS_DISPLAYED)
		return;

	qDebug() << "stopReelAnimation called for reel" << reelIndex;
	reelStoppedVisual[reelIndex] = true; // Mark this reel as visually stopped

	// Update this specific reel column to show its final symbols
	updateReelDisplay(reelIndex);

	// Check if *all* reels have now stopped visually
	bool allStopped = std::all_of(reelStoppedVisual.begin(), reelStoppedVisual.end(), [](bool stopped)
								  { return stopped; });

	if (allStopped)
	{
		qDebug() << "All reels have stopped visually.";
		animationTimer->stop(); // Stop the main animation flicker
		qDebug() << "Animation timer stopped.";

		qDebug() << "Calling slotMachine->checkWinCondition()...";
		int totalPayout = slotMachine->checkWinCondition(); // Check result now
		qDebug() << "slotMachine->checkWinCondition() returned:" << totalPayout;

		// Update credits display
		creditsLabel->setText("Credits: " + QString::number(slotMachine->getCredits()));

		if (totalPayout > 0)
		{
			// *** Trigger win sequence (sound + flash) ***
			startWinSequence();
			messageLabel->setText("WINNER! Payout: " + QString::number(totalPayout));
		}
		else
		{
			messageLabel->setText("Try Again Loser");
			// Re-enable buttons immediately if no win sequence
			isSpinning = false;
			spinButton->setEnabled(true);
			const auto &bets = slotMachine->getAllowedBets();
			bool canDecrease = !isSpinning && !bets.empty() && currentBetIndex > 0;
			bool canIncrease = !isSpinning && !bets.empty() && currentBetIndex < static_cast<int>(bets.size()) - 1;
			decreaseBetButton->setEnabled(canDecrease);
			increaseBetButton->setEnabled(canIncrease);
			qDebug() << "No win. Re-enabling buttons. isSpinning=false.";
		}
		qDebug() << "Final message set:" << messageLabel->text();
	}
}

// *** Triggers win effects ***
void GameWindow::startWinSequence()
{
	qDebug() << "Starting win sequence...";
	// Play Win Sound
	if (winSoundEffect && winSoundEffect->isLoaded())
	{
		winSoundEffect->play();
		qDebug() << "Played win sound."; // DEBUG
	}
	else
	{
		qWarning() << "Win sound not ready or not loaded. Status:" << (winSoundEffect ? winSoundEffect->status() : QSoundEffect::Error);
	}

	// Start Flashing
	flashCount = 6;																					// Number of ON/OFF cycles (e.g., 6 flashes = 3 on, 3 off)
	flashOn = true;																					// Start with flash ON
	int flashIntervalMs = 150;																		// Duration of each flash state (ON or OFF)
	qDebug() << "Starting flash timer. Interval:" << flashIntervalMs << "ms, Count:" << flashCount; // DEBUG
	flashTimer->start(flashIntervalMs);
	flashStep(); // Call immediately to set initial flash state

	// Schedule buttons to re-enable *after* flashing finishes
	int flashDurationMs = flashCount * flashIntervalMs;
	QTimer::singleShot(flashDurationMs + 50, this, [this]() { // Small delay after last flash
		stopFlashing();										  // Ensure flashing stops cleanly
		isSpinning = false;									  // Clear flag only after win sequence
		spinButton->setEnabled(true);
		const auto &bets = slotMachine->getAllowedBets();
		bool canDecrease = !isSpinning && !bets.empty() && currentBetIndex > 0;
		bool canIncrease = !isSpinning && !bets.empty() && currentBetIndex < static_cast<int>(bets.size()) - 1;
		decreaseBetButton->setEnabled(canDecrease);
		increaseBetButton->setEnabled(canIncrease);
		qDebug() << "Win sequence finished. Re-enabling buttons. isSpinning=false.";
	});
}

// *** NEW SLOT: Called by flash timer ***
void GameWindow::flashStep()
{
	qDebug() << "flashStep called. FlashCount:" << flashCount << "FlashOn:" << flashOn; // DEBUG
	if (flashCount <= 0)
	{
		// Timer might still be running, ensure it stops and style is restored
		stopFlashing();
		return;
	}

	if (flashOn)
	{
		// Apply flashing style (e.g., bright background)
		// Combine with original style to avoid losing everything else
		this->setStyleSheet("QWidget#GameWindow { background-color: yellow; }");
		qDebug() << "Flash ON";
	}
	else
	{
		// Restore original style
		this->setStyleSheet(originalWindowStyle);
		qDebug() << "Flash OFF";
	}

	flashOn = !flashOn; // Toggle state
	flashCount--;
}

// *** NEW SLOT: Stops flashing explicitly ***
void GameWindow::stopFlashing()
{
	if (flashTimer->isActive())
	{
		qDebug() << "Stopping flashing timer.";
		flashTimer->stop();
	}
	qDebug() << "Restoring original stylesheet.";
	// Ensure original style is restored, even if timer was stopped early
	this->setStyleSheet(originalWindowStyle);
	flashOn = false;
	flashCount = 0;
}

// Slot to decrease the bet
void GameWindow::decreaseBet()
{
	qDebug() << "decreaseBet called. isSpinning=" << isSpinning;
	if (!slotMachine || isSpinning || currentBetIndex < 0)
		return;
	const auto &bets = slotMachine->getAllowedBets();
	if (bets.empty() || currentBetIndex <= 0)
	{
		return;
	}
	currentBetIndex--;
	slotMachine->setSelectedBet(bets[currentBetIndex]);
	updateDisplay(); // Update bet label
					 // *** Play Bet Sound ***
	if (betSoundEffect && betSoundEffect->isLoaded())
	{
		betSoundEffect->play();
		qDebug() << "Played bet sound.";
	}
	else
	{
		qWarning() << "Bet sound not ready or not loaded.";
	}
}

// Slot to increase the bet
void GameWindow::increaseBet()
{
	qDebug() << "increaseBet called. isSpinning=" << isSpinning;
	if (!slotMachine || isSpinning || currentBetIndex < 0)
		return;
	const auto &bets = slotMachine->getAllowedBets();
	if (bets.empty() || currentBetIndex >= static_cast<int>(bets.size()) - 1)
	{
		return;
	}
	currentBetIndex++;
	slotMachine->setSelectedBet(bets[currentBetIndex]);
	updateDisplay(); // Update bet label
	if (betSoundEffect && betSoundEffect->isLoaded())
	{
		betSoundEffect->play();
	}
	else
	{
		qWarning() << "Bet sound not ready or not loaded.";
	}
}

// updateDisplay
void GameWindow::updateDisplay()
{
	if (!slotMachine)
		return;
	// Note: Reel symbols are updated individually by stopReelAnimation via updateReelDisplay helper.

	// Update credits label
	creditsLabel->setText("Credits: " + QString::number(slotMachine->getCredits()));
	// Update current bet label
	currentBetLabel->setText("Bet: " + QString::number(slotMachine->getSelectedBet()));

	// Update bet button states - disable if spinning
	const auto &bets = slotMachine->getAllowedBets();
	bool canDecrease = !isSpinning && !bets.empty() && currentBetIndex > 0;
	bool canIncrease = !isSpinning && !bets.empty() && currentBetIndex < static_cast<int>(bets.size()) - 1;
	decreaseBetButton->setEnabled(canDecrease);
	increaseBetButton->setEnabled(canIncrease);
}

// Updates a single reel column to its final state
void GameWindow::updateReelDisplay(int reelIndex)
{
	if (!slotMachine || reelIndex < 0 || reelIndex >= NUM_REELS_DISPLAYED)
		return;

	qDebug() << "Updating final display for reel" << reelIndex;
	try
	{
		std::vector<Symbol> visibleSymbols = slotMachine->getVisibleSymbols(reelIndex);
		if (visibleSymbols.size() == SYMBOLS_PER_REEL_DISPLAYED)
		{
			for (int s = 0; s < SYMBOLS_PER_REEL_DISPLAYED; ++s)
			{
				// Use reelIndex (passed to function) instead of 'r'
				if (reelDisplayLabels[reelIndex][s])
				{
					const std::string &symbolId = visibleSymbols[s].id;
					auto it = symbolPixmaps.find(symbolId);
					if (it != symbolPixmaps.end())
					{
						reelDisplayLabels[reelIndex][s]->setPixmap(it->second);
						reelDisplayLabels[reelIndex][s]->setText("");
					}
					else
					{
						qWarning() << "Pixmap not found for symbol ID:" << QString::fromStdString(symbolId);
						reelDisplayLabels[reelIndex][s]->clear();
						reelDisplayLabels[reelIndex][s]->setText("?");
					}
				}
			}
		}
		else
		{
			std::cerr << "Error: getVisibleSymbols for reel " << reelIndex << " returned " << visibleSymbols.size() << " symbols." << std::endl;
			for (int s = 0; s < SYMBOLS_PER_REEL_DISPLAYED; ++s)
			{
				if (reelDisplayLabels[reelIndex][s])
					reelDisplayLabels[reelIndex][s]->setText("ERR");
			}
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error updating reel display column " << reelIndex << ": " << e.what() << std::endl;
		for (int s = 0; s < SYMBOLS_PER_REEL_DISPLAYED; ++s)
		{
			if (reelDisplayLabels[reelIndex][s])
				reelDisplayLabels[reelIndex][s]->setText("ERR");
		}
	}
}
