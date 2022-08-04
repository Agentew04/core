#ifndef ENTITY_H
#define ENTITY_H

#include "geometry.h"
#include "timer.h"

#define SHIELD_RADIUS 75
#define SHIELD_THICK 6
#define INIT_VOLUME 500
#define MAX_VOLUME M_PI*((SHIELD_RADIUS-SHIELD_THICK)*(SHIELD_RADIUS-SHIELD_THICK))

#define BULLET_SPEED 5
#define ROCKET_SPEED 3
#define PROJ_RADIUS 10
#define ROCKET 1
#define BULLET 2
#define SLOW_TIME 5

#define ENMY_MAX_VOLUME 200
#define ENMY_MIN_VOLUME 100
#define ERR_MARGIN 25
#define ENMY_MAX_SPEED 4
#define ENMY_MIN_SPEED 1
#define BUFFER_SIZE 500


typedef struct armor{
    int available;
    int active;
    double startTime;
    double duration;
    double radius;
    double lastHit;
} Armor;
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
    Timer *timer;

    int rocketAvaiable;
    Projectile *projectile;
    double lastShot;
    double slowUntil;

    Armor armor;

    int canPause;
    int isPaused;
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
void addScore(int score, Player *player);
int getLevelXp(int level);
float getSpeedIncrease(int level);
void togglePause(Player *player);
void activateArmor(Player *player);
void deactivateArmor(Player *player);

int randomInt(int min, int max);
float randomFloat(float min, float max);

void showArmor(Player *player);
void showShield(Player *player);
void showPlayer(Player *player, ALLEGRO_DISPLAY *janela);
void showEnemies(Enemy **enemies, int count);
void showRocketLauncher(Player *plPayer, ALLEGRO_DISPLAY *janela);
void showProjectile(Player *player);

#endif 