#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <vector>
#include <map>	
#include <string>

#include <QPixmap>

// Forward declarations
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
	~GameWindow() override = default;

private slots:
	// Slot for main spin button
	void handleSpinButtonClicked();
	// Slots for bet adjustment buttons
	void increaseBet();
	void decreaseBet();

private:
	// --- UI Elements ---
	QLabel *reelLabels[3];	 // Labels to display symbols (using 3 for MVP)
	QPushButton *spinButton; // Button to trigger a spin
	QLabel *creditsLabel;	 // Label to display current credits
	QLabel *messageLabel;	 // Label for win/loss messages
	QPushButton *increaseBetButton;
	QPushButton *decreaseBetButton;
	QLabel *currentBetLabel;

	// --- Layouts ---
	QVBoxLayout *mainLayout;	   // Overall vertical layout
	QHBoxLayout *reelLayout;	   // Horizontal layout for the reels
	QHBoxLayout *betControlLayout; // New layout for bet controls

	// --- Game Logic ---
	SlotMachine *slotMachine; // Pointer to the SlotMachine instance
	int currentBetIndex;	  // Index within the slotMachine->getAllowedBets() vector

	// --- Image Cache ---
	// Map to store loaded and scaled pixmaps
	std::map<std::string, QPixmap> symbolPixmaps; // <-- Added image cache

	// --- Helper Methods ---
	void setupUI();			 // Helper to create and arrange widgets
	void updateDisplay();	 // Helper to update labels based on SlotMachine state
	void loadSymbolImages(); // Function to load images
};

#endif // GAMEWINDOW_H
