#ifndef GAME_H
#define GAME_H

/**
 * @file game.h
 * @brief Header file containing the declarations of structures and functions for the game.
 * @author Zach Kohlman, CPE 2600/121
 * @date 12/2/2023
 */

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point position;
    int active;
    int color;
} Enemy;

// Function prototypes
void setup(void);
void draw(void);
void input(void);
void spawnEnemies(void);
void moveEnemies(void);
void checkCollision(void);


#endif  /* Compile guard */

