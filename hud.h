#ifndef HUD_H
#define HUD_H

#include "a.h"
#include "timer.h"
#include "allegro5/allegro_font.h"

void showFps(ALLEGRO_FONT *font, double tempo, double lastTempo);
void ShowHud(ALLEGRO_FONT *font, ALLEGRO_DISPLAY *janela, Player *player, Timer *timer, double tempo, double lastTempo);

#endif // HUD_H