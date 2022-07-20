#include <allegro5/allegro_font.h>

typedef struct timer {
    int isPaused;
    ALLEGRO_TIMER *timer;
} Timer;
typedef struct point{
    float x, y;
} Point;
Timer* initTimer();
void startTimer(Timer *timer);
void pauseTimer(Timer *timer);
void resetTimer(Timer *timer);
int getMinutes(Timer *timer);
int getSeconds(Timer *timer);
void showTimer(ALLEGRO_FONT *font, ALLEGRO_DISPLAY *janela, Timer *timer, int show);
void freeTimer(Timer *timer);
void showFps(ALLEGRO_FONT *font, double tempo, double lastTempo, int show);
float getMouseAngle(Point src, Point mouse);
void startAllegro();
void showShield(Point mouse);