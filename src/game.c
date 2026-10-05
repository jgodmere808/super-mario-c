
#include "game.h"
#include "items.h"
#include "enemies.h"

#define STARTING_LIVES 3
#define LEVEL_TIME 400.0f
#define HUD_COLUMNS 5
#define FLAG_SLIDE_SPEED (120.0f * FACTOR)
#define CASTLE_TURN_TIME 0.4f
#define BONUS_TICK_TIME (1.0f / 60.0f)

enum GoalPhase {
    GOAL_PLAYING,
    GOAL_SLIDING,
    GOAL_TURNING,
    GOAL_WALKING,
    GOAL_COUNTDOWN,
    GOAL_COMPLETE
};

enum GameSound {
    SOUND_BRICK_SMASH,
    SOUND_JUMP_SMALL,
    SOUND_JUMP_SUPER,
    SOUND_POWERUP,
    SOUND_COIN,
    SOUND_POWERUP_APPEARS,
    SOUND_STOMP,
    SOUND_KICK,
    SOUND_MARIO_DIE,
    SOUND_FLAGPOLE,
    SOUND_STAGE_CLEAR,
    SOUND_COUNT
};

static const char *soundPaths[SOUND_COUNT] = {
    "resources/audio/sfx/smb_breakblock.wav",
    "resources/audio/sfx/smb_jump-small.wav",
    "resources/audio/sfx/smb_jump-super.wav",
    "resources/audio/sfx/smb_powerup.wav",
    "resources/audio/sfx/smb_coin.wav",
    "resources/audio/sfx/smb_powerup_appears.wav",
    "resources/audio/sfx/smb_stomp.wav",
    "resources/audio/sfx/smb_kick.wav",
    "resources/audio/sfx/smb_mariodie.wav",
    "resources/audio/sfx/smb_flagpole.wav",
    "resources/audio/sfx/smb_stage_clear.wav"
};

typedef struct _game {
    Mario mario;
    float cameraX;
    int coins;
    int score;
    int lives;
    float timeRemaining;
    float deathTimer;
    bool gameOver;
    enum GoalPhase goalPhase;
    float goalPhaseTime;
    float flagY;
    float scorePopupTime;
    float flagScoreY;
    float bonusAccumulator;
    int flagScore;
    Music music;
    Sound sounds[SOUND_COUNT];
} Game;

static Game game;

static int flagpolePoints(float grabY, const MapGoal *goal)
{
    float heightFraction = (grabY - goal->topY) / (goal->baseY - goal->topY);
    if (heightFraction < 0.2f) return 5000;
    if (heightFraction < 0.4f) return 2000;
    if (heightFraction < 0.6f) return 800;
    if (heightFraction < 0.8f) return 400;
    return 100;
}

static void beginGoal(const MapGoal *goal)
{
    game.flagScore = flagpolePoints(
        game.mario.pos.y + game.mario.height / 2.0f, goal
    );
    game.score += game.flagScore;
    game.scorePopupTime = 1.5f;
    game.flagScoreY = game.mario.pos.y;
    game.goalPhase = GOAL_SLIDING;
    game.goalPhaseTime = 0.0f;
    game.timeRemaining = ceilf(game.timeRemaining);
    game.mario.pos.x = goal->poleX - game.mario.width;
    if (game.mario.pos.y > goal->baseY - game.mario.height)
        game.mario.pos.y = goal->baseY - game.mario.height;
    game.mario.vel = (Vector2){ 0, 0 };
    game.mario.animation = FLAGPOLE;
    game.mario.frameTimeCounter = 0.0f;
    game.mario.facingLeft = false;
    game.mario.invulnerableTimer = 0.0f;
    StopMusicStream(game.music);
    PlaySound(game.sounds[SOUND_FLAGPOLE]);
}

