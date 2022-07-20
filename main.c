
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>


// TODO REMOVE
#include "a.h"

// defines
#define SCR_W 1280
#define SCR_H 720
#define PI 3.14


int main(void) {
    // Variável representando a janela principal
    ALLEGRO_DISPLAY *janela = NULL;
    ALLEGRO_FONT *font = NULL;
    ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;

    // Inicializamos a biblioteca
    al_init();
    al_init_font_addon();
    al_init_primitives_addon();
    al_install_mouse();
    al_install_keyboard();
    Timer *timer = initTimer();
    startTimer(timer);

    font = al_create_builtin_font();
    fila_eventos = al_create_event_queue();

    janela = al_create_display(SCR_W, SCR_H);

    al_register_event_source(fila_eventos, al_get_display_event_source(janela));

    al_register_event_source(fila_eventos, al_get_mouse_event_source()); // registrando mouse

    al_register_event_source(fila_eventos, al_get_keyboard_event_source()); // registrando teclado
    int rodando = 1;
    double lastTempo, tempo = al_get_time();
    float mouseX=0, mouseY=0;
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
                mouseX = evento.mouse.x;
                mouseY = evento.mouse.y;
            }

            if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                
            }
            if(evento.type==ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            }

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

        showFps(font, tempo, lastTempo, 1);
        showTimer(font, janela, timer, 1);
        Point mouse = {mouseX, mouseY};
        showShield(mouse);



        // atualiza tela
        al_flip_display();

    }


    // Finaliza a janela
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);

    return 0;
}

void handleEvents(){
    
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
float getMouseAngle(Point src, Point mouse){
    double rad = atan2(src.y-mouse.y, mouse.x - src.x) /**180/PI*/;
    printf("%lf\n", rad*180/PI);
    return rad;
}
// end mouse

// player
void showShield(Point mouse){
    ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
    Point mid = { SCR_W/2, SCR_H/2 };
    float alpha = getMouseAngle(mid, mouse);
    float angIni = -PI/2-alpha;
    float deltaAng = PI;
    al_draw_arc(mid.x, mid.y, 75, angIni, deltaAng, white, 2);
}
// end player