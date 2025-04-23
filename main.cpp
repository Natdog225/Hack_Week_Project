#include "GameWindow.h"
#include "SlotMachine.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	try {
		// Create the game instance
		SlotMachine machineLogic(3, 100); // 3 reels, 100 starting credits

		// Create the main window
		GameWindow mainWindow(&machineLogic);
		mainWindow.show(); // Show it

		// Start the Qt event loop
		return app.exec();

	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		// uncomment to show error in a Qt message box
		// QMessageBox::critical(nullptr, "Error", e.what());
		return 1; // error
	} catch (...) {
		std::cerr << "An unknown error occurred." << std::endl;
		 // Same thing as above
		// QMessageBox::critical(nullptr, "Error", "An unknown error occurred.");
		return 1;
	}
}
