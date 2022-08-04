#include "entity.h"
#include "timer.h"
#include "main.h"
#include <stdio.h>
#include <malloc.h>
#include <math.h>

Player* generatePlayer(){
    Player *p = (Player*)malloc(sizeof(Player));
    if(p==NULL){
        return NULL;
    }
    p->alpha = 0;
    p->volume = INIT_VOLUME;
    p->livesRemaining = 3;
    p->level = 1;
    p->score = 0;
    p->timer = initTimer();
    p->isPaused = 0;

    p->lastShot = 0;
    p->projectile = NULL;
    p->slowUntil = 0;
    
    p->armor.active = 0;
    p->armor.available = 0;
    p->armor.startTime = 0;
    p->armor.duration = 0;
    return p;
}
Enemy* generateRandomEnemy(){
    Enemy* e = (Enemy*)malloc(sizeof(Enemy));
    int border = rand()%4;
    switch (border){
    case 0: 
        e->pos.x = randomInt(0, SCR_W+BUFFER_SIZE);
        e->pos.y = randomInt(-BUFFER_SIZE,0);
        break;
    case 1: 
        e->pos.x = randomInt(SCR_W+BUFFER_SIZE,SCR_W);
        e->pos.y = randomInt(0,SCR_H+BUFFER_SIZE);
        break;
    case 2: 
        e->pos.x = randomInt(-BUFFER_SIZE,SCR_W);
        e->pos.y = randomInt(SCR_H,SCR_H+BUFFER_SIZE);
        break;
    case 3: 
        e->pos.x = randomInt(-BUFFER_SIZE,0);
        e->pos.y = randomInt(-BUFFER_SIZE,SCR_H);
        break;
    }
    e->speed = rand()%(ENMY_MAX_SPEED-ENMY_MIN_SPEED)+ENMY_MIN_SPEED;
    int midX = rand()%((SCR_W/2+ERR_MARGIN)-(SCR_W/2-ERR_MARGIN))+(SCR_W/2-ERR_MARGIN);
    int midY = rand()%((SCR_H/2+ERR_MARGIN)-(SCR_H/2-ERR_MARGIN))+(SCR_H/2-ERR_MARGIN);
    Point mid = { midX, midY };
    e->relMid = mid;
    e->alpha = getAngleBetweenPoints(e->pos, mid);
    e->volume = rand()%(ENMY_MAX_VOLUME-ENMY_MIN_VOLUME)+ENMY_MIN_VOLUME;
    e->isAlive=1;
    e->isAlly = rand()%6==0;
    return e;
}

Projectile* generateProjectile(Player *player, int isRocket){
    Projectile *p = (Projectile*)malloc(sizeof(Projectile));
    if(p==NULL){
        return NULL;
    }
    Point mid = { SCR_W/2 , SCR_H/2 };
    p->pos = mid;
    p->alpha = M_PI+player->alpha;
    p->speed = isRocket ? ROCKET_SPEED : BULLET_SPEED;
    p->type = isRocket ? ROCKET : BULLET;
    p->bouncesLeft = isRocket ? 5 : 0;
    p->enemiesDestroyed = 0;
    player->rocketAvaiable = 0;
    return p;
}

int checkEnemyShieldCollision(Enemy* enemy, Player *player){
    Point mid = { SCR_W/2, SCR_H/2 };
    Arc a = {
        .center = mid,
        .radius = SHIELD_RADIUS,
        .thickness = SHIELD_THICK,
        .startAlpha = M_PI/2+player->alpha,
        .deltaAlpha = M_PI
    };
    Circle c = {
        .center = enemy->pos,
        .radius = getRadiusFromVolume(enemy->volume)
    };
    return checkCircleArcCollision(a, c);
}

int checkEnemyPlayerCollision(Enemy *enemy, Player *player){
    float rPlayer = getRadiusFromVolume(player->volume);
    float rEnemy = getRadiusFromVolume(enemy->volume);
    Point mid = { SCR_W/2 , SCR_H/2 };
    Circle circleEnemy = {
        .center = enemy->pos,
        .radius = rEnemy
    };
    Circle circlePlayer = {
        .center = mid,
        .radius = rPlayer
    };
    if(checkCircleCicleCollision(circleEnemy, circlePlayer)){
        return 1;
    }
    return 0;
}



void showShield(Player *player){
    ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
    float angIni = -M_PI/2-player->alpha;
    float deltaAng = M_PI;
    al_draw_arc(SCR_W/2, SCR_H/2, SHIELD_RADIUS, angIni, deltaAng, white, SHIELD_THICK);
}

void showProjectile(Player *player){
    if(player->projectile==NULL)
        return;
    ALLEGRO_COLOR yellow = al_map_rgb(255,255,0);
    Projectile *proj = player->projectile;
    al_draw_filled_circle(proj->pos.x, proj->pos.y, PROJ_RADIUS, yellow);
}

