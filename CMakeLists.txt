cmake_minimum_required(VERSION 3.16)
project(SlotMachineMVP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find the Qt6 package
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(SlotMachineMVP
	main.cpp
	Reel.cpp
	SlotMachine.cpp
	GameWindow.cpp
	# Add other .cpp files here later, you know, when they're done.
	# Add .h files if not automatically found
)

# Link the Qt libraries to your executable
target_link_libraries(SlotMachineMVP PRIVATE
	Qt6::Core
	Qt6::Gui
	Qt6::Widgets
)

# Add include directories if your headers are in specific folders
# target_include_directories(SlotMachineMVP PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

