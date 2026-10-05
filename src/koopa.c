#include "koopa.h"

#define KOOPA_SPEED (20.0f * FACTOR)
#define SHELL_SPEED (180.0f * FACTOR)
#define SHELL_RECOVERY_TIME 5.0f

void updateKoopa(Enemy *enemy, float dt)
{
    if (enemy->state == ENEMY_SHELL_IDLE) {
        enemy->timer += dt;
        if (enemy->timer >= SHELL_RECOVERY_TIME) {
            enemy->state = ENEMY_WALKING;
            enemy->pos.y -= 8 * FACTOR;
            enemy->timer = 0.0f;
        }
        enemy->vel.x = 0.0f;
        moveEnemy(enemy, dt);
        return;
    }

    enemy->vel.x = (enemy->state == ENEMY_SHELL_MOVING
        ? SHELL_SPEED : KOOPA_SPEED) * enemy->direction;
    moveEnemy(enemy, dt);
    enemy->timer += dt;
}

void stompKoopa(Enemy *enemy)
{
    if (enemy->state == ENEMY_WALKING) {
        // The 16-pixel shell keeps the walking Koopa's feet on the ground.
        enemy->pos.y += 8 * FACTOR;
    }
    enemy->state = ENEMY_SHELL_IDLE;
    enemy->vel.x = 0.0f;
    enemy->timer = 0.0f;
    enemy->contactGrace = 0.2f;
}

void kickKoopa(Enemy *enemy, float marioCenterX)
{
    enemy->state = ENEMY_SHELL_MOVING;
    enemy->direction = marioCenterX < enemy->pos.x + 8 * FACTOR ? 1 : -1;
    enemy->contactGrace = 0.2f;
}

void drawKoopa(const Enemy *enemy, float cameraX)
{
    if (enemy->state != ENEMY_WALKING) {
        DrawTexturePro(textureMap.koopaShellGreen,
            (Rectangle){ 0, 0, 16, 14 },
            (Rectangle){ enemy->pos.x - cameraX, enemy->pos.y + 2 * FACTOR,
                         16 * FACTOR, 14 * FACTOR },
            (Vector2){ 0, 0 }, 0, WHITE);
        return;
    }

    int frame = ((int)(enemy->timer / 0.15f)) % 2;
    DrawTexturePro(textureMap.koopaGreen,
        (Rectangle){ frame * 16, 0, enemy->direction < 0 ? -16 : 16, 24 },
        (Rectangle){ enemy->pos.x - cameraX, enemy->pos.y,
                     16 * FACTOR, 24 * FACTOR },
        (Vector2){ 0, 0 }, 0, WHITE);
}