static void updateGoal(const MapGoal *goal, float dt, float frameTime)
{
    if (game.scorePopupTime > 0.0f) game.scorePopupTime -= frameTime;

    if (game.goalPhase == GOAL_SLIDING) {
        float marioBottomY = goal->baseY - game.mario.height;
        float flagBottomY = goal->baseY - 16 * FACTOR;
        game.mario.pos.y = fminf(game.mario.pos.y + FLAG_SLIDE_SPEED * dt,
                                marioBottomY);
        game.flagY = fminf(game.flagY + FLAG_SLIDE_SPEED * dt, flagBottomY);
        animateMario(&game.mario, dt);
        if (game.mario.pos.y >= marioBottomY && game.flagY >= flagBottomY) {
            game.goalPhase = GOAL_TURNING;
            game.goalPhaseTime = 0.0f;
        }
    } else if (game.goalPhase == GOAL_TURNING) {
        game.goalPhaseTime += dt;
        float progress = fminf(game.goalPhaseTime / CASTLE_TURN_TIME, 1.0f);
        game.mario.pos.x = goal->poleX - game.mario.width +
            progress * (game.mario.width + 3 * FACTOR);
        game.mario.pos.y = goal->baseY - game.mario.height -
            sinf(progress * PI) * 10 * FACTOR;
        game.mario.facingLeft = true;
        if (game.mario.size == SMALL) game.mario.smallFrameRect.x = 13 * 16;
        else game.mario.largeFrameRect.x = 14 * 16;
        if (progress >= 1.0f) {
            game.goalPhase = GOAL_WALKING;
            game.mario.facingLeft = false;
            game.mario.frameTimeCounter = 0.0f;
        }
    } else if (game.goalPhase == GOAL_WALKING) {
        walkMarioToCastle(&game.mario, dt);
        if (game.mario.pos.x + game.mario.width / 2.0f >= goal->castleDoorX) {
            game.mario.pos.x = goal->castleDoorX - game.mario.width / 2.0f;
            game.mario.vel = (Vector2){ 0, 0 };
            game.mario.animation = IDLE;
            animateMario(&game.mario, 0.0f);
            game.goalPhase = GOAL_COUNTDOWN;
            PlaySound(game.sounds[SOUND_STAGE_CLEAR]);
        }
    } else if (game.goalPhase == GOAL_COUNTDOWN) {
        game.bonusAccumulator += frameTime;
        while (game.bonusAccumulator >= BONUS_TICK_TIME &&
               game.timeRemaining > 0.0f) {
            game.timeRemaining -= 1.0f;
            game.score += 50;
            game.bonusAccumulator -= BONUS_TICK_TIME;
        }
        if (game.timeRemaining <= 0.0f) {
            game.timeRemaining = 0.0f;
            game.goalPhase = GOAL_COMPLETE;
        }
    }
}

static void beginMarioDeath(void)
{
    game.lives--;
    game.deathTimer = 1.5f;
    game.mario.animation = DIEING;
    game.mario.smallFrameRect.x = 6 * 16;
    game.mario.vel = (Vector2){ 0, -400.0f };
    StopMusicStream(game.music);
    PlaySound(game.sounds[SOUND_MARIO_DIE]);
}

static void restartLevel(void)
{
    if (!loadMap(MAP_1_1)) {
        game.gameOver = true;
        return;
    }

    game.mario = initMario((Vector2){ 32 * FACTOR, 176 * FACTOR }, SMALL);
    game.cameraX = 0.0f;
    game.timeRemaining = LEVEL_TIME;
    game.deathTimer = 0.0f;
    game.goalPhase = GOAL_PLAYING;
    game.flagY = getMapGoal()->topY + 6 * FACTOR;
    game.scorePopupTime = 0.0f;
    game.bonusAccumulator = 0.0f;
    resetItems();
    resetEnemies();
    PlayMusicStream(game.music);
}

static void unloadGameAudio(void)
{
    if (IsMusicValid(game.music)) UnloadMusicStream(game.music);
    game.music = (Music){ 0 };

    for (int i = 0; i < SOUND_COUNT; i++) {
        if (IsSoundValid(game.sounds[i])) UnloadSound(game.sounds[i]);
        game.sounds[i] = (Sound){ 0 };
    }
}

