#ifndef A_H
#define A_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include "geometry.h"

#define SCR_W 1280
#define SCR_H 720

#define SHIELD_RADIUS 75
#define SHIELD_THICK 3
#define BUFFER_SIZE 500
#define INIT_VOLUME 500
#define MAX_VOLUME M_PI*((SHIELD_RADIUS-SHIELD_THICK)*(SHIELD_RADIUS-SHIELD_THICK))

#define BULLET_SPEED 5
#define PROJ_SPEED 5
#define PROJ_RADIUS 5
#define ROCKET 1
#define BULLET 2

#define ENMY_MAX_VOLUME 200
#define ENMY_MIN_VOLUME 100
#define ERR_MARGIN 25
#define ENMY_MAX_SPEED 4
#define ENMY_MIN_SPEED 1

typedef struct player Player;
typedef struct projectile{
    Point pos;
    float speed;
    float alpha;
    float volume;
    int type;
    int bouncesLeft;
    int enemiesDestroyed;
    Player *shooter;
} Projectile;
typedef struct player{
    float alpha;
    float volume;
    int livesRemaining;
    int score;
    int level;
    int rocketAvaiable;
    int shieldAvaiable;
    Projectile *projectile;
    double lastShot;
} Player;

typedef struct enemy{
    Point pos;
    Point relMid;
    float alpha;
    float speed;
    float volume;
    int isAlive;
    int isAlly;
} Enemy;


void init();
void freeEnemies(Enemy **enemies, int count);
void showShield(Player *player);
void showPlayer(Player *player, ALLEGRO_DISPLAY *janela);
void showEnemies(Enemy **enemies, int count);
void showRocketLauncher(Player *plPayer, ALLEGRO_DISPLAY *janela);
void draw_vertical_gradient_rect(float x, float y, float w, float h, ALLEGRO_COLOR top, ALLEGRO_COLOR bottom);
int checkEnemyShieldCollision(Enemy* enemy, Player *player);
int checkEnemyPlayerCollision(Enemy* enemy, Player *player);
Enemy* generateRandomEnemy();
Player* generatePlayer();
Projectile* generateProjectile(Player *player, int isRocket);
void updateEnemies(Enemy **enemies, int nEnemies);
void update(Player *player, Enemy **enemies, int nEnemies);


#endif // A_H