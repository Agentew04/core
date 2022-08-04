#ifndef HUD_H
#define HUD_H

#include "entity.h"
#include "timer.h"
#include "allegro5/allegro_font.h"

typedef struct {
    Point p1, p2;
    const char *text;
} Button;

void showFps(ALLEGRO_FONT *font, double tempo, double lastTempo);
void showHud(ALLEGRO_FONT *font, ALLEGRO_DISPLAY *janela, Player *player, Timer *timer, double tempo, double lastTempo);
void showGame(Player *player, Enemy **enemies, int enemyCount, double lastTempo, ALLEGRO_FONT *font);
void showPause(ALLEGRO_FONT *font);
void showMainMenu(ALLEGRO_FONT *font);
void showButton(Button button, ALLEGRO_FONT *font);
void showEnd(ALLEGRO_FONT *font, Player *player, int win);
void handleButtons(int *currentMenu, Point mouse, Player **playerPtr, Enemy **enemies, int enemiesCount);
void showHelp(ALLEGRO_FONT *font);

#endif // HUD_H