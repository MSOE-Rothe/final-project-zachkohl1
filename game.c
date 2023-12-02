
/**
 * @file game.c
 * @brief Implementation of the game logic and main function.
 * 
 * This file contains the implementation of the game logic, including functions for setting up the game environment,
 * moving enemies, handling user input, spawning enemies, checking collisions, and drawing the game screen.
 * The main function initializes the game screen, sets up colors, starts the game loop, and prints the level reached
 * after the game is over.
 * 
 * @author Zach Kohlman, CPE 2600/121
 * @date 12/2/2023
 */
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <curses.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "game.h"

#define WIDTH 100
#define HEIGHT 30
#define MAX_ENEMIES 50    // Also serves as MAX_LEVELS
#define BOUNDARY_CHARS 2
#define PLAYER_START_X BOUNDARY_CHARS;
#define PLAYER_START_Y (HEIGHT/2) - BOUNDARY_CHARS

// File scope variables
static int level = 1;
static int num_enemies = 5;
static int gameOver = 0;
static int spawnCounter = 0;
static pthread_mutex_t mutex;
static pthread_t thread;
static Point player;
static Point targetPoint;
static Enemy enemies[MAX_ENEMIES];




/**
 * @brief Function that moves the enemies in a separate thread.
 * 
 * This function is responsible for continuously moving the enemies in the game.
 * It is executed in a separate thread to allow for concurrent execution with other game logic.
 * The enemies are moved at a fixed interval of 100 milliseconds (100000 microseconds).
 * 
 * @param arg Unused argument for thread function.
 * @return NULL
 */
void* moveEnemiesThread(void *arg){
    pthread_mutex_lock(&mutex);
    while(!gameOver)
    {
        moveEnemies();
        usleep(100000);
    }

    pthread_mutex_unlock(&mutex);
    return NULL;
}

/**
 * @brief The main function of the game.
 * 
 * This function initializes the game screen, sets up colors, and starts the game loop.
 * It creates a separate thread for moving enemies and waits for the game to be over.
 * After the game is over, it prints the level reached and returns 0.
 * 
 * @return 0 indicating successful execution of the program.
 */
int main(void) {
    // Initialize the game screen
    initscr();

    // Set up colors for different game elements
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);  // Player color
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK);  // Target color
    init_pair(3, COLOR_RED, COLOR_BLACK);  // Enemy color
    init_pair(4, COLOR_BLUE, COLOR_BLACK);  // Enemy color
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);  // Enemy color
    init_pair(6, COLOR_CYAN, COLOR_BLACK);  // Enemy color
    init_pair(7, COLOR_WHITE, COLOR_BLACK);  // Enemy color

    // Set up the game
    setup();

    // Game loop
    while (!gameOver) {
        input();
        checkCollision();
        draw();
        spawnEnemies();  // Call spawnEnemies() in the game loop
    }
    // Clean up the game screen
    endwin();

    // Wait for the enemy movement thread to finish
    pthread_join(thread, NULL);


    // Print the level reached
    printf("Game Over! Level Reached: %d\n", level);

	pthread_mutex_destroy(&mutex);
    pthread_exit(&thread);


    return 0;
}

/**
 * @brief Sets up the game environment and initializes necessary variables.
 * 
 * This function initializes the ncurses library, clears the screen, disables
 * echoing of input, enables non-blocking input, sets the timeout for getch(),
 * sets the player's starting position, generates a random target point, initializes
 * the enemies, and spawns initial enemies.
 */
void setup(void) {

    // Create a separate thread for moving enemies
    pthread_create(&thread, NULL, moveEnemiesThread, NULL);

    // Initalize mutex
	pthread_mutex_init(&mutex, NULL);

    clear();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(100);  // Set a timeout for getch() to enable non-blocking input

    player.x = PLAYER_START_X
    player.y = PLAYER_START_Y;

    targetPoint.x = rand() % (WIDTH - 2 * BOUNDARY_CHARS) + BOUNDARY_CHARS;
    targetPoint.y = rand() % (HEIGHT - 2 * BOUNDARY_CHARS) + BOUNDARY_CHARS;

    // Initialize enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = 0;
    }

    srand(time(NULL));

    endwin();
    // Spawn initial enemies
    spawnEnemies();
}



/**
 * @brief Draws the game screen with the play area, player, target point, enemies, level, and score.
 * 
 * This function clears the screen and then draws the play area, player, target point, enemies, level, and score.
 * The play area is represented by a rectangular border. The player is represented by the character 'P'.
 * The target point is represented by the character 'T'. The enemies are represented by the character 'E'.
 * The level and score are displayed at the top left corner of the screen.
 * 
 * @param None
 * @return None
 */
