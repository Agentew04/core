#include "help.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <stdlib.h>

void showFps(double tempo, double lastTempo){
    double fps  = 1/(tempo-lastTempo);
    ALLEGRO_FONT *font=al_create_builtin_font();
    al_draw_textf(font, 
        al_map_rgb(255,255,255),
        0,0,
        ALLEGRO_ALIGN_LEFT,
        "FPS: %.1f", fps);
    free(font);
}