#include <raylib.h>
typedef struct {
  Color color;
  Vector2 pos;
  Vector2 nextPos;
  KeyboardKey dir;
} Player;

typedef struct {
  Color color;
  Vector2 pos;
  Vector2 nextPos;
} Enemy;

typedef struct {
  Vector2 a;
  Vector2 b;
  Vector2 c;
} Triangle;
