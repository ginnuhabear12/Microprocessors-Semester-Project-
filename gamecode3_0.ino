#include <UTFT.h>
#include <UTFT_Geometry.h>


// Define analog pins for the D-pad
#define DPAD A0


//bool quitMenuActive = false; // Tracks whether the quit menu is active
unsigned long lastSW5Press = 0; // Tracks the last time SW5 was pressed
const unsigned long doublePressInterval = 500; // Interval for double press in milliseconds




bool isStartScreen = true;
int screenState = 0; // 0: Title Screen, 1: Instructions Screen, 2: Game Screen
int currentPlayer = 1; // Tracks the current player (1 or 2)
unsigned long player1Time = 0;
unsigned long player2Time = 0;


UTFT myGLCD(CTE40, 38, 39, 40, 41);
extern uint8_t BigFont[];


// Define thresholds for button detection
#define SW1_THRESHOLD 50
#define SW2_THRESHOLD 250
#define SW3_THRESHOLD 450
#define SW4_THRESHOLD 600
#define SW5_THRESHOLD 850


// Logical direction identifiers
const int DIR_LEFT = 1;
const int DIR_UP = 2;
const int DIR_DOWN = 3;
const int DIR_RIGHT = 4;


// Custom font character codes for arrows
const char LCD_LEFT = 80;
const char LCD_UP = 81;
const char LCD_DOWN = 82;
const char LCD_RIGHT = 83;


// Buzzer pin definition
#define BUZZER_PIN 7


// Melody Note Definitions
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0


int melody[] = {
  REST,1, REST,1, NOTE_C4,4, NOTE_E4,4, NOTE_G4,4, NOTE_E4,4,
  NOTE_C4,4, NOTE_E4,8, NOTE_G4,-4, NOTE_E4,4,
  NOTE_A3,4, NOTE_C4,4, NOTE_E4,4, NOTE_C4,4,
  NOTE_A3,4, NOTE_C4,8, NOTE_E4,-4, NOTE_C4,4
};


// Melody timing variables
int tempo = 160;
int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;






// Function declarations
void startGame();
void retryRound();
void nextRound();
void generateSequence();
void displaySequence();
int getButtonPress(int pin, unsigned long &lastReadTime, bool &lock);
void endGame();
char mapDirectionToChar(int direction);








// Game configuration
int sequenceLength = 4;          // Initial sequence length
int maxRounds = 10;              // Total rounds
int sequence[13];                // Store the sequence for the round
int playerProgress = 0;          // Track player's progress in the sequence
int currentRound = 1;            // Track current round
bool gameStarted = false;        // Game start flag
bool roundInProgress = false;    // Track if a round is in progress








// Timing variables
unsigned long gameStartTime = 0; // Start time of the game
unsigned long totalTime = 0;     // Total time to complete the game








// Debounce timing and lock mechanisms
unsigned long lastReadTime = 0;   // Last read time for the D-pad
unsigned long lastReadTimeSW5 = 0;// Last read time for SW5
const int debounceDelay = 50;     // Reduced debounce delay for better responsiveness
const int holdCheckDelay = 10;    // Further reduced for quicker response
bool dpadLocked = false;          // Lock status for the D-pad
bool sw5Locked = false;           // Lock status for SW5 button








extern uint8_t Various_Symbols_32x32[];










void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0)); // Seed random generator
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);




  // Show the title screen
  displayTitleScreen();
}
char mapDirectionToChar(int direction) {
  switch (direction) {
    case DIR_LEFT: return LCD_DOWN;  // Swapped
    case DIR_UP: return LCD_UP;
    case DIR_DOWN: return LCD_LEFT;  // Swapped
    case DIR_RIGHT: return LCD_RIGHT;
    default: return ' ';
  }
}


