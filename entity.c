#include "entity.h"
#include "timer.h"
#include "main.h"
#include <stdio.h>
#include <malloc.h>
#include <math.h>

Player* generatePlayer(){
    Player *p = (Player*)malloc(sizeof(Player));
    if(p==NULL){
        printf("erro ao alocar player\n");
        return NULL;
    }
    p->alpha = 0;
    p->volume = INIT_VOLUME;
    p->livesRemaining = 3;
    p->level = 1;
    p->score = 0;

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
    // get random point in border of screen
    int border = rand()%4;
    switch (border){
    case 0: // top buffer
        e->pos.x = randomInt(0, SCR_W+BUFFER_SIZE);
        e->pos.y = randomInt(-BUFFER_SIZE,0);
        break;
    case 1: // right buffer
        e->pos.x = randomInt(SCR_W+BUFFER_SIZE,SCR_W);
        e->pos.y = randomInt(0,SCR_H+BUFFER_SIZE);
        break;
    case 2: // bottom buffer
        e->pos.x = randomInt(-BUFFER_SIZE,SCR_W);
        e->pos.y = randomInt(SCR_H,SCR_H+BUFFER_SIZE);
        break;
    case 3: // left buffer
        e->pos.x = randomInt(-BUFFER_SIZE,0);
        e->pos.y = randomInt(-BUFFER_SIZE,SCR_H);
        break;
    }
    // random speed
    e->speed = rand()%(ENMY_MAX_SPEED-ENMY_MIN_SPEED)+ENMY_MIN_SPEED;
    // calculate random center
    int midX = rand()%((SCR_W/2+ERR_MARGIN)-(SCR_W/2-ERR_MARGIN))+(SCR_W/2-ERR_MARGIN);
    int midY = rand()%((SCR_H/2+ERR_MARGIN)-(SCR_H/2-ERR_MARGIN))+(SCR_H/2-ERR_MARGIN);
    Point mid = { midX, midY };
    e->relMid = mid;
    // calculate alpha to center
    e->alpha = getAngleBetweenPoints(e->pos, mid);
    e->volume = rand()%(ENMY_MAX_VOLUME-ENMY_MIN_VOLUME)+ENMY_MIN_VOLUME;
    e->isAlive=1;
    // 1 in 6 is ally
    e->isAlly = rand()%6==0;
    return e;
}

Projectile* generateProjectile(Player *player, int isRocket){
    Projectile *p = (Projectile*)malloc(sizeof(Projectile));
    if(p==NULL){
        printf("erro ao alocar projetil\n");
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
    int bmpSize = 32;
    ALLEGRO_BITMAP *rocket = al_create_bitmap(bmpSize, bmpSize);
    al_set_target_bitmap(rocket);
    Point p1 = { 0, bmpSize/2 };
    Point p2 = { bmpSize, 0 };
    Point p3 = { bmpSize, bmpSize };
    al_draw_filled_triangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, yellow);
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
    // update armor radius
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
    float nextMulti = ceilf(score/50.0f)*50.0;//((score + 49) / 50) * 50;
    if(player->score < nextMulti && player->score + score >= nextMulti){
        // get missile
        printf("rocket available now\n");
        player->rocketAvaiable = 1;
    }
    player->score += score;

    if(player->score > getLevelXp(player->level)){
        // levelup
        printf("level up, %d -> %d\n", player->level, player->level+1);
        printf("can pause now\n");
        printf("can armor? %d\n", player->armor.available);
        player->level++;
        player->canPause=1;
        if(player->level%10 == 0)
            player->livesRemaining++;
        if(player->level%3 == 0)
            player->armor.available = 1;
    }
}

int getLevelXp(int level){
    // y = log1.5(x)+5x+5
    float log = log2(level) / log2(1.5);
    float result = log + (5*level) + 5;
    return floor(result);
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
    printf("shooting %s\n", player->projectile->type==ROCKET ? "rocket" : "bullet");
    if(player->rocketAvaiable)
        player->rocketAvaiable=0;
}

void togglePause(Player *player, Timer *timer){
    if(!player->livesRemaining==0 && player->volume<=0)
        return;

    if(!player->isPaused && player->canPause){
        player->canPause = 0;
        player->isPaused = 1;
        pauseTimer(timer);
    }else if(player->isPaused){
        player->isPaused = 0;
        resumeTimer(timer);
    }
}

void activateArmor(Player *player){
    printf("trying to activate armor\n");
    if(player->armor.active || !player->armor.available){
        printf("armor already active or not available\n");
        return;
    }
    player->armor.active = 1;
    player->armor.available = 0;
    player->armor.startTime = al_get_time();
    player->armor.duration = randomFloat(5.0f, 10.0f);
    printf("armor time: %.2f\n", player->armor.duration);
}

void deactivateArmor(Player *player){
    printf("deactivating armor\n");
    player->armor.active = 0;
    player->armor.startTime = 0;
    player->armor.duration = 0;
}