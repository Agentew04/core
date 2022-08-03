
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>


#include "a.h"
#include "timer.h"
#include "geometry.h"
#include "hud.h"
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
    *janela = al_create_display(SCR_W, SCR_H);

    if(!*fila_eventos || !*janela){
        printf("Erro ao inicializar o allegro.\n");
        exit(EXIT_FAILURE);
    }
    al_register_event_source(*fila_eventos, al_get_display_event_source(*janela));
    al_register_event_source(*fila_eventos, al_get_mouse_event_source()); // registrando mouse
    al_register_event_source(*fila_eventos, al_get_keyboard_event_source()); // registrando teclado

}



int main(void) {
    // Variável representando a janela principal
    ALLEGRO_DISPLAY *janela;
    ALLEGRO_FONT *fontPeq, *fontMed;
    ALLEGRO_EVENT_QUEUE *fila_eventos;
    init(&fila_eventos, &janela);
    fontPeq = al_create_builtin_font();
    fontMed = al_load_ttf_font("Roboto/Roboto-Bold.ttf", 36, 0);

    // 'global' vars
    int rodando = 1;
    double lastTempo, tempo = al_get_time();
    Player *player = generatePlayer();
    Point mouse = {0, 0};
    const Point mid = {SCR_W/2, SCR_H/2};
    Timer *timer = initTimer();
    startTimer(timer);

    // alloc enemies
    int enemyCount=20;
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
                    shoot(player);
                }
            }
            if(evento.type==ALLEGRO_EVENT_MOUSE_BUTTON_UP){}
            if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch (evento.keyboard.keycode) {
                    case ALLEGRO_KEY_ESCAPE:
                        rodando=0;
                    break;
                }
            }
            if (evento.type == ALLEGRO_EVENT_KEY_UP) {
                switch (evento.keyboard.keycode) {
                }
            }
        }

        al_clear_to_color(al_map_rgb(0, 0, 0));
        ALLEGRO_COLOR topFade = al_map_rgb(34, 77, 92);
        ALLEGRO_COLOR bottomFade = al_map_rgb(3, 6, 7);
        draw_vertical_gradient_rect(0,0, SCR_W, SCR_H, topFade, bottomFade);
        showPlayer(player, janela);
        ShowHud(fontMed, janela, player, timer, tempo, lastTempo);
        showEnemies(enemies, enemyCount);

        updateCollision(enemies, enemyCount, player);
        updateMovement(enemies, enemyCount, player);        

        al_flip_display();

    }

    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);
    al_destroy_font(fontPeq);
    freeTimer(timer);
    freeEnemies(enemies, enemyCount);

    return 0;
}


void freeEnemies(Enemy **enemies, int nEnemies){
    for(int i=0; i<nEnemies; i++){
        free(enemies[i]);
    }
    free(enemies);
}

void updateMovement(Enemy **enemies, int nEnemies, Player *player){
    // enemies
    for(int i=0; i<nEnemies; i++){
        if(!enemies[i]->isAlive)
            continue;
        Enemy *e = enemies[i];
        float finalSpeed = e->speed;
        if(al_get_time()<player->slowUntil){
            printf("slowed\n");
            finalSpeed *= 0.25;
        }
        float dx = finalSpeed*cos(e->alpha);
        float dy = finalSpeed*(-sin(e->alpha));
        e->pos.x += dx;
        e->pos.y += dy;

        // check if out of visible screen
        int outOfScreen = e->pos.x<0||
            e->pos.x>SCR_W||
            e->pos.y<0||
            e->pos.y>SCR_H;
        // P+a.vec = M
        // P -> pos // M -> relMid // a -> unknown realNumber // vec -> direction(dx,dy)
        float a = (e->relMid.x - e->pos.x)/dx;
        int alphaOutOfScreen = a<0;
        if(outOfScreen && alphaOutOfScreen){
            //e->isAlive = 0;
        }
    }

    // projectile
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

void updateCollision(Enemy **enemies, int nEnemies, Player *player){
    if(player->volume<=0 || player->livesRemaining<=0)
        return;

    // check for collisions
    for (int i = 0; i < nEnemies; i++)
    {
        Enemy* e = enemies[i];
        if(!e->isAlive)
            continue;

        // killed by shield
        if(checkEnemyShieldCollision(e, player)){
            printf("enemy killed by shield\n");
            e->isAlive = 0;
            if(!e->isAlly)
                addScore(1, player);
            continue;
        }

        // enemy hit player
        if(checkEnemyPlayerCollision(e, player)){
            printf("enemy hit player\n");
            player->volume += e->isAlly ? e->volume : -e->volume;
            e->isAlive = 0;
            if(player->volume<=0){
                player->livesRemaining--;
                player->volume = INIT_VOLUME;
            }
            continue;
        }

        // enemy hit big shield
        if(player->shieldActive){
            Circle c1 = {
                .center = { SCR_W/2, SCR_H/2},
                .radius = getRadiusFromVolume(player->volume)
            };
            Circle c2 = {
                .center = { e->pos.x, e->pos.y},
                .radius = getRadiusFromVolume(e->volume)
            };
            if(checkCircleCicleCollision(c1,c2)){
                addScore(3, player);
                continue;
            }
        }

        // projectile hit enemy
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
            printf("colisao tiro x inimigo\n");
            e->isAlive = 0;
            proj->enemiesDestroyed++;
            float diam = getRadiusFromVolume(player->volume);
            diam += (diam/100)*2; // add 2% of diameter
            player->volume += getVolumeFromRadius(diam);
            // colidiu
            if(proj->type==BULLET){
                // bullet
                addScore(1, player);
                free(proj);
                player->projectile = NULL;
            }else{
                // rocket
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