void displayVictoryScreen() {
  // Set the green background and display the victory message
  myGLCD.fillScr(VGA_GREEN); // Set green background
  myGLCD.setFont(BigFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_GREEN);
  myGLCD.print("Game Completed!", CENTER, 120);


  // Start playing the shortened victory melody immediately
  int halfNotes = notes / 2; // Play only the first half of the melody
  for (int thisNote = 0; thisNote < halfNotes * 2; thisNote += 2) {
    divider = melody[thisNote + 1];
    if (divider > 0) {
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // Dotted notes
    }


    // Play the note
    tone(BUZZER_PIN, melody[thisNote], noteDuration * 0.9);


    // Maintain the green screen while the song plays
    delay(noteDuration);
    noTone(BUZZER_PIN);
  }


  delay(500); // Keep the green screen for 2 seconds after the song finishes
}




void displayTitleScreen() {
  myGLCD.clrScr(); // Clear the screen




  // Set purple background
  myGLCD.fillScr(VGA_PURPLE);




  // Set font and color for the title
  myGLCD.setFont(BigFont); // Use BigFont for the title
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_PURPLE); // Match background color




  // Display the title centered at the top
  myGLCD.print("Welcome to", CENTER, 50);
  myGLCD.print("Direction Dynamics", CENTER, 90);




  // Display creators' names
  myGLCD.print("Created by Gia, Ginna,", CENTER, 200);
  myGLCD.print("and Luke", CENTER, 240);




  // Display prompt to continue
  myGLCD.print("Press SW5 to continue", CENTER, 280);
}




void displayInstructionsScreen() {
  myGLCD.clrScr(); // Clear the screen




  // Set font and color for instructions
  myGLCD.setFont(BigFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);




  // Display instructions
  myGLCD.print("Instructions:", CENTER, 10);
  myGLCD.print("Replicate the arrow", LEFT, 50);
  myGLCD.print("patterns displayed", LEFT, 90);
  myGLCD.print("with buttons.", LEFT, 110);
  myGLCD.print("There will be 10 rounds,", LEFT, 140);
  myGLCD.print("try to be the quickest.", LEFT, 170);
  myGLCD.print("Fastest time wins.", LEFT, 200);
  myGLCD.print("A wrong pattern results", LEFT, 230);
  myGLCD.print("in a 1-second delay!", LEFT, 260);




  // Display prompt to continue
  myGLCD.print("Press SW5 to continue", CENTER, 300);
}




void displayPlayerSelectionScreen() {
  myGLCD.clrScr(); // Clear the screen




  // Set font and color
  myGLCD.setFont(BigFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);




  // Display player selection message
  myGLCD.print("Press UP for Player 1", CENTER, 100);
  myGLCD.print("Press DOWN for Player 2", CENTER, 140);
}




void displayCountdown() {
  // Set green background and ensure it stays consistent
  myGLCD.fillScr(VGA_GREEN);




  // Set font and color for countdown
  myGLCD.setFont(BigFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_GREEN); // Ensure the text background is green




  // Display countdown numbers
  for (int i = 3; i > 0; i--) {
    myGLCD.fillScr(VGA_GREEN); // Keep the background green
    myGLCD.printNumI(i, CENTER, 120); // Display the number in the center
    delay(1000); // Wait for 1 second
  }




  // Reset the screen to black background for the game
  myGLCD.clrScr();
  myGLCD.setBackColor(VGA_BLACK); // Reset background color for the game
}




// Global variables for double-click detection
bool sw5FirstClick = false;          // Tracks the first click of SW5
unsigned long sw5FirstClickTime = 0; // Timestamp of the first SW5 click
const int doubleClickThreshold = 500; // Max time (ms) between two clicks for double-click
bool quitMenuActive = false;         // Flag to indicate quit menu is active


