#include "a.h"
#include "entity.h"
#include <stdio.h>
#include <malloc.h>

Player* generatePlayer(){
    Player *p = (Player*)malloc(sizeof(Player));
    if(p==NULL){
        printf("erro ao alocar player\n");
        return NULL;
    }
    p->alpha = 0;
    p->livesRemaining = 3;
    p->level = 1;
    p->score = 0;
    p->shieldAvaiable = 0;
    p->shieldActive = 0;
    p->lastShot = 0;
    p->slowUntil = 0;
    p->projectile = NULL;
    p->shieldAvaiable = 0;
    p->volume = INIT_VOLUME;
    return p;
}
Enemy* generateRandomEnemy(){
    printf("generating enemy\n");
    Enemy* e = (Enemy*)malloc(sizeof(Enemy));
    // get random point in border of screen
    int border = rand()%5;
    switch (border)
    {
    case 0: // top buffer
        e->pos.x = rand()%((SCR_W+BUFFER_SIZE)-(0))+(0);
        e->pos.y = rand()%((0)-(-BUFFER_SIZE))+(-BUFFER_SIZE);
        break;
    case 1: // right buffer
        e->pos.x = rand()%((SCR_W+BUFFER_SIZE)-(SCR_W))+(SCR_W);
        e->pos.y = rand()%((SCR_H+BUFFER_SIZE)-(0))+(0);
        break;
    case 2: // bottom buffer
        e->pos.x = rand()%((SCR_W)-(-BUFFER_SIZE))+(-BUFFER_SIZE);
        e->pos.y = rand()%((SCR_H+BUFFER_SIZE)-(SCR_H))+(SCR_H);
        break;
    case 3: // left buffer
        e->pos.x = rand()%((0)-(-BUFFER_SIZE))+(-BUFFER_SIZE);
        e->pos.y = rand()%((SCR_H)-(-BUFFER_SIZE))+(-BUFFER_SIZE);
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
    p->alpha = player->alpha;
    p->speed = isRocket ? ROCKET_SPEED : BULLET_SPEED;
    p->type = isRocket ? ROCKET : BULLET;
    p->bouncesLeft = isRocket ? 5 : 0;
    p->enemiesDestroyed = 0;
    player->rocketAvaiable = 0;
    // references
    player->projectile = p;
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

void shoot(Player *player){
    double now = al_get_time();
    if(now - player->lastShot < 1 || player->projectile!=NULL){
        return;
    }
    player->projectile = generateProjectile(player, player->rocketAvaiable);
    player->lastShot = now;
    printf("shooting %s\n", player->projectile->type==ROCKET ? "rocket" : "bullet");
    if(player->rocketAvaiable)
        player->rocketAvaiable=0;
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

void showPlayer(Player *player, ALLEGRO_DISPLAY *janela){
    if(player->volume<=0)
        return;
    ALLEGRO_COLOR green = al_map_rgb(0, 255, 0);
    float radius = getRadiusFromVolume(player->volume);
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
    int nextMulti = ((score + 49) / 50) * 50;
    if(player->score < nextMulti && player->score+score>nextMulti){
        // get missile
        player->rocketAvaiable = 1;
    }
    player->score += score;

    if(player->score > getLevelXp(player->level)){
        // levelup
        printf("level up, %d -> ", player->level);
        player->level++;
        printf("%d\n", player->level);
        if(player->level%10 == 0)
            player->livesRemaining++;
        if(player->level%3 == 0)
            player->shieldAvaiable = 1;
    }
}

int getLevelXp(int level){
    // y = log1.5(x)+5x+5
    float log = log2(level) / log2(1.5);
    float result = log + (5*level) + 5;
    return floor(result);
}