#include "colors.h"
#include "grid.h"
#include "structs.h"
#include "timer.h"
#include "utils.h"

#include <raylib.h>
#include <raymath.h>

// Constants
#define TILE_SIZE 48
#define POINT_SIZE 8
#define GRID_W 24
#define GRID_H 14
#define SCREEN_W 1280
#define SCREEN_H 720
#define H_LANE_START 5
#define H_LANE_END 9
#define V_LANE_START 10
#define V_LANE_END 14
#define PLAYER_START_X 11
#define PLAYER_START_Y 6
#define PLAYER_SPEED 15
#define ENEMY_SPEED 30
#define INPUT_BUF_LEN 16
#define ENEMY_BUF_LEN 255

const int DIRS[4] = {KEY_RIGHT, KEY_LEFT, KEY_DOWN, KEY_UP};
const Vector2 DIR_VECS[4] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
const Triangle DIR_ARROW_POINTS[4] = {{{1.3, 0.5}, {1.1, 0.3}, {1.1, 0.7}},
                                      {{-0.3, 0.5}, {-0.1, 0.7}, {-0.1, 0.3}},
                                      {{0.5, 1.3}, {0.7, 1.1}, {0.3, 1.1}},
                                      {{0.5, -0.3}, {0.3, -0.1}, {0.7, -0.1}}};

Color COLORS[6] = {BLUE, PINK, PURPLE, ORANGE, GREEN, RED};

// --- GLOBALS ---
int inputBuffer[INPUT_BUF_LEN];
Timer spawnTimer;
Grid *g = NULL;
int gridAbsStartX = -1, gridAbsStartY = -1, gridAbsW = 0, gridAbsH = 0;

Enemy enemies[ENEMY_BUF_LEN];
Player player;
int enemyCount = 0, score = 0, wave = 0;

// --- MAIN ---
void init();
void draw();
void update();
void spawnEnemy();

int main() {

  // INITIALIZATION
  InitWindow(SCREEN_W, SCREEN_H, "Dice-Dash");

  init();
  StartTimer(&spawnTimer, 3.0);

  SetTargetFPS(60);
  // LoadAssets();

  while (!WindowShouldClose()) {
    update();
    draw();
  }

  // DE-INITIALIZATION
  FreeGrid(g);
  CloseWindow();

  return 0;
}

void init() {

  player.dir = KEY_LEFT;
  player.color = COLORS[GetRandomValue(0, 5)];
  player.pos = (Vector2){PLAYER_START_X, PLAYER_START_Y};
  player.nextPos = (Vector2){PLAYER_START_X, PLAYER_START_Y};

  g = NewGrid(GRID_W, GRID_H);
  gridAbsW = TILE_SIZE * GRID_W;
  gridAbsH = TILE_SIZE * GRID_H;
  gridAbsStartX = (SCREEN_W - gridAbsW) / 2;
  gridAbsStartY = (SCREEN_H - gridAbsH) / 2;

  for (int row = 0; row < g->h; row++) {
    for (int col = 0; col < g->w; col++) {
      int onHorizontalLane = row >= H_LANE_START && row <= H_LANE_END;
      int onVerticalLane = col >= V_LANE_START && col <= V_LANE_END;
      if (onHorizontalLane && onVerticalLane) {
        g->data[row][col] = '_';
      } else if (onHorizontalLane || onVerticalLane) {
        g->data[row][col] = '.';
      } else {
        g->data[row][col] = ' ';
      }
    }
  }
}

// --- UPDATING ---

int isDirKey(KeyboardKey key) { return (key >= KEY_RIGHT && key <= KEY_UP); }
int isValidKey(KeyboardKey key) { return isDirKey(key) || key == KEY_SPACE; }

int updateInputBuffer() {
  int j = 0;
  int tempBuffer[INPUT_BUF_LEN] = {0};
  // Remove keys not currently pressed
  for (int i = 0; i < INPUT_BUF_LEN; i++) {
    KeyboardKey bufferKey = inputBuffer[i];
    if (isValidKey(bufferKey) && !IsKeyUp(bufferKey)) {
      tempBuffer[j] = bufferKey;
      j++;
    }
  }

  for (int i = 0; i < INPUT_BUF_LEN; i++)
    inputBuffer[i] = tempBuffer[i];

  KeyboardKey pressedKey = GetKeyPressed();
  if (isValidKey(pressedKey)) {
    inputBuffer[j] = pressedKey;
    j++;
  }
  return j;
}

int isInbounds(Vector2 pos) {
  int onHorizontalLane = pos.y >= H_LANE_START && pos.y < H_LANE_END;
  int onVerticalLane = pos.x >= V_LANE_START && pos.x < V_LANE_END;
  return onHorizontalLane && onVerticalLane;
}

Vector2 getDirVec(KeyboardKey key) {
  for (int i = 0; i < 4; i++) {
    if (key == DIRS[i])
      return DIR_VECS[i];
  }
  return (Vector2){0, 0};
}

Vector2 getRandomStartPos() {
  if (GetRandomValue(0, 1)) {
    int x = GetRandomValue(0, 1) ? GRID_W - 1 : 0;
    int y = GetRandomValue(H_LANE_START, H_LANE_END - 1);
    return (Vector2){x, y};
  }
  int x = GetRandomValue(V_LANE_START, V_LANE_END - 1);
  int y = GetRandomValue(0, 1) ? GRID_H - 1 : 0;
  return (Vector2){x, y};
}