bool initGame()
{
    unloadGameAudio();

    if (!loadMap(MAP_1_1)) {
        return false;
    }

    game = (Game){
        .mario = initMario((Vector2){ 32 * FACTOR, 176 * FACTOR }, SMALL),
        .lives = STARTING_LIVES,
        .timeRemaining = LEVEL_TIME,
        .flagY = getMapGoal()->topY + 6 * FACTOR
    };
    resetItems();
    resetEnemies();

    game.music = LoadMusicStream(getMapMusicPath());
    if (!IsMusicValid(game.music)) {
        unloadGameAudio();
        return false;
    }

    for (int i = 0; i < SOUND_COUNT; i++) {
        game.sounds[i] = LoadSound(soundPaths[i]);
        if (!IsSoundValid(game.sounds[i])) {
            unloadGameAudio();
            return false;
        }
    }

    game.music.looping = true;
    PlayMusicStream(game.music);

    return true;
}

void endGame()
{
    unloadGameAudio();
}

void updateGame()
{
    if (game.gameOver) {
        if (IsKeyPressed(KEY_ENTER)) initGame();
        return;
    }

    UpdateMusicStream(game.music);

    float frameTime = GetFrameTime();
    float dt = frameTime;
    if (dt > 1.0f / 30.0f) dt = 1.0f / 30.0f;

    if (game.goalPhase != GOAL_PLAYING) {
        if (game.goalPhase != GOAL_COMPLETE)
            updateGoal(getMapGoal(), dt, frameTime);
        return;
    }

    if (game.deathTimer > 0.0f) {
        game.mario.pos.y += game.mario.vel.y * dt;
        game.mario.vel.y += GRAVITY * dt;
        game.deathTimer -= dt;
        if (game.deathTimer <= 0.0f) {
            if (game.lives > 0) restartLevel();
            else game.gameOver = true;
        }
        return;
    }

    game.timeRemaining -= frameTime;
    if (game.timeRemaining <= 0.0f) {
        game.timeRemaining = 0.0f;
        beginMarioDeath();
        return;
    }

    float previousMarioBottom = game.mario.pos.y + game.mario.height;
    BlockHit blockHit;
    if (updateMario(&game.mario, &blockHit)) {
        PlaySound(game.sounds[game.mario.size == SMALL
            ? SOUND_JUMP_SMALL : SOUND_JUMP_SUPER]);
    }

    const MapGoal *goal = getMapGoal();
    if (game.mario.vel.x >= 0.0f &&
        game.mario.pos.x + game.mario.width >= goal->poleX - 8 * FACTOR &&
        game.mario.pos.x < goal->poleX &&
        game.mario.pos.y <= goal->baseY &&
        game.mario.pos.y + game.mario.height > goal->topY) {
        beginGoal(goal);
        return;
    }

    if (game.mario.pos.y > SCREEN_HEIGHT) {
        beginMarioDeath();
        return;
    }

    if (blockHit.happened) {
        enum BlockReward reward = hitMapBlock(
            blockHit.row, blockHit.col, game.mario.size == SMALL
        );
        Vector2 blockPos = {
            blockHit.col * 16 * FACTOR,
            (blockHit.row + 1) * 16 * FACTOR
        };

        if (reward == BLOCK_REWARD_COIN) {
            game.coins++;
            game.score += 200;
            spawnItem(ITEM_BOX_COIN, blockPos);
            PlaySound(game.sounds[SOUND_COIN]);
        } else if (reward == BLOCK_REWARD_POWERUP) {
            if (spawnItem(ITEM_MUSHROOM, blockPos)) {
                PlaySound(game.sounds[SOUND_POWERUP_APPEARS]);
            }
        } else if (reward == BLOCK_REWARD_BRICK_SMASH) {
            PlaySound(game.sounds[SOUND_BRICK_SMASH]);
        }
    }

    updateMap();
    ItemPickups pickups = updateItems(
        (Rectangle){
            game.mario.pos.x, game.mario.pos.y,
            game.mario.width, game.mario.height
        },
        dt
    );
    if (pickups.mushroomsCollected > 0) {
        game.score += 1000 * pickups.mushroomsCollected;
        growMario(&game.mario);
        PlaySound(game.sounds[SOUND_POWERUP]);
    }

    float marioCenter = game.mario.pos.x + game.mario.width / 2.0f;
    float cameraTarget = marioCenter - SCREEN_WIDTH / 2.0f;

    // Scroll once Mario reaches the center; never scroll back to the left.
    if (cameraTarget > game.cameraX) {
        game.cameraX = cameraTarget;
    }

    float maxCameraX = getMapWidthPixels() - SCREEN_WIDTH;
    if (maxCameraX < 0.0f) maxCameraX = 0.0f;
    if (game.cameraX > maxCameraX) game.cameraX = maxCameraX;

    EnemyEvents enemyEvents = updateEnemies(
        &game.mario, previousMarioBottom, game.cameraX, dt
    );
    if (enemyEvents.stomped) {
        game.score += 100;
        PlaySound(game.sounds[SOUND_STOMP]);
    }
    if (enemyEvents.kicked) PlaySound(game.sounds[SOUND_KICK]);
    if (enemyEvents.hitMario && hurtMario(&game.mario)) {
        beginMarioDeath();
    }
}

