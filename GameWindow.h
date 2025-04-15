#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget> // Base class for widgets

class QPushButton;
class QLabel;
class QHBoxLayout;
class QVBoxLayout;

class SlotMachine;

/**
 * @brief The main window for the Slot Machine application.
 * Displays reels, controls, and messages.
 */
class GameWindow : public QWidget
{
Q_OBJECT // macro for classes with signals and slots

	public :
	// Constructor - takes a pointer to the game logic
	explicit GameWindow(SlotMachine *machineLogic, QWidget *parent = nullptr);
	~GameWindow() override = default;

private slots:
	// handle the spin button being clicked
	void handleSpinButtonClicked();

private:
	// --- UI Elements ---
	QLabel *reelLabels[3];	 // to display symbols (using 3 for MVP)
	QPushButton *spinButton; // to trigger a spin
	QLabel *creditsLabel;	 // to display current credits
	QLabel *messageLabel;	 // for win/loss messages

	// --- Layouts ---
	QVBoxLayout *mainLayout; // vertical layout
	QHBoxLayout *reelLayout; // horizontal layout for reels

	// --- Game Logic ---
	SlotMachine *slotMachine; // Pointer to slotmachine cause it's easily confused

	// --- Helper Methods ---
	void setupUI();		  // Helper to create widgets
	void updateDisplay(); // Helper to update labels based on what the machine says
	void connectReelSignals(); // Connect reel signals to update display
};

#endif
