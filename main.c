#include "colors.h"
#include "grid.h"

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
#define INPUT_BUF_LEN 16

typedef struct {
  Vector2 pos;
  Vector2 nextPos;
  KeyboardKey dir;
} Player;

typedef struct {
  Vector2 a;
  Vector2 b;
  Vector2 c;
} Triangle;

const int DIRS[4] = {KEY_RIGHT, KEY_LEFT, KEY_DOWN, KEY_UP};
const Vector2 DIR_VECS[4] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
const Triangle DIR_ARROW_POINTS[4] = {{{1.3, 0.5}, {1.1, 0.3}, {1.1, 0.7}},
                                      {{-0.3, 0.5}, {-0.1, 0.7}, {-0.1, 0.3}},
                                      {{0.5, 1.3}, {0.7, 1.1}, {0.3, 1.1}},
                                      {{0.5, -0.3}, {0.3, -0.1}, {0.7, -0.1}}};

// GLOBALS
int inputBuffer[INPUT_BUF_LEN];
Grid *g = NULL;
Player player;
int startX = -1, startY = -1;

int IsInbounds(Vector2 pos);

void init();
void draw();
void update();

int main() {

  // INITIALIZATION
  init();

  InitWindow(SCREEN_W, SCREEN_H, "Dice-Dash");

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

int IsInbounds(Vector2 pos) {
  int onHorizontalLane = pos.y >= H_LANE_START && pos.y < H_LANE_END;
  int onVerticalLane = pos.x >= V_LANE_START && pos.x < V_LANE_END;
  return onHorizontalLane && onVerticalLane;
}

Triangle getArrowPoints(KeyboardKey dir) {
  for (int i = 0; i < 4; i++) {
    if (dir == DIRS[i])
      return DIR_ARROW_POINTS[i];
  }
  return (Triangle){{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}};
}

Vector2 getAbsPos(Vector2 pos) {
  return Vector2Add(Vector2Scale(pos, TILE_SIZE), (Vector2){startX, startY});
}

void init() {
  player.pos = (Vector2){PLAYER_START_X, PLAYER_START_Y};
  player.dir = KEY_LEFT;
  player.nextPos = (Vector2){PLAYER_START_X, PLAYER_START_Y};

  g = NewGrid(GRID_W, GRID_H);
  int gridAbsW = TILE_SIZE * GRID_W;
  int gridAbsH = TILE_SIZE * GRID_H;
  startX = (SCREEN_W - gridAbsW) / 2;
  startY = (SCREEN_H - gridAbsH) / 2;

  for (int row = 0; row < g->h; row++) {
    for (int col = 0; col < g->w; col++) {
      int onHorizontalLane = row >= H_LANE_START && row < H_LANE_END;
      int onVerticalLane = col >= V_LANE_START && col < V_LANE_END;
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

Vector2 getDirVec(KeyboardKey key) {
  for (int i = 0; i < 4; i++) {
    if (key == DIRS[i])
      return DIR_VECS[i];
  }
  return (Vector2){0, 0};
}

int isDirKey(KeyboardKey key) { return (key >= KEY_RIGHT && key <= KEY_UP); }

int updateInputBuffer() {
  int j = 0;
  int tempBuffer[INPUT_BUF_LEN] = {0};

  // Remove keys not currently pressed
  for (int i = 0; i < INPUT_BUF_LEN; i++) {
    KeyboardKey bufferKey = inputBuffer[i];
    if (isDirKey(bufferKey) && !IsKeyUp(bufferKey)) {
      tempBuffer[j] = bufferKey;
      j++;
    }
  }

  for (int i = 0; i < INPUT_BUF_LEN; i++) {
    inputBuffer[i] = tempBuffer[i];
  }

  KeyboardKey pressedKey = GetKeyPressed();
  if (isDirKey(pressedKey)) {
    inputBuffer[j] = pressedKey;
    j++;
  }

  return j;
}

void update() {
  float deltaTime = GetFrameTime();

  int inputBufferLen = updateInputBuffer();

  if (Vector2Equals(player.pos, player.nextPos)) {
    KeyboardKey key = inputBufferLen > 0 ? inputBuffer[inputBufferLen - 1] : -1;
    if (isDirKey(key)) {
      Vector2 dirDelta = getDirVec(key);
      Vector2 newPos = Vector2Add(player.pos, dirDelta);
      if (IsInbounds(newPos))
        player.nextPos = newPos;
      player.dir = key;
    }
  } else {
    player.pos = Vector2MoveTowards(player.pos, player.nextPos, 10 * deltaTime);
  }
}

void draw() {
  BeginDrawing();
  // Draw background
  ClearBackground(BLACK);
  for (int row = 0; row < g->h; row++) {
    for (int col = 0; col < g->w; col++) {
      int onHorizontalLane = row >= H_LANE_START && row < H_LANE_END;
      int onVerticalLane = col >= V_LANE_START && col < V_LANE_END;

      int x1 = startX + TILE_SIZE * col;
      int y1 = startY + TILE_SIZE * row;

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

  // Draw player
  Vector2 playerAbsPos = getAbsPos(player.pos);
  DrawRectangle(playerAbsPos.x, playerAbsPos.y, TILE_SIZE, TILE_SIZE, RED);
  // Draw player direction arrow
  Triangle arrow = getArrowPoints(player.dir);
  Vector2 a = Vector2Add(Vector2Scale(arrow.a, TILE_SIZE), playerAbsPos);
  Vector2 b = Vector2Add(Vector2Scale(arrow.b, TILE_SIZE), playerAbsPos);
  Vector2 c = Vector2Add(Vector2Scale(arrow.c, TILE_SIZE), playerAbsPos);
  DrawTriangle(a, b, c, RED);

  // Draw framerate
  int fps = GetFPS();
  DrawText(TextFormat("FPS: %d", fps), 4, 4, 16, WHITE);

  EndDrawing();
}
