+---------------------+      +-------------------------+      +----------------------+
|     [Start App]     | ---> | Initialize SlotMachine  | ---> | Initialize GameWindow|
| (main.cpp executes) |      | (Credits, Reels, Rules) |      | (UI Setup, Sounds,   |
+---------------------+      +-------------------------+      |  Load Images)        |
                                                               +----------+-----------+
                                                                          |
                                                                          V
+-------------------------------------------------------------------------+-----+
|                                  [Idle State]                                 |
|                    (Display Initial UI, Wait for User Input)                  |
+-----------------------------------------+-------------------------------------+
                                          |
                +-------------------------+-------------------------+
                |                         |                         |
                V                         V                         V
  < User Clicks +/- Bet >     < User Clicks Spin Button >     < User Closes Window >
                |                         |                         |
                V                         V                         V
+---------------------------+   +---------------------------+   +-----------+
|   GameWindow::adjustBet   |   | GameWindow::handleSpinBtn |   |  [End]    |
| (Play sound, call         |   | (Checks state, credits)   |   +-----------+
| SlotMachine::setSelectedBet,|   +------------+------------+
| updateDisplay)            |                | (If OK to Spin)
+--------------+------------+                V
               |              +---------------------------+
               |              |   Set isSpinning = true   |
               |              |   Disable Buttons         |
               |              |   Play Spin Sound         |
               |              |   Call SlotMachine::spinReels |
               |              |   Start animationTimer    |
               |              |   Schedule stopReelAnimation|
               |              |   (for Reel 0, 1, 2)      |
               |              +------------+------------+
               |                           |
               +---------------------------+ (Back to Idle)
                                           |
                                           V
                                +----------------------+
                                | [Animation Running]  |
                                | (animationTimer ticks|
                                | calls animateReelStep|
                                | which updates labels |
                                | randomly)            |
                                +----------+-----------+
                                           | (Scheduled Timers Fire)
                                           V
                                +---------------------------+
                                | GameWindow::stopReelAnim. |
                                | (for specific reel index) |
                                | - Mark reel visually stop |
                                | - updateReelDisplay(idx)  |
                                | - Check if all stopped    |
                                +------------+------------+
                                             | (If NOT all stopped)
                                             +---------------------> (Back to Animation Running)
                                             |
                                             | (If YES, all stopped)
                                             V
                                +---------------------------+
                                | - Stop animationTimer     |
                                | - Call SlotMachine::      |
                                |   checkWinCondition()     |
                                | - Update Credits Label    |
                                +------------+------------+
                                             |
                            +----------------+----------------+
                            | (Payout > 0 ?)                  |
                            V (Yes)                           V (No)
                +---------------------------+    +---------------------------+
                | GameWindow::startWinSeq.  |    | - Set "Lose" Message      |
                | (Play Win Sound, Start    |    | - Set isSpinning = false  |
                | Flash Timer, Schedule     |    | - Enable Buttons          |
                | Button Re-enable)         |    +------------+------------+
                | - Set "Win" Message       |                 |
                +------------+------------+                 |
                             | (Flash Timer Ticks)           |
                             V                               |
                +---------------------------+                |
                | GameWindow::flashStep     |                |
                | (Toggle background style) |                |
                +------------+------------+                |
                             | (Flash Timer Finishes/Stops)  |
                             V                               |
                +---------------------------+                |
                | GameWindow::stopFlashing  |                |
                | (Restore style)           |                |
                +------------+------------+                |
                             | (Scheduled Re-enable Timer)   |
                             V                               |
                +---------------------------+                |
                | - Set isSpinning = false  |                |
                | - Enable Buttons          |                |
                +------------+------------+                |
                             |                               |
                             +--------+----------------------+
                                      |
                                      V
                               (Back to Idle State)

Explanation of Symbols:
[Process] : A rectangular box representing a processing step (e.g., initializing, calculating).
<Decision> : A diamond shape representing a decision point (e.g., checking credits, checking for win).
[I/O] or [State] : Often represented by parallelograms or rounded rectangles, indicating input/output or a specific state the program is in. Here, used more generally for states or UI actions.
--> / | / V : Arrows indicating the direction of flow.

This flowchart outlines the main sequence of events from starting the app, handling user input for betting and spinning, running the animation, checking the win condition, triggering effects, and returning to an idle state.