void spawnEnemy() {
  Vector2 pos = getRandomStartPos();
  Vector2 nextPos = {pos.x, pos.y};
  int deltaX = 0, deltaY = 0;
  if (pos.x < V_LANE_START)
    deltaX = -1;
  else if (pos.x > V_LANE_END)
    deltaX = 1;
  else if (pos.y < H_LANE_START)
    deltaY = -1;
  else if (pos.y > H_LANE_END)
    deltaY = 1;

  pos.x += deltaX * 3;
  pos.y += deltaY * 3;

  Enemy newEnemy = {
      .color = COLORS[GetRandomValue(0, 5)], .pos = pos, .nextPos = nextPos};

  // Move other enemies in lane forward
  for (int i = 0; i < enemyCount; i++) {
    Enemy *e = &enemies[i];
    int inLane = (deltaX < 0 && pos.y == e->pos.y && e->pos.x < V_LANE_START) ||
                 (deltaX > 0 && pos.y == e->pos.y && e->pos.x > V_LANE_END) ||
                 (deltaY < 0 && pos.x == e->pos.x && e->pos.y < H_LANE_START) ||
                 (deltaY > 0 && pos.x == e->pos.x && e->pos.y > H_LANE_END);

    if (inLane) {
      e->nextPos.x -= deltaX;
      e->nextPos.y -= deltaY;
    }
  }

  enemies[enemyCount] = newEnemy;
  enemyCount++;
}

void update() {
  float deltaTime = GetFrameTime();

  if (TimerDone(spawnTimer)) {
    spawnEnemy();
    StartTimer(&spawnTimer, 3.0);
  }

  for (int i = 0; i < enemyCount; i++) {
    Enemy *e = &enemies[i];
    e->pos = Vector2MoveTowards(e->pos, e->nextPos, ENEMY_SPEED * deltaTime);
  }

  int inputBufferLen = updateInputBuffer();
  if (Vector2Equals(player.pos, player.nextPos)) {
    KeyboardKey key = inputBufferLen > 0 ? inputBuffer[inputBufferLen - 1] : -1;
    if (isDirKey(key)) {
      Vector2 dirDelta = getDirVec(key);
      Vector2 newPos = Vector2Add(player.pos, dirDelta);
      if (isInbounds(newPos))
        player.nextPos = newPos;
      player.dir = key;
    } else {
    }
  } else {
    player.pos = Vector2MoveTowards(player.pos, player.nextPos,
                                    PLAYER_SPEED * deltaTime);
  }
}

void drawBackground() {
  ClearBackground(BLACK);
  for (int row = 0; row < g->h; row++) {
    for (int col = 0; col < g->w; col++) {
      int onHorizontalLane = row >= H_LANE_START && row < H_LANE_END;
      int onVerticalLane = col >= V_LANE_START && col < V_LANE_END;

      int x1 = gridAbsStartX + TILE_SIZE * col;
      int y1 = gridAbsStartY + TILE_SIZE * row;

      if (onHorizontalLane && onVerticalLane) {
        DrawRectangle(x1, y1, TILE_SIZE, TILE_SIZE, DD_LIGHT_GRAY);
        DrawCircle(x1 + TILE_SIZE / 2, y1 + TILE_SIZE / 2, POINT_SIZE, DD_GRAY);
      } else if (onHorizontalLane || onVerticalLane) {
        DrawCircle(x1 + TILE_SIZE / 2, y1 + TILE_SIZE / 2, POINT_SIZE, DD_GOLD);
      } else {
        DrawCircle(x1 + TILE_SIZE / 2, y1 + TILE_SIZE / 2, POINT_SIZE,
                   DD_DARK_GRAY);
      }
    }
  }
}

// --- DRAWING ---

Triangle getArrowPoints(KeyboardKey dir) {
  for (int i = 0; i < 4; i++) {
    if (dir == DIRS[i])
      return DIR_ARROW_POINTS[i];
  }
  return (Triangle){{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}};
}

Vector2 getAbsPos(Vector2 pos) {
  return Vector2Add(Vector2Scale(pos, TILE_SIZE),
                    (Vector2){gridAbsStartX, gridAbsStartY});
}

void drawPlayer() {
  Vector2 playerAbsPos = getAbsPos(player.pos);
  DrawRectangle(playerAbsPos.x, playerAbsPos.y, TILE_SIZE, TILE_SIZE,
                player.color);
  // Draw player direction arrow
  Triangle arrow = getArrowPoints(player.dir);
  Vector2 a = Vector2Add(Vector2Scale(arrow.a, TILE_SIZE), playerAbsPos);
  Vector2 b = Vector2Add(Vector2Scale(arrow.b, TILE_SIZE), playerAbsPos);
  Vector2 c = Vector2Add(Vector2Scale(arrow.c, TILE_SIZE), playerAbsPos);
  DrawTriangle(a, b, c, RED);
}

void drawEnemies() {
  for (int i = 0; i < enemyCount; i++) {
    Enemy e = enemies[i];
    Vector2 enemyAbsPos = getAbsPos(e.pos);
    DrawRectangle(enemyAbsPos.x, enemyAbsPos.y, TILE_SIZE, TILE_SIZE, e.color);
  }
}

void draw() {
  BeginDrawing();

  drawBackground();
  drawPlayer();
  drawEnemies();
  DrawText(TextFormat("FPS: %d", GetFPS()), 4, 4, 16, WHITE);

  EndDrawing();
}
