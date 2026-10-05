#pragma once

#include "enemies.h"

void updateGoomba(Enemy *enemy, float dt);
void stompGoomba(Enemy *enemy);
void drawGoomba(const Enemy *enemy, float cameraX);
