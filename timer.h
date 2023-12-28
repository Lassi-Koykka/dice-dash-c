#ifndef INCLUDE_TIMER
#define INCLUDE_TIMER

#include <raylib.h>
typedef struct Timer {
  double startTime; // Start time (seconds)
  double lifeTime;  // Lifetime (seconds)
} Timer;

static inline void StartTimer(Timer *timer, double lifetime) {
  timer->startTime = GetTime();
  timer->lifeTime = lifetime;
}

static inline int TimerDone(Timer timer) {
  return GetTime() - timer.startTime >= timer.lifeTime;
}

static inline double GetElapsed(Timer timer) {
  return GetTime() - timer.startTime;
}

#endif
