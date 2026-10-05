#pragma once

#include "mario.h"

typedef enum {
    ENEMY_WALKING,
    ENEMY_STOMPED,
    ENEMY_SHELL_IDLE,
    ENEMY_SHELL_MOVING
} EnemyState;

typedef struct {
    enum EnemyType type;
    EnemyState state;
    bool active;
    bool awake;
    Vector2 pos;
    Vector2 vel;
    float timer;
    float contactGrace;
    int direction;
} Enemy;

typedef struct {
    bool stomped;
    bool kicked;
    bool hitMario;
} EnemyEvents;

void resetEnemies(void);
EnemyEvents updateEnemies(Mario *mario, float previousMarioBottom,
                          float cameraX, float dt);
void drawEnemies(float cameraX);

// Shared terrain movement used by the individual enemy modules.
void moveEnemy(Enemy *enemy, float dt);
Rectangle enemyBounds(const Enemy *enemy);
