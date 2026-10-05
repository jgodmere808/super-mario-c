#include "goomba.h"

#define GOOMBA_SPEED (20.0f * FACTOR)
#define STOMP_DURATION 0.35f

void updateGoomba(Enemy *enemy, float dt)
{
    if (enemy->state == ENEMY_STOMPED) {
        enemy->timer -= dt;
        if (enemy->timer <= 0.0f) enemy->active = false;
        return;
    }

    enemy->vel.x = GOOMBA_SPEED * enemy->direction;
    moveEnemy(enemy, dt);
    enemy->timer += dt;
}

void stompGoomba(Enemy *enemy)
{
    enemy->state = ENEMY_STOMPED;
    enemy->timer = STOMP_DURATION;
    enemy->vel = (Vector2){ 0, 0 };
}

void drawGoomba(const Enemy *enemy, float cameraX)
{
    if (enemy->state == ENEMY_STOMPED) {
        DrawTexturePro(textureMap.goombaSquashed, (Rectangle){ 0, 0, 16, 8 },
            (Rectangle){ enemy->pos.x - cameraX, enemy->pos.y + 8 * FACTOR,
                         16 * FACTOR, 8 * FACTOR },
            (Vector2){ 0, 0 }, 0, WHITE);
        return;
    }

    int frame = ((int)(enemy->timer / 0.15f)) % 2;
    DrawTexturePro(textureMap.goomba, (Rectangle){ frame * 16, 0, 16, 16 },
        (Rectangle){ enemy->pos.x - cameraX, enemy->pos.y,
                     16 * FACTOR, 16 * FACTOR },
        (Vector2){ 0, 0 }, 0, WHITE);
}