void draw(void) {
    clear();

    // Draw play area
    for (int i = 0; i < WIDTH; i++) {
        mvprintw(1, i, "#"); // Top edge
        mvprintw(HEIGHT - 2, i, "#"); // Bottom edge
    }

    for(int i = 1; i < HEIGHT - 1; i++) {
        mvprintw(i, 0, "#"); // Left edge
        mvprintw(i, WIDTH - 1, "#"); // Right edge
    }

    // Draw player
    attron(COLOR_PAIR(1));
    mvprintw(player.y, player.x, "P");
    attroff(COLOR_PAIR(1));


    // Draw target Point
    attron(COLOR_PAIR(2));
    mvprintw(targetPoint.y, targetPoint.x, "T");
    attroff(COLOR_PAIR(2));

    // Draw enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            attron(COLOR_PAIR(enemies[i].color));
            mvprintw(enemies[i].position.y, enemies[i].position.x, "E");
            attroff(COLOR_PAIR(enemies[i].color));
        }
    }

    // Draw level and score
    mvprintw(0, 0, "Level: %d\n", level);

    refresh();
}

/**
 * Handles user input for moving the player character.
 * @param void
 * @return void
 */
void input(void) {

    // Check the key and perform corresponding actions
    switch (getch()) {
        case KEY_LEFT:
            // Move the player left if within the boundary
            if (player.x > BOUNDARY_CHARS-1)
                player.x--;
            break;
        case KEY_RIGHT:
            // Move the player right if within the boundary
            if (player.x < WIDTH - BOUNDARY_CHARS)
                player.x++;
            break;
        case KEY_UP:
            // Move the player up if within the boundary
            if (player.y > BOUNDARY_CHARS)
                player.y--;
            break;
        case KEY_DOWN:
            // Move the player down if within the boundary
            if ((HEIGHT-BOUNDARY_CHARS-1)-player.y > 0)
                player.y++;
            break;
        case 27:  // ESC key
            // Set the game over flag to exit the game
            gameOver = 1;
            return;
    }
}

/**
 * Spawns enemies in the game.
 * Adjust the spawn rate by modifying the value of spawnCounter.
 * 
 * @param void
 * @return void
 */
void spawnEnemies(void) {
    if (spawnCounter >= 10) {  // Adjust this value to control the spawn rate
        for (int i = 0; i < num_enemies; i++) {
            if (!enemies[i].active) {
                enemies[i].position.x = rand() % (WIDTH - 2 * BOUNDARY_CHARS) + BOUNDARY_CHARS;
                enemies[i].position.y = 1;
                enemies[i].color = (rand() % 5) + 3;  // Assign a random color
                enemies[i].active = 1;
                break;  // Only spawn one enemy at a time
            }
        }
        spawnCounter = 0;  // Reset the counter
    } else {
        spawnCounter++;
    }
}

/**
 * Moves the enemies in the game.
 * If an enemy is active, it moves it down by one unit on the y-axis.
 * If an enemy reaches the bottom of the screen, it respawns at the top of the screen with a random x-coordinate.
 */
void moveEnemies(void) {
    for (int i = 0; i < num_enemies; i++) {
        if (enemies[i].active) {
            if (enemies[i].position.y < (HEIGHT- BOUNDARY_CHARS-1)) {
                enemies[i].position.y++;
            } else {
                // Respawn the enemy at the top of the screen
                enemies[i].position.y = 0;
                enemies[i].position.x = rand() % (WIDTH - 2 * BOUNDARY_CHARS) + BOUNDARY_CHARS;
            }
        }
    }
}

/**
 * Checks for collisions between the player and enemies, as well as the player and the target point.
 * If a collision occurs with an enemy, the game is over.
 * If the player reaches the target point, the level is incremented and the player is reset.
 * If the player has reached the maximum level, the game is over.
 */
void checkCollision(void) {    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active &&
            player.x == enemies[i].position.x &&
            player.y == enemies[i].position.y) {
            gameOver = 1;
            return;
        }
    }

    if (player.x == targetPoint.x && player.y == targetPoint.y) {
        level++;
        num_enemies = (num_enemies < MAX_ENEMIES)? num_enemies + 1 : MAX_ENEMIES;

        // Randomize target poitn location for next level
        targetPoint.x = rand() % (WIDTH - 2 * BOUNDARY_CHARS) + BOUNDARY_CHARS;
        targetPoint.y = rand() % (HEIGHT - 2 * BOUNDARY_CHARS) + BOUNDARY_CHARS;

        // Reset player
        player.x = PLAYER_START_X;
        player.y = PLAYER_START_Y;



        // Increase the number of enemies with each level
        if (level <= MAX_ENEMIES) {
            spawnEnemies();
        } else {
            // Player has won all levels
            gameOver = 1;
        }
    }
}
