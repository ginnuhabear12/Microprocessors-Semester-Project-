# Microprocessors-Semester-Project
# Direction Dynamics 🎮

A two-player pattern-matching game built on an Arduino Mega 2560 with a 3.5" LCD display. Players must replicate sequences of directional arrows using a D-pad controller, competing to complete all 10 rounds in the shortest time.

**Created by:** Gia Golecki, Virginia (Ginna) Hebert, and Luke Marty — EECE 342, December 2024

---

## How to Play

1. Power on the device — the **title screen** appears with the game name and credits.
2. Press **SW5** to advance to the **instructions screen**.
3. Press **SW5** again to reach the **player selection screen**.
4. Press **UP** to play as Player 1, or **DOWN** to play as Player 2.
5. A **3-2-1 countdown** plays, then the rounds begin.
6. Each round displays a sequence of directional arrows on the LCD. Replicate the sequence using the D-pad in the correct order.
7. A **wrong input** flashes the screen red, plays a buzzer sound, and adds a ~1-second penalty before restarting the round.
8. After Player 1 finishes, Player 2 is prompted to play. The **scoreboard** then shows both times and declares a winner.
9. At the end, press **UP** to rematch (returns to player selection) or **DOWN** to quit.

---

## Game Structure

| Round | Sequence Length |
|-------|----------------|
| 1     | 4 arrows        |
| 2     | 5 arrows        |
| ...   | ...             |
| 10    | 13 arrows       |

Sequences grow by one arrow each round. The player with the lowest total completion time wins.

---

## Hardware Requirements

| Component | Details |
|-----------|---------|
| Microcontroller | Arduino Mega 2560 |
| Display | 3.5" LCD (CTE40, landscape mode) |
| Input | Analog-to-digital D-pad (connected to pin `A0`) |
| Audio | Active buzzer (connected to pin `7`) |
| Other | Breadboard, stacking header pins |

---

## Software / Library Dependencies

- [`UTFT`](http://www.rinkydinkelectronics.com/library.php?id=51) — LCD display driver
- [`UTFT_Geometry`](http://www.rinkydinkelectronics.com/library.php?id=97) — geometry extensions for UTFT
- `Various_Symbols_32x32` font — custom font package required to render arrow characters on the LCD (included as an external font file referenced via `extern uint8_t Various_Symbols_32x32[]`)
- `BigFont` — standard UTFT font for text display

---

## Pin Mapping

| Pin | Function |
|-----|---------|
| `A0` | D-pad analog input |
| `7`  | Active buzzer output |
| `38–41` | LCD display data/control lines (CTE40 config) |

### D-pad Button Thresholds (Analog Read)

| Button | Threshold | Direction |
|--------|-----------|-----------|
| SW1 | ≤ 50 | Left |
| SW2 | ≤ 250 | Up |
| SW3 | ≤ 450 | Down |
| SW4 | ≤ 600 | Right |
| SW5 | ≤ 850 | Confirm / Navigate |

> **Note:** The physical D-pad wiring results in a left/down swap in software. `mapDirectionToChar()` compensates for this by remapping `DIR_LEFT → LCD_DOWN` and `DIR_DOWN → LCD_LEFT`.

---

## Code Overview

### Key Global State

```cpp
int screenState;       // 0=Title, 1=Instructions, 2=Player Select, 3=Game
int currentPlayer;     // 1 or 2
int currentRound;      // 1–10
int sequenceLength;    // Starts at 4, increases each round
int sequence[13];      // Current round's arrow sequence
int playerProgress;    // How many arrows the player has matched
unsigned long player1Time, player2Time; // Completion times in ms
```

### Core Functions

| Function | Description |
|----------|-------------|
| `setup()` | Initializes serial, LCD, random seed, and shows the title screen |
| `loop()` | Main state machine — handles screen transitions and game input |
| `startGame()` | Starts the countdown, initializes round state, and begins Round 1 |
| `generateSequence()` | Fills `sequence[]` with random directions (1–4) |
| `displaySequence()` | Renders the arrow sequence centered on the LCD using the symbols font |
| `getButtonPress()` | Reads the analog D-pad with debounce logic; returns a direction constant or `-1` |
| `nextRound()` | Advances to the next round or calls `endGame()` if all rounds are complete |
| `retryRound()` | Resets player progress, flashes red, plays error tones, and redisplays the sequence |
| `endGame()` | Records time, displays the victory screen and scoreboard, handles rematch/quit |
| `resetGameState()` | Resets all round/sequence/progress variables for a new game |
| `mapDirectionToChar()` | Maps logical direction IDs to the correct LCD arrow characters |
| `displayVictoryScreen()` | Shows green "Game Completed!" screen and plays the victory melody |
| `displayGoodbyeScreen()` | Shows "Bye!" on a black screen when the player quits |

### Screen States

```
[0] Title Screen
      ↓ SW5
[1] Instructions Screen
      ↓ SW5
[2] Player Selection Screen
      ↓ UP (Player 1) or DOWN (Player 2)
[3] Game Screen (rounds 1–10)
      ↓ Complete all rounds
    Victory Screen → Scoreboard
      ↓ UP = Rematch (→ [2]) | DOWN = Quit
    Goodbye Screen
```

---

## Known Limitations & Future Work

- **No simultaneous two-player mode** — both players share one D-pad and take turns. The original design called for two simultaneous D-pads, but handling concurrent inputs caused delays and was abandoned.
- **No live timer display** — elapsed time could not be shown during gameplay without corrupting the arrow display, due to conflicts between the symbols font and standard text rendering.
- **No in-game quit menu** — a double-press quit mechanism was partially implemented (`displayQuitMenu()` exists in the code) but removed due to input interference during rounds.
- **Fixed font size** — a larger font for arrows was desired but required complex font packages that weren't pursued within the project timeline.
- **No physical enclosure** — the hardware is exposed on a breadboard; a custom enclosure was planned but not built.

---

## Project Background

This project was developed for EECE 342. The initial concept involved two players competing simultaneously with separate D-pads and a point-per-round scoring system. Several adjustments were made during development:

- Switched from simultaneous to turn-based play to avoid input timing conflicts
- Changed scoring from per-round points to total completion time
- Reduced rounds from 13 to 10
- Replaced a passive speaker melody with an active buzzer
- Added a red-screen penalty with buzzer for incorrect inputs