void loop() {
  int button = getButtonPress(DPAD, lastReadTimeSW5, sw5Locked);


  // Handle Quit Menu
  if (quitMenuActive) {
    int quitButton = getButtonPress(DPAD, lastReadTime, dpadLocked);
    if (quitButton == DIR_UP) { // Quit confirmed
      Serial.println("Quit Menu: Quit selected");
      displayGoodbyeScreen();
      quitMenuActive = false;
      while (true); // Stop the program
    } else if (quitButton == DIR_DOWN) { // Resume game
      Serial.println("Quit Menu: Resume selected");
      quitMenuActive = false;


      // Resume game based on the current state
      myGLCD.clrScr();
      if (screenState == 3) { // Game is active
        displayRoundNumber(); // Redisplay current round
        displaySequence();    // Redisplay sequence
      } else if (screenState == 2) { // Player selection
        displayPlayerSelectionScreen();
      }
    }
    return; // Exit loop to avoid interfering with other logic
  }


  // Handle screen transitions
  if (screenState == 0 && button == 5) { // Title Screen
    displayInstructionsScreen();
    screenState = 1; // Update to instructions screen state
    delay(500); // Debounce delay
  } else if (screenState == 1 && button == 5) { // Instructions Screen
    displayPlayerSelectionScreen();
    screenState = 2; // Update to player selection screen state
    delay(500); // Debounce delay
  } else if (screenState == 2) { // Player Selection Screen
    if (button == DIR_UP) { // Select Player 1
      currentPlayer = 1;
      screenState = 3; // Update to game screen state
      startGame();
      delay(500); // Debounce delay
    } else if (button == DIR_DOWN) { // Select Player 2
      currentPlayer = 2;
      screenState = 3; // Update to game screen state
      startGame();
      delay(500); // Debounce delay
    }
  }


  // Handle game logic when in the game screen
  if (screenState == 3 && roundInProgress) {
    int dpadButton = getButtonPress(DPAD, lastReadTime, dpadLocked);


    if (dpadButton != -1) {
      Serial.print("Player ");
      Serial.print(currentPlayer);
      Serial.print(" - Button Pressed: ");
      switch (dpadButton) {
        case DIR_LEFT: Serial.println("Left"); break;
        case DIR_UP: Serial.println("Up"); break;
        case DIR_DOWN: Serial.println("Down"); break;
        case DIR_RIGHT: Serial.println("Right"); break;
      }


      // Process button input during the game
      if (dpadButton == sequence[playerProgress]) {
        playerProgress++;
        Serial.print("Player ");
        Serial.print(currentPlayer);
        Serial.print(" - Sequence Progress: ");
        Serial.print(playerProgress);
        Serial.print("/");
        Serial.println(sequenceLength);


        if (playerProgress == sequenceLength) {
          nextRound();
        }
      } else {
        retryRound();
      }
    }
  }
}




void startGame() {
  displayCountdown(); // Show the countdown before starting




  gameStartTime = millis();   // Start timing the game
  roundInProgress = true;




  // Clear the screen and initialize the first round
  myGLCD.clrScr(); // Ensure a clean slate
  myGLCD.setBackColor(VGA_BLACK); // Set background to black for the game
  displayRoundNumber();
  generateSequence();
  displaySequence();
  delay(200); // Short delay to prevent interference
}








// Function to retry the current round if the player makes a mistake
// Function to retry the current round if the player makes a mistake
// Function to retry the current round if the player makes a mistake
void retryRound() {
  playerProgress = 0; // Reset player's progress in the sequence
  Serial.println("Retrying current round...");




  // Flash the screen red and play buzzer simultaneously
  myGLCD.fillScr(VGA_RED);   // Turn the screen red
  tone(BUZZER_PIN, 440, 300); // Play the first "error" tone
  delay(300);
  tone(BUZZER_PIN, 330, 300); // Play the second "error" tone
  delay(300);
  noTone(BUZZER_PIN);         // Stop the buzzer
  delay(400);                 // Keep the red screen on for the remainder of 1 second




  myGLCD.clrScr();            // Clear the screen back to default
  displayRoundNumber();       // Redisplay the round number
  displaySequence();          // Redisplay the sequence
}








