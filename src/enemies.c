#include "enemies.h"

#include "goomba.h"
#include "koopa.h"

#define MAX_ENEMIES 64
#define TILE_SIZE (16 * FACTOR)

static Enemy enemies[MAX_ENEMIES];
static int enemyCount;

static int tileCol(float x)
{
    return (int)floorf(x / TILE_SIZE);
}

static int tileRow(float y)
{
    return (int)floorf(y / TILE_SIZE) - 1;
}

Rectangle enemyBounds(const Enemy *enemy)
{
    float height = enemy->type == ENEMY_KOOPA_GREEN &&
        enemy->state == ENEMY_WALKING ? 24 * FACTOR : 16 * FACTOR;
    return (Rectangle){ enemy->pos.x, enemy->pos.y, TILE_SIZE, height };
}

void moveEnemy(Enemy *enemy, float dt)
{
    Rectangle bounds = enemyBounds(enemy);
    enemy->pos.x += enemy->vel.x * dt;

    if (enemy->vel.x != 0.0f) {
        int col = tileCol(enemy->vel.x > 0.0f
            ? enemy->pos.x + TILE_SIZE - 0.001f : enemy->pos.x);
        int top = tileRow(enemy->pos.y);
        int bottom = tileRow(enemy->pos.y + bounds.height - 0.001f);
        for (int row = top; row <= bottom; row++) {
            if (!isMapSolidAt(row, col)) continue;
            enemy->pos.x = enemy->vel.x > 0.0f
                ? col * TILE_SIZE - TILE_SIZE : (col + 1) * TILE_SIZE;
            enemy->direction = -enemy->direction;
            break;
        }
    }

    enemy->vel.y += GRAVITY * dt;
    if (enemy->vel.y > 900.0f) enemy->vel.y = 900.0f;
    enemy->pos.y += enemy->vel.y * dt;

    if (enemy->vel.y >= 0.0f) {
        int row = tileRow(enemy->pos.y + bounds.height - 0.001f);
        int left = tileCol(enemy->pos.x);
        int right = tileCol(enemy->pos.x + TILE_SIZE - 0.001f);
        if (isMapSolidAt(row, left) || isMapSolidAt(row, right)) {
            enemy->pos.y = (row + 1) * TILE_SIZE - bounds.height;
            enemy->vel.y = 0.0f;
        }
    }

    if (enemy->pos.y > SCREEN_HEIGHT + TILE_SIZE) enemy->active = false;
}

void resetEnemies(void)
{
    int count;
    const EnemySpawn *spawns = getMapEnemySpawns(&count);
    enemyCount = count < MAX_ENEMIES ? count : MAX_ENEMIES;
    memset(enemies, 0, sizeof(enemies));
    for (int i = 0; i < enemyCount; i++) {
        enemies[i] = (Enemy){
            .type = spawns[i].type,
            .state = ENEMY_WALKING,
            .active = true,
            .pos = spawns[i].pos,
            .direction = -1
        };
    }
}

EnemyEvents updateEnemies(Mario *mario, float previousMarioBottom,
                          float cameraX, float dt)
{
    EnemyEvents events = { 0 };
    if (dt > 1.0f / 30.0f) dt = 1.0f / 30.0f;
    Rectangle marioRect = {
        mario->pos.x, mario->pos.y, mario->width, mario->height
    };

    for (int i = 0; i < enemyCount; i++) {
        Enemy *enemy = &enemies[i];
        if (!enemy->active) continue;
        if (!enemy->awake && enemy->pos.x <= cameraX + SCREEN_WIDTH + 32 * FACTOR)
            enemy->awake = true;
        if (!enemy->awake) continue;

        if (enemy->contactGrace > 0.0f) enemy->contactGrace -= dt;
        if (enemy->type == ENEMY_GOOMBA) updateGoomba(enemy, dt);
        else updateKoopa(enemy, dt);
        if (!enemy->active) continue;

        if (enemy->pos.x + TILE_SIZE < cameraX - TILE_SIZE) {
            enemy->active = false;
            continue;
        }
        if (enemy->state == ENEMY_STOMPED) continue;

        Rectangle bounds = enemyBounds(enemy);
        if (!CheckCollisionRecs(marioRect, bounds)) continue;
        if (enemy->contactGrace > 0.0f) continue;

        bool stomp = mario->vel.y > 0.0f &&
            previousMarioBottom <= bounds.y + 10 * FACTOR;
        if (stomp) {
            if (enemy->type == ENEMY_GOOMBA) stompGoomba(enemy);
            else if (enemy->state == ENEMY_SHELL_IDLE) {
                kickKoopa(enemy, mario->pos.x + mario->width / 2.0f);
                events.kicked = true;
            } else stompKoopa(enemy);
            mario->pos.y = bounds.y - mario->height;
            mario->vel.y = -400.0f;
            mario->onGround = false;
            events.stomped = true;
            marioRect.y = mario->pos.y;
        } else if (enemy->type == ENEMY_KOOPA_GREEN &&
                   enemy->state == ENEMY_SHELL_IDLE) {
            kickKoopa(enemy, mario->pos.x + mario->width / 2.0f);
            events.kicked = true;
        } else if (mario->invulnerableTimer <= 0.0f) {
            events.hitMario = true;
        }
    }

    // A moving Koopa shell defeats other enemies it touches.
    for (int i = 0; i < enemyCount; i++) {
        Enemy *shell = &enemies[i];
        if (!shell->active || !shell->awake ||
            shell->type != ENEMY_KOOPA_GREEN ||
            shell->state != ENEMY_SHELL_MOVING) continue;
        for (int j = 0; j < enemyCount; j++) {
            Enemy *other = &enemies[j];
            if (i == j || !other->active || !other->awake ||
                other->state == ENEMY_STOMPED ||
                !CheckCollisionRecs(enemyBounds(shell), enemyBounds(other))) continue;
            other->active = false;
        }
    }
    return events;
}

void drawEnemies(float cameraX)
{
    for (int i = 0; i < enemyCount; i++) {
        const Enemy *enemy = &enemies[i];
        if (!enemy->active || !enemy->awake ||
            enemy->pos.x + TILE_SIZE < cameraX ||
            enemy->pos.x > cameraX + SCREEN_WIDTH) continue;
        if (enemy->type == ENEMY_GOOMBA) drawGoomba(enemy, cameraX);
        else drawKoopa(enemy, cameraX);
    }
}
