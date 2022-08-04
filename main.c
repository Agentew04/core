
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>


#include "main.h"
#include "timer.h"
#include "geometry.h"
#include "gui.h"
#include "entity.h"

void init(ALLEGRO_EVENT_QUEUE **fila_eventos, ALLEGRO_DISPLAY **janela){
    srand(time(NULL));
    al_init(); 
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_install_mouse();
    al_install_keyboard();
    *fila_eventos = al_create_event_queue();
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    *janela = al_create_display(SCR_W, SCR_H);

    if(!*fila_eventos || !*janela){
        exit(EXIT_FAILURE);
    }
    al_register_event_source(*fila_eventos, al_get_display_event_source(*janela));
    al_register_event_source(*fila_eventos, al_get_mouse_event_source()); 
    al_register_event_source(*fila_eventos, al_get_keyboard_event_source()); 

}



int main(void) {
    ALLEGRO_DISPLAY *janela;
    ALLEGRO_FONT *fontPeq, *fontMed;
    ALLEGRO_EVENT_QUEUE *fila_eventos;
    init(&fila_eventos, &janela);
    fontPeq = al_load_ttf_font("Roboto/Roboto-Regular.ttf", 12, 0);
    fontMed = al_load_ttf_font("Roboto/Roboto-Bold.ttf", 36, 0);

    int rodando = 1;
    double lastTempo, tempo = al_get_time();
    Player *player = generatePlayer();
    Point mouse = {0, 0};
    const Point mid = {SCR_W/2, SCR_H/2};
    int currentMenu = MENU_MAIN;

    const int enemyCount=25;
    Enemy **enemies = (Enemy**)malloc(enemyCount*sizeof(Enemy*));
    for (int i = 0; i < enemyCount; i++)
        enemies[i] = generateRandomEnemy();
    
    while (rodando) {
        lastTempo = tempo;
        tempo = al_get_time();

        while (!al_is_event_queue_empty(fila_eventos))
        {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);

            if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                rodando = 0;
            }
            if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) {
                mouse.x = evento.mouse.x;
                mouse.y = evento.mouse.y;
                player->alpha = getAngleBetweenPoints(mid, mouse);
            }
            if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                if (evento.mouse.button == 1) {
                    if(currentMenu==MENU_GAME)
                        shoot(player);
                    else{
                        handleButtons(&currentMenu, mouse, &player, enemies, enemyCount);
                    }
                }
            }
            if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch (evento.keyboard.keycode) {
                    case ALLEGRO_KEY_ESCAPE:
                        togglePause(player);
                    break;
                    case ALLEGRO_KEY_SPACE:
                        if(!player->isPaused)
                            activateArmor(player);
                        break;
                }
            }
            if (evento.type == ALLEGRO_EVENT_KEY_UP) {
                if(!player->isPaused && currentMenu==MENU_GAME)
                    switch (evento.keyboard.keycode) {
                        case ALLEGRO_KEY_SPACE:
                            deactivateArmor(player);
                            break;
                    }
            }
        }
        switch (currentMenu){
        case MENU_MAIN:
            showMainMenu(fontMed);
            break;
        case MENU_GAME:
            showGame(player, enemies, enemyCount, lastTempo, fontMed);
            break;
        case MENU_END:
            showEnd(fontMed, player, 0);
            break;
        case MENU_HELP:
            showHelp(fontMed);
            break;
        case MENU_WIN:
            showEnd(fontMed, player, 1);
            break;
        }

        if(player->livesRemaining<=0){
            currentMenu = MENU_END;
        }
        if(player->volume >= MAX_VOLUME){
            currentMenu = MENU_WIN;
        }

        al_flip_display();

    }

    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);
    al_destroy_font(fontPeq);
    freeTimer(player->timer);
    free(player->projectile);
    freeEnemies(enemies, enemyCount);

    return 0;
}
void draw_vertical_gradient_rect(float x, float y, float w, float h, ALLEGRO_COLOR top, ALLEGRO_COLOR bottom) {
    ALLEGRO_VERTEX v[] = {
        {.x=x    , .y=y    , .z=0, .color=top},
        {.x=x + w, .y=y    , .z=0, .color=top},
        {.x=x    , .y=y + h, .z=0, .color=bottom},
        {.x=x + w, .y=y + h, .z=0, .color=bottom}};
    al_draw_prim(v, NULL, NULL, 0, 4, ALLEGRO_PRIM_TRIANGLE_STRIP);
}

void freeEnemies(Enemy **enemies, int nEnemies){
    for(int i=0; i<nEnemies; i++){
        free(enemies[i]);
    }
    free(enemies);
}

