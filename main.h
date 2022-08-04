#ifndef A_H
#define A_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>

#include "geometry.h"
#include "entity.h"


#define SCR_W 1280
#define SCR_H 720

#define MENU_MAIN 0
#define MENU_HELP 1
#define MENU_GAME 2
#define MENU_END 3
#define MENU_WIN 4

void init();
void draw_vertical_gradient_rect(float x, float y, float w, float h, ALLEGRO_COLOR top, ALLEGRO_COLOR bottom);
void freeEnemies(Enemy **enemies, int count);
void updateMovement(Enemy **enemies, int nEnemies, Player *player);
void updateCollision(Enemy **enemies, int nEnemies, Player *player);
void updateOther(Player *player);
void respawnEnemies(Enemy **enemies, int nEnemies);

#endif // A_H