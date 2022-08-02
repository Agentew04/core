
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
                // shoot
                if (evento.mouse.button == 1) {

                    if (tempo - player->lastShot > 1 && player->projectile==NULL) {
                        player->projectile = generateProjectile(player, player->rocketAvaiable);
                        player->lastShot = tempo;
                    }
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
                    case ALLEGRO_KEY_UP:
                    player->volume+=25;
                    break;
                }
            }
        }

        al_clear_to_color(al_map_rgb(0, 0, 0));

        ALLEGRO_COLOR topFade = al_map_rgb(34, 77, 92);
        ALLEGRO_COLOR bottomFade = al_map_rgb(3, 6, 7);
        draw_vertical_gradient_rect(0,0, SCR_W, SCR_H, topFade, bottomFade);
        showPlayer(player, janela);
        ShowHud(fontMed, janela, player, timer, tempo, lastTempo);

        updateCollision(enemies, enemyCount, player);
        updateMovement(enemies, enemyCount, player);        

        showEnemies(enemies, enemyCount);

        // atualiza tela
        al_flip_display();

    }


    // Finaliza a janela
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);
    al_destroy_font(fontPeq);
    freeTimer(timer);
    freeEnemies(enemies, enemyCount);

    return 0;
}

// renders
void showShield(Player *player){
    ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
    float angIni = -M_PI/2-player->alpha;
    float deltaAng = M_PI;
    al_draw_arc(SCR_W/2, SCR_H/2, SHIELD_RADIUS, angIni, deltaAng, white, 3);
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




void freeEnemies(Enemy **enemies, int nEnemies){
    for(int i=0; i<nEnemies; i++){
        free(enemies[i]);
    }
    free(enemies);
}
// end enemies


// end player

void updateMovement(Enemy **enemies, int nEnemies, Player *player){
    // enemies
    for(int i=0; i<nEnemies; i++){
        if(!enemies[i]->isAlive)
            continue;
        Enemy *e = enemies[i];
        float dx = e->speed*cos(e->alpha);
        float dy = e->speed*(-sin(e->alpha));
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
            e->isAlive = 0;
        }
    }

    // projectile
    Projectile *proj = player->projectile;
    if(proj!=NULL){
        float dx = proj->speed*cos(proj->alpha);
        float dy = proj->speed*(-sin(proj->alpha));
        proj->pos.x += dx;
        proj->pos.y += dy;
        if(proj->bouncesLeft>0 && proj->type==ROCKET){
            proj->bouncesLeft--;
            // bounce off
            float A = proj->alpha;
            float B = M_PI - 2*A;
            float C = M_PI - (A + B);
            float newAlpha = M_PI - C;
            proj->alpha = newAlpha;
        }else{
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
            e->isAlive = 0;
            if(!e->isAlly)
                player->score++;
            continue;
        }

        // enemy hit player
        if(checkEnemyPlayerCollision(e, player)){
            player->volume += e->isAlly ? e->volume : -e->volume;
            e->isAlive = 0;
            if(player->volume<=0){
                player->livesRemaining--;
                player->volume = INIT_VOLUME;
            }
        }

        // projectile hit enemy
        Projectile *proj = player->projectile;
        if(proj!=NULL)
        if(checkCircleCicleCollision(proj->pos, PROJ_RADIUS, e->pos, getRadiusFromVolume(e->volume))){
            e->isAlive = 0;
            proj->enemiesDestroyed++;
            // colidiu
            if(proj->type==BULLET){
                free(proj);
                player->score++;
                player->projectile = NULL;
                float diam = getRadiusFromVolume(player->volume)*2;
                diam += (diam/100)*2; // add 2% of diameter
                player->volume += getVolumeFromRadius(diam/2);
            }else{
                
            }
            player->score+= proj->type==BULLET ? 2 : 5;
            if(proj->type==BULLET){
                free(proj);
                player->projectile = NULL;
                continue;
            }
        }
    }
}