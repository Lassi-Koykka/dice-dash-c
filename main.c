#include "grid.h"

#include <raylib.h>
#include <stdio.h>

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

// Colors
// #D3D3D3
#define DD_LIGHT_GRAY                                                          \
  (Color) { 0xD3, 0xD3, 0xD3, 0xff }
// #949494
#define DD_GRAY                                                                \
  (Color) { 0x94, 0x94, 0x94, 0xff }
// #646400
#define DD_GOLD                                                                \
  (Color) { 0x64, 0x64, 0x00, 0xff }
// #141414
#define DD_DARK_GRAY                                                           \
  (Color) { 0x14, 0x14, 0x14, 0xff }

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

    for (int row = 0; row < g->h; row++) {
      for (int col = 0; col < g->w; col++) {
        int onHorizontalLane = row >= H_LANE_START && row < H_LANE_END;
        int onVerticalLane = col >= V_LANE_START && col < V_LANE_END;

        int x1 = startX + TILE_SIZE * col;
        int y1 = startY + TILE_SIZE * row;

        if (onHorizontalLane && onVerticalLane) {
          DrawRectangle(x1, y1, TILE_SIZE, TILE_SIZE, DD_LIGHT_GRAY);
          DrawCircle(x1 + TILE_SIZE / 2, y1 + TILE_SIZE / 2, POINT_SIZE,
                     DD_GRAY);
        } else if (onHorizontalLane || onVerticalLane) {
          DrawCircle(x1 + TILE_SIZE / 2, y1 + TILE_SIZE / 2, POINT_SIZE,
                     DD_GOLD);
        } else {
          DrawCircle(x1 + TILE_SIZE / 2, y1 + TILE_SIZE / 2, POINT_SIZE,
                     DD_DARK_GRAY);
        }
      }
    }

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