void updateMovement(Enemy **enemies, int nEnemies, Player *player){
    for(int i=0; i<nEnemies; i++){
        if(!enemies[i]->isAlive)
            continue;
        Enemy *e = enemies[i];
        float finalSpeed = e->speed;
        if(al_get_time()<player->slowUntil)
            finalSpeed *= 0.25;
        finalSpeed *= getSpeedIncrease(player->level);
        float dx = finalSpeed*cos(e->alpha);
        float dy = finalSpeed*(-sin(e->alpha));
        e->pos.x += dx;
        e->pos.y += dy;

        int outOfScreen = e->pos.x<0||
            e->pos.x>SCR_W||
            e->pos.y<0||
            e->pos.y>SCR_H;
        float a = (e->relMid.x - e->pos.x)/dx;
        int alphaOutOfScreen = a<0;
        if(outOfScreen && alphaOutOfScreen){
            e->isAlive = 0;
        }
    }

    Projectile *proj = player->projectile;
    if(proj!=NULL){
        float dx = proj->speed*cos(proj->alpha);
        float dy = proj->speed*(-sin(proj->alpha));
        proj->pos.x += dx;
        proj->pos.y += dy;
        int isOut = proj->pos.x<0||
            proj->pos.x>SCR_W||
            proj->pos.y<0||
            proj->pos.y>SCR_H;
        
        
        if(proj->bouncesLeft>0 && proj->type==ROCKET && isOut){
            proj->bouncesLeft--;
            Point mid = {SCR_W/2, SCR_H/2};
            Point normal = vecFromPoints(proj->pos, mid);
            int outLeftRight = proj->pos.x<0||proj->pos.x>SCR_W;
            int outUpDown = proj->pos.y<0||proj->pos.y>SCR_H;
            if(outLeftRight){
                normal.y = 0;
            }
            else if(outUpDown){
                normal.x = 0;
            }
            normal = vecNormalize(normal);
            Point vecMove = vecFromAngleLength(proj->alpha, proj->speed);
            Point reflex = vecSubtract(
                vecMove, 
                vecMultiply(
                    normal,
                    dotProduct(vecMove, normal)*2
                )    
            );
            proj->alpha = vecAngle(reflex);
        }else if(isOut){
            free(proj);
            player->projectile = NULL;
        }
    }

}

void updateOther(Player *player){
    if(player->armor.active){
        double time = al_get_time();
        if(time>player->armor.startTime+player->armor.duration){
            player->armor.active=0;
        }else{
            double deltaTime = 0.25f; 
            double damage = 2.0f; 
            if(time>player->armor.lastHit+deltaTime){
                player->armor.lastHit = time;
                player->volume -= damage;
                if(player->volume<50)
                    player->volume = 50;
            }
        }
    }
}

void updateCollision(Enemy **enemies, int nEnemies, Player *player){
    if(player->volume<=0 || player->livesRemaining<=0)
        return;
    for (int i = 0; i < nEnemies; i++)
    {
        Enemy* e = enemies[i];
        if(!e->isAlive)
            continue;

        if(checkEnemyShieldCollision(e, player)){
            e->isAlive = 0;
            if(!e->isAlly)
                addScore(1, player);
            continue;
        }

        if(checkEnemyPlayerCollision(e, player)){
            player->volume += e->isAlly ? e->volume : -e->volume;
            e->isAlive = 0;
            if(player->volume<=0){
                player->livesRemaining--;
                player->volume = INIT_VOLUME;
            }
            continue;
        }

        if(player->armor.active){
            Circle c1 = {
                .center = { SCR_W/2, SCR_H/2},
                .radius = player->armor.radius
            };
            Circle c2 = {
                .center = { e->pos.x, e->pos.y},
                .radius = getRadiusFromVolume(e->volume)
            };
            if(checkCircleCicleCollision(c1,c2)){
                addScore(3, player);
                e->isAlive = 0;
                continue;
            }
        }

        Projectile *proj = player->projectile;
        if(proj!=NULL){
            Circle c1 = {
                .center = proj->pos,
                .radius = PROJ_RADIUS
            };
            Circle c2 = {
                .center = e->pos,
                .radius = getRadiusFromVolume(e->volume)
            };
            if(!checkCircleCicleCollision(c1, c2))
                continue;
            e->isAlive = 0;
            proj->enemiesDestroyed++;
            float diam = getRadiusFromVolume(player->volume)*2;
            diam += diam*0.02;
            player->volume = getVolumeFromRadius(diam/2);
            if(proj->type==BULLET){
                addScore(1, player);
                free(proj);
                player->projectile = NULL;
            }else{
                if(e->isAlly){
                    player->slowUntil = al_get_time() + SLOW_TIME;
                }else{
                    addScore(5, player);
                }
                if(proj->enemiesDestroyed>=3){
                    free(proj);
                    player->projectile = NULL;
                }
            }
       }
    }
}

void respawnEnemies(Enemy **enemies, int nEnemies, int force){
    for (int i = 0; i < nEnemies; i++)
    {
        if(enemies[i]->isAlive && !force)
            continue;
        free(enemies[i]);
        enemies[i] = generateRandomEnemy();
    }
    
}

