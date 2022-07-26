#include "hud.h"
#include "a.h"
#include "timer.h"

void showFps(ALLEGRO_FONT *font, double tempo, double lastTempo){

    double fps  = 1/(tempo-lastTempo);
    al_draw_textf(font, 
        al_map_rgb(255,255,255),
        0,0,
        ALLEGRO_ALIGN_LEFT,
        "FPS: %.1f", fps);
}
void ShowHud(ALLEGRO_FONT *font, ALLEGRO_DISPLAY *janela, Player *player, Timer *timer, double tempo, double lastTempo){
    // draw timer and fps
    showFps(font, tempo, lastTempo);
    showTimer(font, janela, timer);

    // current level
    ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
    al_draw_textf(font, 
        white,
        al_get_display_width(janela)/2, 0,
        ALLEGRO_ALIGN_CENTER,
        "Level: %d", player->level);
}