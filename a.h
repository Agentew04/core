#include <allegro5/allegro_font.h>
#include <math.h>

#define SCR_W 1280
#define SCR_H 720
#define PI 3.14
#define SHIELD_RADIUS 75
#define BULLET_SPEED 5
#define BUFFER_SIZE 500
#define MAX_SPEED 50
#define MIN_SPEED 49
#define MAX_VOLUME 200
#define MIN_VOLUME 150

typedef struct timer {
    int isPaused;
    ALLEGRO_TIMER *timer;
} Timer;
typedef struct point{
    float x, y;
} Point;
typedef struct player{
    float alpha;
    float volume;
    int vidas;
    int score;
    int rocketAvaiable;
    int shieldAvaiable;
    int canShootRocket;
    double lastShot;
} Player;
typedef struct rocket{
    float alpha;
    int bouncesLeft;
    int enemiesDestroyed;
    Player *shooter;
} Rocket;
typedef struct bullet{
    float alpha;
    Player *shooter;
} Bullet;
typedef struct enemy{
    Point pos;
    float alpha;
    float speed;
    float volume;
} Enemy;

Timer* initTimer();
void startTimer(Timer *timer);
void pauseTimer(Timer *timer);
void resetTimer(Timer *timer);
int getMinutes(Timer *timer);
int getSeconds(Timer *timer);
void showTimer(ALLEGRO_FONT *font, ALLEGRO_DISPLAY *janela, Timer *timer, int show);
void freeTimer(Timer *timer);
void showFps(ALLEGRO_FONT *font, double tempo, double lastTempo, int show);
float getAngleBetweenPoints(Point p1, Point p2);
void startAllegro();
void showShield(Player *player);
void showPlayer(Player *player);
void showEnemy(Enemy *enemy);
float getRadiusFromVolume(float volume);
int checkEnemyShieldCollision(Enemy* enemy, Point mouse);
float getDistance(Point p1, Point p2);
int checkCircleArcCollision(Point arcCenter, Point mouse, Point circleCenter, float circleRadius);
int checkCircleCicleCollision(Point c1, float r1, Point c2, float r2);
Enemy* generateRandomEnemy();
void updateEnemies(Enemy *enemies, int nEnemies);

// geometria
float getRadiusFromVolume(float volume){
    // r = raiz(area/pi)
    return sqrt(volume/PI);
}
int checkCircleArcCollision(Point arcCenter, Point mouse, Point circleCenter, float circleRadius){
    float alphaArc = getAngleBetweenPoints(arcCenter, mouse);
    float alphaCircle = getAngleBetweenPoints(arcCenter, circleCenter);
    float distArcCircle = getDistance(arcCenter, circleCenter)-SHIELD_RADIUS;
    float arcRadius = SHIELD_RADIUS;
    if(distArcCircle<0.1 && alphaCircle<alphaArc+PI/2 && alphaCircle>alphaArc-PI/2)
        return 1;
    return 0;
}
int checkCircleCicleCollision(Point c1, float r1, Point c2, float r2){
    float dist = getDistance(c1, c2);
    if(dist<r1+r2)
        return 1;
    return 0;
}
float getDistance(Point p1, Point p2){
    return sqrt(pow(p1.x-p2.x, 2)+pow(p1.y-p2.y, 2));
}