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
    p->lastShot = 0;
    p->projectile = NULL;
    p->shieldAvaiable = 0;
    p->volume = INIT_VOLUME;
    return p;
}
Enemy* generateRandomEnemy(){
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
    p->speed = PROJ_SPEED;
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
        .startAlpha = -M_PI/2-player->alpha,
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
    if(checkCircleCicleCollision(mid, rPlayer, enemy->pos, rEnemy)){
        return 1;
    }
    return 0;
}

void shoot(Player *player){
    double now = al_get_time();
    if(now - player->lastShot < 1 || player->projectile!=NULL){
        return;
    }
    player->projectile = generateProjectile(player, 0);
    // shoot
}