#ifndef TIMER_H
#define TIMER_H

#include "allegro5/allegro.h"
#include "allegro5/allegro_font.h"

typedef struct timer {
    int isPaused;
    ALLEGRO_TIMER *timer;
} Timer;
Timer* initTimer();
void startTimer(Timer *timer);
void pauseTimer(Timer *timer);
void resetTimer(Timer *timer);
int getMinutes(Timer *timer);
int getSeconds(Timer *timer);
void freeTimer(Timer *timer);
void showTimer(ALLEGRO_FONT *font, ALLEGRO_DISPLAY *janela, Timer *timer);

#endif // TIMER_H