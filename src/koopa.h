#pragma once

#include "enemies.h"

void updateKoopa(Enemy *enemy, float dt);
void stompKoopa(Enemy *enemy);
void kickKoopa(Enemy *enemy, float marioCenterX);
void drawKoopa(const Enemy *enemy, float cameraX);
