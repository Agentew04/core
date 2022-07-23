#include <allegro5/allegro_font.h>
#include <math.h>

#define SCR_W 1280
#define SCR_H 720
#define PI 3.14
#define SHIELD_RADIUS 75
#define BULLET_SPEED 5
#define BUFFER_SIZE 500
#define MAX_SPEED 4
#define MIN_SPEED 1
#define MAX_VOLUME 200
#define MIN_VOLUME 100
#define ERR_MARGIN 50

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
    Point relMid;
    float alpha;
    float speed;
    float volume;
} Enemy;

Timer* initTimer();
void init();

void startTimer(Timer *timer);
void pauseTimer(Timer *timer);
void resetTimer(Timer *timer);
int getMinutes(Timer *timer);
int getSeconds(Timer *timer);

void freeTimer(Timer *timer);
void freeEnemies(Enemy **enemies, int count);

void showTimer(ALLEGRO_FONT *font, ALLEGRO_DISPLAY *janela, Timer *timer, int show);
void showFps(ALLEGRO_FONT *font, double tempo, double lastTempo, int show);
void showShield(Player *player);
void showPlayer(Player *player);
void showEnemies(Enemy **enemies, int count);

float getAngleBetweenPoints(Point p1, Point p2);
float getRadiusFromVolume(float volume);
int checkEnemyShieldCollision(Enemy* enemy, Point mouse);
float getDistance(Point p1, Point p2);
int checkCircleArcCollision(Point arcCenter, Point mouse, Point circleCenter, float arcRadius, float circleRadius);
int checkCircleCicleCollision(Point c1, float r1, Point c2, float r2);

Enemy* generateRandomEnemy();
void updateEnemies(Enemy **enemies, int nEnemies);

// geometria
float getRadiusFromVolume(float volume){
    // r = raiz(area/pi)
    return sqrt(volume/PI);
}
float getAngleBetweenPoints(Point p1, Point p2){
    double rad = atan2(p1.y-p2.y, p2.x - p1.x);
    return rad;
}
int checkCircleArcCollision(Point arcCenter, Point mouse, Point circleCenter, float arcRadius, float circleRadius){
    float alphaArc = getAngleBetweenPoints(arcCenter, mouse);
    float alphaCircle = getAngleBetweenPoints(arcCenter, circleCenter);
    float distArcCircle = getDistance(arcCenter, circleCenter)-arcRadius-circleRadius;
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

// timer
Timer* initTimer(){
    ALLEGRO_TIMER *alTimer = al_create_timer(1);
    Timer *myTimer = malloc(sizeof(Timer));
    myTimer->isPaused=1;
    myTimer->timer=alTimer;
    return myTimer;
}

void startTimer(Timer *timer){
    if(timer->isPaused){
        al_start_timer(timer->timer);
        timer->isPaused=0;
    }
}

void pauseTimer(Timer *timer){
    if(!timer->isPaused){
        al_stop_timer(timer->timer);
        timer->isPaused=1;
    }
}

void resetTimer(Timer *timer){
    al_destroy_timer(timer->timer);
    timer->isPaused=1;
    timer->timer = al_create_timer(1);
}

int getMinutes(Timer *timer){
    return al_get_timer_count(timer->timer)/60;
}

int getSeconds(Timer *timer){
    return al_get_timer_count(timer->timer)%60;
}

void showTimer(ALLEGRO_FONT *font, ALLEGRO_DISPLAY *janela, Timer *timer, int show){
    if(!show)
        return;
    ALLEGRO_COLOR white = al_map_rgb(255,255,255);
    al_draw_textf(font, 
        white, 
        al_get_display_width(janela),
        0,
        ALLEGRO_ALIGN_RIGHT,
        "%02d:%02d", getMinutes(timer), getSeconds(timer));
}

void freeTimer(Timer *timer){
    al_destroy_timer(timer->timer);
    free(timer);
}
// end timer
