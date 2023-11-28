#include "grid.h"
#include <assert.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GRID_MAX_W 1024
#define GRID_MAX_H 1024

#define max(x, y) x > y ? x : y

Grid *NewGrid(int cols, int rows) {
  assert(cols > 0);
  assert(rows > 0);
  char *values = calloc((cols + 1) * rows, sizeof(char));
  assert(values != NULL);
  char **grid = malloc(rows * sizeof(char *));
  assert(grid != NULL);
  for (int i = 0; i < rows; ++i) {
    grid[i] = values + i * (cols + 1);
  }
  Grid *g = malloc(sizeof(Grid));
  assert(g != NULL);
  g->h = rows;
  g->w = cols;
  g->data = grid;
  return g;
}

Grid *ReadGridFromFile(char *filepath) {
  FILE *f = fopen(filepath, "r");
  assert(f != NULL);

  char s[GRID_MAX_W];

  int cols = 0;
  int rows = 0;
  while ((fgets(s, GRID_MAX_W, f)) != NULL) {
    cols = max(cols, strlen(s) - 1);
    rows++;
  }

  int lineLen = cols + 2; // Length with newline and null byte

  rewind(f);

  Grid *g = NewGrid(cols, rows);
  int row = 0;
  while ((fgets(s, lineLen, f)) != NULL) {
    for (int i = 0; i < cols; i++) {
      g->data[row][i] = s[i];
    }
    row++;
  }

  fclose(f);
  return g;
}

Vector2 GridFind(Grid *g, char c) {
  for (int row = 0; row < g->h; row++) {
    for (int col = 0; col < g->w; col++) {
      if (g->data[row][col] == c)
        return (Vector2){col, row};
    }
  }
  return (Vector2){0, 0};
}

void PrintGrid(FILE *f, Grid *g, int spacing) {
  for (int i = 0; i < g->h; i++) {
    for (int j = 0; j < g->w; j++) {
      fprintf(f, "%c", g->data[i][j]);
      for (int x = 0; x < spacing; x++)
        fprintf(f, " ");
    }
    fprintf(f, "\n");
  }
}

void FreeGrid(Grid *grid) {
  free(*grid->data);
  free(grid->data);
  free(grid);
}

Vector2 GridPosToAbs(Vector2 pos, float tilesize) {
  pos.x *= tilesize;
  pos.y *= tilesize;
  return pos;
}
