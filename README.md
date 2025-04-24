# Slot Machine in C++

## Authors
Nathan W. (Nathaniel.wilson@atlasschool.com) Github: Natdog225
Deshon Stout (deshonjewell@gmail.com) Github: DeshonS
José N. Olmos () Github: chepeniv

## Purpose
The purpose of this project is to create a slot machine using the C++ programming language. (Will update as we progress)

## Function
Project Description: "Money-Taker 3000" Slot Machine
This project is a C++ application simulating a classic slot machine game, built using the Qt framework for its graphical user interface (GUI) and core functionalities. The goal was to create an interactive and visually engaging slot machine experience, incorporating standard features like spinning reels, variable betting, multiple paylines, and special symbols.

Theme: While the initial project constraint mentioned a "Waves" theme, this project pivoted to a more traditional slot machine concept, focusing on implementing game mechanics and GUI interaction within the C++/Qt environment. Isn't luck a wave in itself?

Technologies Used:

C++: The core programming language used for logic implementation, class design, and overall structure.

Qt Framework: Utilized for:

GUI: Creating the visual window, buttons, labels, and layouts (using Qt Widgets).

Signals & Slots: Managing interactions between UI elements and game logic.

Graphics: Displaying symbol images (QPixmap).

Timers: Implementing the spinning animation and win sequence delays (QTimer).

Multimedia: Playing sound effects (QSoundEffect).

Resource System: Embedding assets (images, sounds) into the application (.qrc).

CMake: Used as the build system to manage compilation and linking of the C++ code and Qt libraries.

Git: Used for version control and collaboration.

How It Works: Simple Explanation
The program combines a visual front-end (the window you see) with a logical back-end (the game rules).

Initialization:

When the application starts (main.cpp), it creates the core game logic object (SlotMachine) and the main window object (GameWindow).

SlotMachine initializes its internal state: sets up the reels, defines the list of possible symbols (including IDs and image paths), defines the payout rules (paytable), sets the starting credits, and defines allowed bet amounts.

GameWindow sets up the UI layout (3x3 grid for symbols, buttons, display labels), loads symbol images and sounds (using the Qt Resource system), applies styling (using QSS), connects button clicks to functions (slots), and displays the initial state (credits, default bet).

User Interaction (Betting):

The user clicks the "+" or "-" buttons next to the "Bet" display.

GameWindow detects the click and calls a function (increaseBet or decreaseBet).

These functions update an internal index tracking the current bet level.

They call SlotMachine::setSelectedBet to update the bet amount stored in the game logic.

They play a "bet change" sound effect.

They call GameWindow::updateDisplay to show the new bet amount on the screen and enable/disable the +/- buttons if the min/max bet is reached.

User Interaction (Spinning):

The user clicks the "SPIN" button.

GameWindow::handleSpinButtonClicked is called.

It checks if an animation is already running (isSpinning) or if the player has enough credits (by checking SlotMachine::getCredits vs SlotMachine::getSelectedBet).

If okay to spin:

It sets isSpinning to true and disables the Spin and Bet buttons.

It plays the "spin" sound effect.

It tells the SlotMachine to perform the logical spin (SlotMachine::spinReels). This deducts the bet from credits and randomly determines the final stopping position for each reel internally.

It starts a QTimer (animationTimer) to rapidly call GameWindow::animateReelStep.

It schedules staggered QTimer::singleShot calls for each reel to trigger GameWindow::stopReelAnimation after different delays (e.g., 1s, 1.5s, 2s).

Animation & Stopping:

While animationTimer is running, GameWindow::animateReelStep is called repeatedly. It quickly updates the 9 symbol labels with random images from the cache, creating a flickering/spinning visual effect (only on reels that haven't visually stopped).

When a reel's scheduled singleShot timer fires, GameWindow::stopReelAnimation(reelIndex) is called for that specific reel.

stopReelAnimation marks that reel as visually stopped (reelStoppedVisual[reelIndex] = true).

It calls GameWindow::updateReelDisplay(reelIndex) which gets the actual final symbols (top, middle, bottom) for that specific reel from the SlotMachine (using SlotMachine::getVisibleSymbols) and displays them in the correct labels.

It checks if all reels are now visually stopped.

Win Check & Sequence:

Once the last reel stops visually, stopReelAnimation proceeds:

It stops the main animationTimer.

It calls SlotMachine::checkWinCondition(). This function checks all 5 paylines (3 horizontal, 2 diagonal) against the payoutTable, considering the wild symbol ("RICKROLL"). It returns the total payout amount (0 if no win).

The GameWindow updates the credits display.

If Win (payout > 0):

GameWindow::startWinSequence is called.

The "win" sound plays.

The "WINNER! Payout: X" message is shown.

A flashTimer starts, calling GameWindow::flashStep repeatedly to toggle the window background style (creating a flash).

Another QTimer::singleShot is scheduled to run after the flashing finishes. This timer's slot calls stopFlashing, sets isSpinning back to false, and re-enables the Spin/Bet buttons.

If No Win (payout == 0):

The "Try Again Loser" message is shown.

isSpinning is set back to false.

The Spin/Bet buttons are re-enabled immediately.

Return to Idle: The application is now back in the idle state, waiting for the user to change the bet or spin again.

## Commands
Past compiling it just hit spin. 
