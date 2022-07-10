#include "fps.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <stdlib.h>

void showFps(ALLEGRO_FONT *font, double tempo, double lastTempo, int show){
    if(!show)
        return;

    double fps  = 1/(tempo-lastTempo);
    al_draw_textf(font, 
        al_map_rgb(255,255,255),
        0,0,
        ALLEGRO_ALIGN_LEFT,
        "FPS: %.1f", fps);
}