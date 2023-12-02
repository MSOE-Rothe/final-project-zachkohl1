#ifndef GAME_H
#define GAME_H

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

