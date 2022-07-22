
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>


// TODO REMOVE
#include "a.h"

void init(ALLEGRO_EVENT_QUEUE **fila_eventos, ALLEGRO_FONT **font, ALLEGRO_DISPLAY **janela){
    srand(time(NULL));
    al_init(); 
    al_init_font_addon();
    al_init_primitives_addon();
    al_install_mouse();
    al_install_keyboard();
    *font = al_create_builtin_font();
    *fila_eventos = al_create_event_queue();
    *janela = al_create_display(SCR_W, SCR_H);
    al_register_event_source(*fila_eventos, al_get_display_event_source(*janela));
    al_register_event_source(*fila_eventos, al_get_mouse_event_source()); // registrando mouse
    al_register_event_source(*fila_eventos, al_get_keyboard_event_source()); // registrando teclado
}


int main(void) {
    // Variável representando a janela principal
    ALLEGRO_DISPLAY *janela;
    ALLEGRO_FONT *font;
    ALLEGRO_EVENT_QUEUE *fila_eventos;
    init(&fila_eventos, &font, &janela);

    // 'global' vars
    int rodando = 1;
    double lastTempo, tempo = al_get_time();
    Player *player = (Player*)malloc(sizeof(Player));
    Point mouse = {0, 0};
    const Point mid = {SCR_W/2, SCR_H/2};
    Timer *timer = initTimer();
    startTimer(timer);
    Enemy *enemy = generateRandomEnemy();
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

        showFps(font, tempo, lastTempo, 1);
        showTimer(font, janela, timer, 1);
        showPlayer(player);
        showEnemy(enemy);
        updateEnemies(enemy, 1);


        // atualiza tela
        al_flip_display();

    }


    // Finaliza a janela
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);
    freeTimer(timer);

    return 0;
}


// fps
void showFps(ALLEGRO_FONT *font, double tempo, double lastTempo, int show){
    if(!show)
        return;

    double fps  = 1/(tempo-lastTempo);
    al_draw_textf(font, 
        al_map_rgb(255,255,255),
        0,0,
        ALLEGRO_ALIGN_LEFT,
        "FPS: %.1f", fps);
}
// end fps

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

// mouse
float getAngleBetweenPoints(Point p1, Point p2){
    double rad = atan2(p1.y-p2.y, p2.x - p1.x) /**180/PI*/;
    printf("%lf\n", rad*180/PI);
    return rad;
}
// end mouse

// renders
void showShield(Player *player){
    ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
    float angIni = -PI/2-player->alpha;
    float deltaAng = PI;
    al_draw_arc(SCR_W/2, SCR_H/2, SHIELD_RADIUS, angIni, deltaAng, white, 3);
}

void showPlayer(Player *player){
    ALLEGRO_COLOR green = al_map_rgb(0, 255, 0);
    float radius = getRadiusFromVolume(player->volume);
    showShield(player);
    al_draw_filled_circle(SCR_W/2, SCR_H/2, radius, green);
}
// end player

// enemies
void showEnemy(Enemy *enemy){
    ALLEGRO_COLOR red = al_map_rgb(255, 0, 0);
    float radius = getRadiusFromVolume(enemy->volume);
    al_draw_filled_circle(enemy->pos.x, enemy->pos.y, radius, red);
}
int checkEnemyShieldCollision(Enemy* enemy, Point mouse){
    Point mid = { SCR_W/2, SCR_H/2 };
    if(checkCircleArcCollision(mid, mouse, enemy->pos, getRadiusFromVolume(enemy->volume))){
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
        e->pos.x = rand()%((SCR_W+BUFFER_SIZE)-(-BUFFER_SIZE))+(-BUFFER_SIZE);
        e->pos.y = rand()%((0)-(-BUFFER_SIZE))+(-BUFFER_SIZE);
        break;
    case 1: // right buffer
        e->pos.x = rand()%((SCR_W+BUFFER_SIZE)-(SCR_W))+(SCR_W);
        e->pos.y = rand()%((SCR_H+BUFFER_SIZE)-(-BUFFER_SIZE))+(-BUFFER_SIZE);
        break;
    case 2: // bottom buffer
        e->pos.x = rand()%((SCR_W+BUFFER_SIZE)-(-BUFFER_SIZE))+(-BUFFER_SIZE);
        e->pos.y = rand()%((SCR_H+BUFFER_SIZE)-(SCR_H))+(SCR_H);
        break;
    case 3: // left buffer
        e->pos.x = rand()%((0)-(-BUFFER_SIZE))+(-BUFFER_SIZE);
        e->pos.y = rand()%((SCR_H+BUFFER_SIZE)-(-BUFFER_SIZE))+(-BUFFER_SIZE);
        break;
    default:
        break;
    }
    // random speed
    e->speed = rand()%(MAX_SPEED-MIN_SPEED)+MIN_SPEED;
    // calculate alpha to center
    Point mid = { SCR_W/2, SCR_H/2 };
    e->alpha = getAngleBetweenPoints(e->pos, mid);
    e->volume = rand()%(MAX_VOLUME-MIN_VOLUME)+MIN_VOLUME;
    return e;
}
void updateEnemies(Enemy *enemies, int nEnemies){
    for(int i=0; i<nEnemies; i++){
        // get x component from alpha
        enemies[i].pos.x += enemies[i].speed*cos(enemies[i].alpha);
        // get y component from alpha
        enemies[i].pos.y += enemies[i].speed*sin(enemies[i].alpha);
    }
}
// end enemies