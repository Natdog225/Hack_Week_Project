#include "GameWindow.h"
#include "SlotMachine.h"
#include <QPushButton>
#include <QLabel>
#include <iostream>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString> // For converting C++ strings/ints to Qt strings

// Constructor
GameWindow::GameWindow(SlotMachine* machineLogic, QWidget *parent)
	: QWidget(parent), slotMachine(machineLogic)
{
	if (!slotMachine) {
		// For now, maybe just print an error or throw
		throw std::runtime_error("GameWindow requires a valid SlotMachine instance.");
	}
	setupUI();
	updateDisplay(); // Set initial display state
}

// Helper function to create and arrange UI elements
void GameWindow::setupUI() {
	// --- Create Widgets ---
	reelLayout = new QHBoxLayout(); // Layout for reels
	for (int i = 0; i < 3; ++i) { // Assuming 3 reels for MVP
		reelLabels[i] = new QLabel("---"); // Placeholder text
		reelLabels[i]->setFixedSize(100, 100); // Give reels a fixed size
		reelLabels[i]->setAlignment(Qt::AlignCenter); // Center text
		// basic styling
		reelLabels[i]->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		reelLabels[i]->setLineWidth(2);
		reelLayout->addWidget(reelLabels[i]);
	}

	spinButton = new QPushButton("Spin");
	creditsLabel = new QLabel("Credits: ???");
	messageLabel = new QLabel("Welcome! We hope to take all your money");
	messageLabel->setAlignment(Qt::AlignCenter);

	// --- Create Main Layout ---
	mainLayout = new QVBoxLayout(this); // Main layout
	mainLayout->addLayout(reelLayout);  // Add the reel layout
	mainLayout->addWidget(spinButton);  // Add the spin button
	mainLayout->addWidget(creditsLabel);// Add the credits label
	mainLayout->addWidget(messageLabel);// Add the message label

	// --- Set Window Properties ---
	setWindowTitle("Slot Machine MVP");
	// setMinimumSize(400, 250); // Suggest a minimum size

	// --- Connect Signals and Slots ---
	connect(spinButton, &QPushButton::clicked, this, &GameWindow::handleSpinButtonClicked);
}

// to handle spin button clicks
void GameWindow::handleSpinButtonClicked() {
	std::cout << "We hope you have terrible luck!" << std::endl; // Debug output for now

	// --- Logic ideally? ---
	// 1. Call slotMachine->spinReels()
	// 2. Call slotMachine->checkWinCondition()
	// 3. Call updateDisplay() to refresh the UI

	// potentially spin/check here directly for testing, then refine
	 if (slotMachine->spinReels()) {
		slotMachine->checkWinCondition();
		updateDisplay();
	 } else {
		// Update display to show "Not enough credits" or too poor
		messageLabel->setText("You are poor!");
	 }
}

void GameWindow::updateDisplay() {
	// Update reel labels
	for (int i = 0; i < slotMachine->getReelCount() && i < 3; ++i) {
		 if(reelLabels[i]) {
			 Symbol currentSymbol = slotMachine->getSymbolAt(i);
			 reelLabels[i]->setText(QString::fromStdString(currentSymbol.id));
		 }
	}

	// Update credits
	creditsLabel->setText("Credits: " + QString::number(slotMachine->getCredits()));

	// Reset message label
	// messageLabel->setText("Spin!"); // Or keep previous message
}