// Function to move to the next round or end the game
void nextRound() {
  if (playerProgress == sequenceLength) { // Only proceed if the sequence is correct
    if (currentRound < maxRounds) {
      currentRound++;
      sequenceLength++;
      playerProgress = 0;
      roundInProgress = true;




      // Clear the screen and show the new round number
      myGLCD.clrScr();
      displayRoundNumber(); // Display the updated round number
      generateSequence();   // Generate the new sequence
      displaySequence();    // Display the new sequence
    } else {
      endGame(); // End the game if all rounds are completed
    }
  } else {
    retryRound(); // Retry the current round if the sequence was incorrect
  }
}




// Function to display the round number at the top center of the screen
void displayRoundNumber() {
  myGLCD.setFont(BigFont);
  myGLCD.setColor(VGA_WHITE);
  String roundText = "Round " + String(currentRound);
  int textWidth = myGLCD.getDisplayXSize() / 2 - (roundText.length() * 8);  // Center text calculation
  myGLCD.print(roundText, CENTER, 10);
}








void endGame() {
  unsigned long elapsedTime = millis() - gameStartTime; // Calculate elapsed time
  if (currentPlayer == 1) {
    player1Time = elapsedTime;
  } else if (currentPlayer == 2) {
    player2Time = elapsedTime;
  }

  roundInProgress = false;
  gameStarted = false; // Reset the game start flag to allow replay

  // Display the victory screen
  displayVictoryScreen();

  // Clear the screen and set background to black for the scoreboard
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK); // Ensure black background for text

  // Display "Scoreboard" title
  myGLCD.print("Scoreboard", CENTER, 20);

  // Display Player 1 time
  if (player1Time > 0) {
    myGLCD.print("Player 1 Time: ", LEFT, 100);
    myGLCD.printNumF((float)player1Time / 1000.0, 2, RIGHT, 100);
  }

  // Display Player 2 time
  if (player2Time > 0) {
    myGLCD.print("Player 2 Time: ", LEFT, 130);
    myGLCD.printNumF((float)player2Time / 1000.0, 2, RIGHT, 130);
  }

  // If only one player has played, show "Next Player's Turn"
  if (currentPlayer == 1 && player2Time == 0) {
    myGLCD.print("Next Player's Turn:", CENTER, 200);
    myGLCD.print("Press SW5", CENTER, 230);

    // Wait for SW5 to start Player 2's turn
    while (true) {
      int button = getButtonPress(DPAD, lastReadTime, dpadLocked);
      if (button == 5) { // SW5 pressed
        currentPlayer = 2; // Switch to Player 2
        resetGameState();
        startGame();
        break;
      }
    }
  } else { // Both players have played; show the winner and rematch/quit options
    // Determine the winner
    if (player1Time > 0 && player2Time > 0) {
      if (player1Time < player2Time) {
        myGLCD.print("Player 1 Wins!", CENTER, 180);
      } else if (player1Time > player2Time) {
        myGLCD.print("Player 2 Wins!", CENTER, 180);
      } else {
        myGLCD.print("It's a Tie!", CENTER, 180);
      }
    }

    // Display rematch or quit prompt
    myGLCD.print("Press UP to Rematch", CENTER, 220);
    myGLCD.print("Press DOWN to Quit", CENTER, 250);

    delay(500); // Add a small delay to prevent button press carryover

    // Reset the debounce lock
    sw5Locked = false;

    // Wait for player input
    while (true) {
      int button = getButtonPress(DPAD, lastReadTime, dpadLocked);
      if (button == DIR_UP) { // Player selects rematch
        Serial.println("Rematch selected");
        player1Time = 0; // Reset Player 1 time
        player2Time = 0; // Reset Player 2 time
        resetGameState();
        displayPlayerSelectionScreen(); // Bring back to player selection screen
        screenState = 2; // Update to player selection screen state
        break;
      } else if (button == DIR_DOWN) { // Player selects quit
        Serial.println("Quit selected");
        displayGoodbyeScreen();
        while (true); // Stop further execution, end the program
      }
    }
  }
}


