#include <assert.h>
#include <raylib.h>
#include <stdio.h>

typedef struct {
  int w;
  int h;
  char **data;
} Grid;

Grid *NewGrid(int cols, int rows);

void PrintGrid(FILE *f, Grid *g, int spacing);

Vector2 GridFind(Grid *g, char c);

void FreeGrid(Grid *grid);

Grid *ReadGridFromFile(char *filepath);

Vector2 GridPosToAbs(Vector2 pos, float tilesize);
