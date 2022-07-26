#include "a.h"

void draw_vertical_gradient_rect(float x, float y, float w, float h, ALLEGRO_COLOR top, ALLEGRO_COLOR bottom) {
    ALLEGRO_VERTEX v[] = {
        {.x=x    , .y=y    , .z=0, .color=top},
        {.x=x + w, .y=y    , .z=0, .color=top},
        {.x=x    , .y=y + h, .z=0, .color=bottom},
        {.x=x + w, .y=y + h, .z=0, .color=bottom}};
    al_draw_prim(v, NULL, NULL, 0, 4, ALLEGRO_PRIM_TRIANGLE_STRIP);
}