#include "timer.h"
#include "allegro5/allegro.h"
#include "allegro5/allegro_font.h"

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

void showTimer(ALLEGRO_FONT *font, ALLEGRO_DISPLAY *janela, Timer *timer){
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
