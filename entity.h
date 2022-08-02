#ifndef ENTITY_H
#define ENTITY_H

#include "a.h"
#include "geometry.h"

#define SHIELD_RADIUS 75
#define SHIELD_THICK 3
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
#define BUFFER_SIZE 500

typedef struct projectile{
    Point pos;
    float speed;
    float alpha;
    int type;
    int bouncesLeft;
    int enemiesDestroyed;
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

Player* generatePlayer();
Enemy* generateRandomEnemy();
int checkEnemyShieldCollision(Enemy* enemy, Player *player);
int checkEnemyPlayerCollision(Enemy* enemy, Player *player);
void shoot(Player *player);

#endif 