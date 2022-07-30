
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>


// TODO REMOVE
#include "a.h"
#include "timer.h"
#include "geometry.h"
#include "hud.h"

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
    int enemyCount=1;
    Enemy **enemies = (Enemy**)malloc(enemyCount*sizeof(Enemy*));
    for (int i = 0; i < enemyCount; i++)
        enemies[i] = generateRandomEnemy();
    
    while (rodando) {
        lastTempo = tempo;
        tempo = al_get_time();
        printf("TEMPO = %f\n", tempo-lastTempo);

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
                    // 1 second
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
        showEnemies(enemies, enemyCount);
        updateEnemies(enemies, enemyCount);
        update(player, enemies, enemyCount);
        showPlayer(player, janela);
        ShowHud(fontMed, janela, player, timer, tempo, lastTempo);

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

void updateEnemies(Enemy **enemies, int nEnemies){
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
            free(e);
            enemies[i] = generateRandomEnemy();
        }
    }
}

void freeEnemies(Enemy **enemies, int nEnemies){
    for(int i=0; i<nEnemies; i++){
        free(enemies[i]);
    }
    free(enemies);
}
// end enemies

// player
void update(Player *player, Enemy **enemies, int nEnemies){
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
            // player killed by enemy
            if(player->volume<=0){
                player->livesRemaining--;
                player->volume = ENMY_MAX_VOLUME;
            }
        }

        // projectile hit enemy
        Projectile *proj = player->projectile;
        if(proj!=NULL)
        if(checkCircleCicleCollision(proj->pos, PROJ_RADIUS, e->pos, getRadiusFromVolume(e->volume))){
            e->isAlive = 0;
            // fix this bitch
            player->score+= proj->type==BULLET ? 2 : 5;
            float diam = getRadiusFromVolume(player->volume)*2;
            diam += (diam/100)*2; // add 2% of diameter
            player->volume += getVolumeFromRadius(diam/2);
            if(proj->type==BULLET){
                free(proj);
                player->projectile = NULL;
                continue;
            }
            proj->enemiesDestroyed++;
        }
    }
}

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
    p->volume = getRadiusFromVolume(PROJ_RADIUS);
    p->type = isRocket ? ROCKET : BULLET;
    p->bouncesLeft = isRocket ? 5 : 0;
    p->enemiesDestroyed = 0;
    player->rocketAvaiable = 0;
    // references
    player->projectile = p;
    p->shooter = player;
    return p;
}
// end player