
#include "mario.h"

#define JUMP_VELOCITY -650.0f
#define MARIO_TOP_SPEED 250.0f

#define TILE_SIZE (16 * FACTOR)

/*
 * Mario's position and dimensions are in screen pixels.
 * Map column 0 starts at x = 0.
 * Map row 0 starts at y = TILE_SIZE (see drawMap()).
 */
static int columnAt(float x)
{
    return (int)floorf(x / TILE_SIZE);
}

static int rowAt(float y)
{
    return (int)floorf(y / TILE_SIZE) - 1;
}

static void moveMarioHorizontally(Mario *mario, float dt)
{
    int row, col, topRow, bottomRow;
    float leadingX;
    float distance = mario->vel.x * dt;
    if (distance == 0.0f) return;

    mario->pos.x += distance;

    // Check the column at Mario's leading edge.
    leadingX = distance > 0.0f
        ? mario->pos.x + mario->width - 0.001f
        : mario->pos.x;
    
    col = columnAt(leadingX);

    topRow = rowAt(mario->pos.y);
    bottomRow = rowAt(mario->pos.y + mario->height - 0.001f);

    for (row = topRow; row <= bottomRow; row++) {
        if (!isMapSolidAt(row, col)) continue;

        if (distance > 0.0f) {
            // Moving right: put Mario's right edge against the tile.
            mario->pos.x = col * TILE_SIZE - mario->width;
        } else {
            // Moving left: put Mario's left edge against the tile.
            mario->pos.x = (col + 1) * TILE_SIZE;
        }

        mario->vel.x = 0.0f;
        return;
    }
}

static void moveMarioVertically(Mario *mario, float dt, BlockHit *blockHit)
{
    int row, col, leftCol, rightCol;
    float leadingY;
    float distance = mario->vel.y * dt;
    mario->onGround = false;

    if (distance == 0.0f) return;

    mario->pos.y += distance;

    // Check the row at Mario's leading edge
    leadingY = distance > 0.0f
        ? mario->pos.y + mario->height - 0.001f
        : mario->pos.y;
    
    row = rowAt(leadingY);

    leftCol = columnAt(mario->pos.x);
    rightCol = columnAt(mario->pos.x + mario->width - 0.001f);

    for (col = leftCol; col <= rightCol; col++) {
        if (!isMapSolidAt(row, col)) continue;

        if (distance > 0.0f) {
            // Falling: place Mario's feet on the tile's top.
            mario->pos.y = (row + 1) * TILE_SIZE - mario->height;
            mario->onGround = true;
        } else {
            // Rising: place Mario's head below the tile.
            int centerCol = columnAt(mario->pos.x + mario->width / 2);

            blockHit->happened = true;
            blockHit->row = row;
            blockHit->col = isMapSolidAt(row, centerCol) ? centerCol : col;

            mario->pos.y = (row + 2) * TILE_SIZE;
        }

        mario->vel.y = 0.0f;
        return;
    }
}

Mario initMario(Vector2 pos, enum MarioSize size)
{
    Mario mario = {
        .size = size,
        .animation = IDLE,
        .frameTimeCounter = 0,
        .facingLeft = false,
        .onGround = true,
        .pos = pos,
        .vel = { 0, 0 },
        .smallFrameRect = { 0, 0, 16, 16 },
        .largeFrameRect = { 0, 0, 16, 32 },
        .smallMarioTexture = textureMap.marioSmall,
        .largeMarioTexture = textureMap.marioLarge
    };

    switch (size) {
        case SMALL:
            mario.width = 16 * FACTOR;
            mario.height = 16 * FACTOR;
            break;
        case LARGE:
            mario.width = 16 * FACTOR;
            mario.height = 32 * FACTOR;
            break;
    }

    return mario;
}

static void updateSmallAnimation(Mario *mario, float dt)
{
    int frame;
    float animationTime;
    float frameTime;
    mario->frameTimeCounter += dt;

    switch (mario->animation) {
        case IDLE:
            mario->smallFrameRect.x = 0;
            break;
        case RUNNING:
            mario->smallFrameRect.x = 16 * (1 + (int)(mario->frameTimeCounter / 0.15f) % 3);
            break;
        case JUMPING:
            mario->smallFrameRect.x = 5 * 16;
            break;
        case SKIDDING:
            mario->smallFrameRect.x = 4 * 16;
            break;
        case DIEING:
            mario->smallFrameRect.x = 6 * 16;
            break;
        case SWIMMING:
            animationTime = 0.3;
            frameTime = animationTime / 3;
            if (mario->frameTimeCounter >= animationTime) {
                mario->frameTimeCounter = 0;
            }
            frame = mario->frameTimeCounter / frameTime;
            mario->smallFrameRect.x = 16 * (9 - frame);
            break;
        case CLIMBING:
            break;
        case FLAGPOLE:
            mario->smallFrameRect.x = 16 *
                (12 + (int)(mario->frameTimeCounter / 0.15f) % 2);
            break;
    }
}

static void updateLargeAnimation(Mario *mario, float dt)
{
    mario->frameTimeCounter += dt;

    switch (mario->animation) {
        case IDLE:
            mario->largeFrameRect.x = 0;
            break;
        case RUNNING:
            mario->largeFrameRect.x = 16 * (1 + (int)(mario->frameTimeCounter / 0.15f) % 3);
            break;
        case JUMPING:
            mario->largeFrameRect.x = 16 * 5;
            break;
        case FLAGPOLE:
            mario->largeFrameRect.x = 16 *
                (12 + (int)(mario->frameTimeCounter / 0.15f) % 2);
            break;
        case SKIDDING:
            mario->largeFrameRect.x = 16 * 4;
            break;
        default:
            break;
    }
}

