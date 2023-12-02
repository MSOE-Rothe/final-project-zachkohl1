# Game.c

By: Zach Kohlman
Class: CPE 2600/121

This is the main implementation file for a simple terminal-based game. The game is built using the ncurses library in C and features multi-threading for enemy movement.

## Game Description

The game is a simple terminal-based game where the player must reach the target point to progress to the next level. The player is represented by the character 'P' and can be moved using the arrow keys. Enemies are represented by the character 'E' and move downwards. The player must avoid the enemies while trying to reach the target point, represented by the character 'T'. The game features multiple levels with increasing difficulty. The player's score is based on the number of levels completed.

## Memory Leaks

The game has 9 memory leaks. These leaks are caused by the ncurses library and are not caused by the game itself. The leaks are caused by the following functions:

- `initscr()`
- `init_pair()`
- 'getch()'

## Features

- Player movement with arrow keys
- Asynchronous randomized enemy spawning and movement
- Collision detection with enemies and target points
- Level progression with increasing difficulty
- Colorful game elements

## Game Elements

- **Player**: Represented by the character 'P' and can be moved using the arrow keys.
- **Enemies**: Represented by the character 'E'. They are spawned randomly and move downwards.
- **Target Point**: Represented by the character 'T'. The player's goal is to reach the target point to progress to the next level.

## Functions

- `main()`: Initializes the game screen, sets up colors, starts the game loop, and prints the level reached after the game is over.
- `setup()`: Sets up the game environment and initializes necessary variables.
- `draw()`: Draws the game screen with the play area, player, target point, enemies, level, and score.
- `input()`: Handles user input for moving the player character.
- `spawnEnemies()`: Spawns enemies in the game.
- `moveEnemies()`: Moves the enemies in the game. This function is called in a separate thread to allow for asynchronous movement i.e. the enemies move while the player is moving.
- `checkCollision()`: Checks for collisions between the player and enemies, as well as the player and the target point.
- `moveEnemiesThread()`: Function that moves the enemies in a separate thread.

## Compilation

To compile the game, you need to have the ncurses library installed. You can compile the game using gcc:

```bash
gcc -o game game.c -lncurses -lpthread
```

Or with the provided Makefile:

```bash
make
```
