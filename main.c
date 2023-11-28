#include "grid.h"

#include <raylib.h>
#include <stdio.h>

#define TILE_SIZE 48
#define GRID_W 24
#define GRID_H 14
#define SCREEN_W 1280
#define SCREEN_H 720
#define H_LANE_START 5
#define H_LANE_END 9
#define V_LANE_START 10
#define V_LANE_END 14

int IsInbounds(int row, int col) {
  int onHorizontalLane = row >= H_LANE_START && row < H_LANE_END;
  int onVerticalLane = col >= V_LANE_START && col < V_LANE_END;
  return onHorizontalLane && onVerticalLane;
}

int main() {

  // INITIALIZATION
  Grid *g = NewGrid(GRID_W, GRID_H);

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

  InitWindow(SCREEN_W, SCREEN_H, "Dice-Dash");

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    // float deltaTime = GetFrameTime();

    // DRAW
    BeginDrawing();

    ClearBackground(BLACK);

    // DrawText("Use mouse wheel to zoom", 20, 20, 10, GRAY);
    // DrawText(TextFormat("Tilesize size: %d", tilesize), 20, screenHeight -
    // 50,
    //          10, WHITE);

    int gridAbsW = TILE_SIZE * GRID_W;
    int gridAbsH = TILE_SIZE * GRID_H;

    int startX = (SCREEN_W - gridAbsW) / 2;
    int startY = (SCREEN_H - gridAbsH) / 2;

    DrawRectangle(startX, startY, gridAbsW, gridAbsH, WHITE);

    EndDrawing();
    // ----------
  }

  // LoadAssets();

  // PrintGrid(stdout, g, 1);

  // DE-INITIALIZATION

  FreeGrid(g);
  CloseWindow();

  return 0;
}