int getButtonPress(int pin, unsigned long &lastReadTime, bool &lock) {
  int initialReading = analogRead(pin);


  if (initialReading > SW5_THRESHOLD) {
    lock = false;
    return -1;
  }


  if (!lock && millis() - lastReadTime > debounceDelay) {
    delay(holdCheckDelay);
    int reading = analogRead(pin);
    lastReadTime = millis(); // Set the lastReadTime to prevent flicker
    lock = true;


    if (reading <= SW1_THRESHOLD) return DIR_LEFT;
    else if (reading <= SW2_THRESHOLD) return DIR_UP;
    else if (reading <= SW3_THRESHOLD) return DIR_DOWN;
    else if (reading <= SW4_THRESHOLD) return DIR_RIGHT;
    else if (reading <= SW5_THRESHOLD) return 5; // Special SW5 button
  }


  return -1;
}








void resetGameState() {
  sequenceLength = 4;
  currentRound = 1;
  playerProgress = 0;
  roundInProgress = true;




  // Reset debounce locks
  dpadLocked = false;
  sw5Locked = false;




  // Reset sequence array
  for (int i = 0; i < 13; i++) {
    sequence[i] = 0;
  }
}


// Generate a random sequence for the round using logical identifiers
void generateSequence() {
  for (int i = 0; i < sequenceLength; i++) {
    sequence[i] = random(DIR_LEFT, DIR_RIGHT + 1); // Generate random logical identifiers (1-4 for directions)
  }
}








// Display the sequence of arrows dynamically spaced to fit and centered on the screen
void displaySequence() {
  myGLCD.setFont(Various_Symbols_32x32);

  int screenWidth = 319;       // Screen width in pixels
  int arrowWidth = 40;         // Approximate width of each arrow symbol
  int y = 120;                 // Starting y position
  int maxArrowsPerLine = screenWidth / arrowWidth; // Max arrows that fit on one line

  for (int i = 0; i < sequenceLength; ) {
    int arrowsInLine = min(sequenceLength - i, maxArrowsPerLine); // Arrows to display in the current line
    int totalLineWidth = arrowsInLine * arrowWidth; // Total width of arrows in the current line
    int x = (screenWidth - totalLineWidth) / 2; // Calculate starting x to center the arrows

    for (int j = 0; j < arrowsInLine; j++) {
      char displayChar = mapDirectionToChar(sequence[i]);
      myGLCD.printChar(displayChar, x, y); // Print the arrow at the calculated position
      x += arrowWidth; // Move to the next arrow position
      i++; // Move to the next sequence item
    }

    y += 40; // Move to the next row for wrapped arrows
  }
}

void displayGameTime() {
  myGLCD.setFont(BigFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);




  // Calculate elapsed time
  unsigned long elapsedTime = millis() - gameStartTime;
  float elapsedSeconds = (float)elapsedTime / 1000.0;




  // Clear only the timer area (strictly top-right corner)
  myGLCD.fillRect(240, 0, 319, 30); // Adjust to fit only the timer




  // Display the elapsed time in the top-right corner
  myGLCD.printNumF(elapsedSeconds, 1, 250, 0); // Time with 1 decimal place
}
void displayQuitMenu() {
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);


  // Display the quit menu
  myGLCD.print("Do You Want to Quit?", CENTER, 120);
  myGLCD.print("Press UP for Yes", CENTER, 180);
  myGLCD.print("Press DOWN for No", CENTER, 220);
}
void displayGoodbyeScreen() {
  myGLCD.clrScr(); // Clear the screen

  // Set black background
  myGLCD.fillScr(VGA_BLACK);

  // Set font and color for the goodbye message
  myGLCD.setFont(BigFont);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setBackColor(VGA_BLACK);

  // Display the goodbye message
  myGLCD.print("Bye!", CENTER, 120);
}