static void drawHudField(int column, const char *label, const char *value)
{
    const int fontSize = 24;
    int centerX = (2 * column + 1) * SCREEN_WIDTH / (2 * HUD_COLUMNS);
    DrawText(label, centerX - MeasureText(label, fontSize) / 2, 0, fontSize, WHITE);
    DrawText(value, centerX - MeasureText(value, fontSize) / 2, 24, fontSize, WHITE);
}

static void drawFlagpole(const MapGoal *goal)
{
    int x = (int)lroundf(goal->poleX - game.cameraX);
    int top = (int)lroundf(goal->topY);
    int base = (int)lroundf(goal->baseY);
    int flagY = (int)lroundf(game.flagY);
    Color green = (Color){ 0, 168, 0, 255 };

    DrawRectangle(x - FACTOR, top, 2 * FACTOR, base - top, green);
    DrawCircle(x, top, 4 * FACTOR, green);
    DrawRectangle(x - 17 * FACTOR, flagY, 16 * FACTOR, 12 * FACTOR, green);
    DrawRectangle(x - 16 * FACTOR, flagY + FACTOR,
                  14 * FACTOR, 10 * FACTOR, WHITE);
    DrawRectangle(x - 10 * FACTOR, flagY + 4 * FACTOR,
                  3 * FACTOR, 4 * FACTOR, green);
}

void drawGame()
{
    drawMap(game.cameraX);
    drawFlagpole(getMapGoal());
    drawItems(game.cameraX);
    drawEnemies(game.cameraX);
    drawMario(&game.mario, game.cameraX);

    if (game.scorePopupTime > 0.0f) {
        const MapGoal *goal = getMapGoal();
        DrawText(TextFormat("+%d", game.flagScore),
                 (int)(goal->poleX - game.cameraX - 42 * FACTOR),
                 (int)fmaxf(game.flagScoreY - 8 * FACTOR, 48), 20, WHITE);
    }

    drawHudField(0, "LIVES", TextFormat("%02d", game.lives));
    drawHudField(1, "COINS", TextFormat("%02d", game.coins));
    drawHudField(2, "SCORE", TextFormat("%06d", game.score));
    drawHudField(3, "LEVEL", "1-1");
    drawHudField(4, "TIME", TextFormat("%03d", (int)ceilf(game.timeRemaining)));

    if (game.goalPhase == GOAL_COMPLETE) {
        const char *message = "LEVEL CLEAR";
        const char *score = TextFormat("FINAL SCORE %06d", game.score);
        DrawText(message, 48, 90, 32, WHITE);
        DrawText(score, 48, 130, 24, WHITE);
    }

    if (game.gameOver) {
        const char *message = "GAME OVER";
        const char *prompt = "PRESS ENTER TO RESTART";
        DrawRectangle(0, SCREEN_HEIGHT / 2 - 48, SCREEN_WIDTH, 96,
                      Fade(BLACK, 0.75f));
        DrawText(message, (SCREEN_WIDTH - MeasureText(message, 32)) / 2,
                 SCREEN_HEIGHT / 2 - 30, 32, WHITE);
        DrawText(prompt, (SCREEN_WIDTH - MeasureText(prompt, 20)) / 2,
                 SCREEN_HEIGHT / 2 + 12, 20, WHITE);
    }
}
