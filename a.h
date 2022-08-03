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

void init();
void freeEnemies(Enemy **enemies, int count);
void draw_vertical_gradient_rect(float x, float y, float w, float h, ALLEGRO_COLOR top, ALLEGRO_COLOR bottom);

Projectile* generateProjectile(Player *player, int isRocket);

// update functions
void updateMovement(Enemy **enemies, int nEnemies, Player *player);
void updateCollision(Enemy **enemies, int nEnemies, Player *player);


#endif // A_H