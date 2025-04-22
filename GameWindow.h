#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <vector>
#include <map>
#include <string>
#include <QPixmap>
#include <QTimer>

class QPushButton;
class QLabel;
class QHBoxLayout;
class QVBoxLayout;
class SlotMachine;
struct Symbol;

class GameWindow : public QWidget
{
Q_OBJECT // Required macro for classes with signals and slots

	public :
	// Constructor
	explicit GameWindow(SlotMachine *machineLogic, QWidget *parent = nullptr);
	~GameWindow() override;

private slots:
	// Slot for main spin button
	void handleSpinButtonClicked();
	// Slots for bet adjustment buttons
	void increaseBet();
	void decreaseBet();
	void animateReelStep();
	void stopReelAnimation(int reelIndex);

private:
	// --- UI Elements ---
	static const int NUM_REELS_DISPLAYED = 3;
	static const int SYMBOLS_PER_REEL_DISPLAYED = 3;
	QLabel *reelDisplayLabels[NUM_REELS_DISPLAYED][SYMBOLS_PER_REEL_DISPLAYED]; // 3x3 grid
	QPushButton *spinButton;													// Button to trigger a spin
	QLabel *creditsLabel;														// Label to display current credits
	QLabel *messageLabel;														// Label for win/loss messages
	QPushButton *increaseBetButton;
	QPushButton *decreaseBetButton;
	QLabel *currentBetLabel;

	// --- Layouts ---
	QVBoxLayout *mainLayout;							 // Overall vertical layout
	QHBoxLayout *reelsAreaLayout;						 // Horizontal layout to hold the reel columns
	QVBoxLayout *reelColumnLayouts[NUM_REELS_DISPLAYED]; // Vertical layouts for each reel column
	QHBoxLayout *betControlLayout;						 // New layout for bet controls

	// --- Game Logic ---
	SlotMachine *slotMachine; // Pointer to the SlotMachine instance
	int currentBetIndex;	  // Index within the slotMachine->getAllowedBets() vector

	// -- Image Cache & Animation ---
	std::map<std::string, QPixmap> symbolPixmaps; // <-- image cache
	std::vector<std::string> allSymbolIds;		  // Store all possible symbol IDs for animation
	QTimer *animationTimer;						  // Timer for animation steps
	bool isSpinning;							  // Flag to prevent re-triggering spin
	std::vector<bool> reelStoppedVisual;

	// --- Helper Methods ---
	void setupUI();			 // Helper to create and arrange widgets
	void updateDisplay();	 // Helper to update labels based on SlotMachine state
	void updateReelDisplay(int reelIndex);
	void loadSymbolImages(); // Function to load images
	void applyStyles();
};

#endif // GAMEWINDOW_H