void animateMario(Mario *mario, float dt)
{
    if (mario->size == SMALL) updateSmallAnimation(mario, dt);
    else updateLargeAnimation(mario, dt);
}

void walkMarioToCastle(Mario *mario, float dt)
{
    BlockHit ignored = { 0 };
    mario->vel.x = 180.0f;
    mario->vel.y += GRAVITY * dt;
    if (mario->vel.y > 900.0f) mario->vel.y = 900.0f;
    mario->animation = RUNNING;
    moveMarioHorizontally(mario, dt);
    moveMarioVertically(mario, dt, &ignored);
    animateMario(mario, dt);
}

bool updateMario(Mario *mario, BlockHit *blockHit)
{
    float dt = GetFrameTime();
    bool jumped = false;
    if (dt > 1.0f / 30.0f) dt = 1.0f / 30.0f;
    if (mario->invulnerableTimer > 0.0f) {
        mario->invulnerableTimer -= dt;
        if (mario->invulnerableTimer < 0.0f) mario->invulnerableTimer = 0.0f;
    }
    *blockHit = (BlockHit){ 0 };

    if (!IsKeyDown(KEY_LEFT) && IsKeyDown(KEY_RIGHT)) {
        if (mario->vel.x < 0) {
            if (mario->animation != SKIDDING) mario->frameTimeCounter = 0;
            mario->animation = SKIDDING;
        } else {
            if (mario->animation != RUNNING) mario->frameTimeCounter = 0;
            mario->animation = RUNNING;
        }
        mario->facingLeft = false;
        mario->vel.x += 5.0f;
    }
    if (IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT)) {
        if (mario->vel.x > 0) {
            if (mario->animation != SKIDDING) mario->frameTimeCounter = 0;
            mario->animation = SKIDDING;
        } else {
            if (mario->animation != RUNNING) mario->frameTimeCounter = 0;
            mario->animation = RUNNING;
        }
        mario->facingLeft = true;
        mario->vel.x -= 5.0f;
    }

    if (
        (IsKeyDown(KEY_LEFT) && IsKeyDown(KEY_RIGHT)) ||
        (!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT))
    ) {
        if ((mario->vel.x > 0 ? mario->vel.x : -mario->vel.x) > 5.0f) {
            if (mario->animation != RUNNING) mario->frameTimeCounter = 0;
            mario->animation = RUNNING;
            mario->vel.x *= 0.95;
            if ((mario->vel.x > 0 ? mario->vel.x : -mario->vel.x) < 5.0f) {
                mario->vel.x = 0;
            }
        } else {
            if (mario->animation != IDLE) mario->frameTimeCounter = 0;
            mario->animation = IDLE;
        }
    }
    if (mario->onGround && IsKeyDown(KEY_SPACE)) {
        mario->onGround = false;
        mario->vel.y = JUMP_VELOCITY;
        jumped = true;
    }
    if (!mario->onGround) {
        mario->animation = JUMPING;
    }

    mario->vel.y += GRAVITY * dt;
    if (mario->vel.x > MARIO_TOP_SPEED) mario->vel.x = MARIO_TOP_SPEED;
    if (mario->vel.x < -MARIO_TOP_SPEED) mario->vel.x = -MARIO_TOP_SPEED;
    if (mario->vel.y > 900.0f) mario->vel.y = 900.0f;

    moveMarioHorizontally(mario, dt);
    moveMarioVertically(mario, dt, blockHit);

    animateMario(mario, dt);

    return jumped;
}

void growMario(Mario *mario)
{
    if (mario->size == LARGE) return;

    // Keep Mario's feet in place when his height doubles.
    mario->pos.y -= 16 * FACTOR;
    mario->height = 32 * FACTOR;
    mario->size = LARGE;
    mario->largeFrameRect = (Rectangle){ 0, 0, 16, 32 };
    mario->frameTimeCounter = 0.0f;
}

bool hurtMario(Mario *mario)
{
    if (mario->invulnerableTimer > 0.0f) return false;
    if (mario->size == SMALL) return true;

    // Keep Mario's feet fixed as he shrinks.
    mario->pos.y += 16 * FACTOR;
    mario->height = 16 * FACTOR;
    mario->size = SMALL;
    mario->invulnerableTimer = 2.0f;
    mario->frameTimeCounter = 0.0f;
    return false;
}

void drawMario(Mario *mario, float cameraX)
{
    if (mario->invulnerableTimer > 0.0f &&
        ((int)(mario->invulnerableTimer / 0.08f) % 2) == 0) return;
    Rectangle source = mario->size == SMALL
        ? mario->smallFrameRect : mario->largeFrameRect;

    if (mario->facingLeft) {
        source.width = -source.width;
    }
    
    DrawTexturePro(
        mario->size == SMALL ? mario->smallMarioTexture : mario->largeMarioTexture,
        source,
        (Rectangle){ mario->pos.x - cameraX, mario->pos.y, mario->width, mario->height },
        (Vector2){ 0, 0 },
        0.0f,
        WHITE
    );
}