void showRocketLauncher(Player *player, ALLEGRO_DISPLAY* janela){
    ALLEGRO_COLOR yellow = al_map_rgb(255, 255, 0);
    ALLEGRO_COLOR gray = al_map_rgb(128, 128, 128);
    ALLEGRO_COLOR orange = al_map_rgb(255, 165, 0);
    int bmpSize = 32    ;
    float unit = (float)bmpSize/4;
    al_set_new_bitmap_flags(ALLEGRO_MIPMAP);
    ALLEGRO_BITMAP *rocket = al_create_bitmap(bmpSize, bmpSize);
    al_set_target_bitmap(rocket);
    al_clear_to_color(al_map_rgba(0,0,0,0));
    Point rP1 = { 3*unit, 0 };
    Point rP2 = { 4*unit, 4*unit };

    Point TP1 = { 0, 2*unit };
    Point TP2 = { 3*unit, 0 };
    Point TP3 = { 3*unit, 4*unit };

    Point tP1 = { 0, 2*unit };
    Point tP2 = { 1*unit, 1*unit };
    Point tP3 = { 1*unit, 3*unit };

    al_draw_filled_rectangle(rP1.x, rP1.y, rP2.x, rP2.y, gray);
    al_draw_filled_triangle(TP1.x, TP1.y, TP2.x, TP2.y, TP3.x, TP3.y, orange);
    al_draw_filled_triangle(tP1.x, tP1.y, tP2.x, tP2.y, tP3.x, tP3.y, yellow);
    al_set_target_backbuffer(janela);
    Point rotCenter = { SCR_W/2, SCR_H/2 };
    al_draw_rotated_bitmap(rocket, 
                            SHIELD_RADIUS, bmpSize/2,
                            rotCenter.x, rotCenter.y, 
                            -player->alpha, 0);
    al_destroy_bitmap(rocket);
}

void showArmor(Player *player){
    if(!player->armor.active)
        return;
    float t0 = player->armor.startTime;
    float t1 = t0+player->armor.duration;
    float tx = al_get_time();
    float radiusPerc = 1/(((tx - t0)) / (t1 - t0));
    float radius = radiusPerc * SHIELD_RADIUS;
    if(radius>SHIELD_RADIUS)
        radius = SHIELD_RADIUS;
    player->armor.radius = radius;
    Point mid = { SCR_W/2, SCR_H/2 };
    ALLEGRO_COLOR lightBlue = al_map_rgb(33, 90, 171);
    al_draw_filled_circle(mid.x, mid.y, player->armor.radius, lightBlue);
}

void showPlayer(Player *player, ALLEGRO_DISPLAY *janela){
    if(player->volume<=0 || player->livesRemaining<=0)
        return;
    ALLEGRO_COLOR green = al_map_rgb(0, 255, 0);
    float radius = getRadiusFromVolume(player->volume);
    showArmor(player);
    showShield(player);
    showRocketLauncher(player, janela);
    showProjectile(player);
    al_draw_filled_circle(SCR_W/2, SCR_H/2, radius, green);
}

void showEnemies(Enemy **enemies, int count){
    ALLEGRO_COLOR red = al_map_rgb(255, 0, 0);
    ALLEGRO_COLOR green = al_map_rgb(0, 255, 0);
    for(int i=0; i<count; i++){
        if(!enemies[i]->isAlive)
            continue;

        float radius = getRadiusFromVolume(enemies[i]->volume);
        al_draw_filled_circle(enemies[i]->pos.x, enemies[i]->pos.y, 
                                radius, 
                                enemies[i]->isAlly?green:red);
    }
}

void addScore(int score, Player *player){
    float nextMulti = ceilf(score/50.0f)*50.0;
    if(player->score < nextMulti && player->score + score >= nextMulti){
        player->rocketAvaiable = 1;
    }
    player->score += score;

    if(player->score > getLevelXp(player->level)){
        player->level++;
        player->canPause=1;
        if(player->level%10 == 0)
            player->livesRemaining++;
        if(player->level%3 == 0)
            player->armor.available = 1;
    }
}

int getLevelXp(int level){
    float log = log2(level) / log2(1.5);
    float result = log + (5*level) + 5;
    return floor(result);
}
float getSpeedIncrease(int level){
    float result = atanf(level)/2 +  level/32 + 0.1;
    return result;
}

int randomInt(int min, int max){
    return rand() % (max - min + 1) + min;
}

float randomFloat(float min, float max){
    float scale = rand() / (float) RAND_MAX;
    return min + scale * ( max - min );
}

void shoot(Player *player){
    double now = al_get_time();
    if(now - player->lastShot < 1 || player->projectile!=NULL || player->livesRemaining<=0){
        return;
    }
    player->projectile = generateProjectile(player, player->rocketAvaiable);
    player->lastShot = now;
    if(player->rocketAvaiable)
        player->rocketAvaiable=0;
}

void togglePause(Player *player){
    if(!player->livesRemaining==0 && player->volume<=0)
        return;

    if(!player->isPaused && player->canPause){
        player->canPause = 0;
        player->isPaused = 1;
        pauseTimer(player->timer);
    }else if(player->isPaused){
        player->isPaused = 0;
        resumeTimer(player->timer);
    }
}

void activateArmor(Player *player){
    if(player->armor.active || !player->armor.available){
        return;
    }
    player->armor.active = 1;
    player->armor.available = 0;
    player->armor.startTime = al_get_time();
    player->armor.duration = randomFloat(5.0f, 10.0f);
}

void deactivateArmor(Player *player){
    player->armor.active = 0;
    player->armor.startTime = 0;
    player->armor.duration = 0;
}