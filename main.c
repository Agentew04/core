
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include "help.h"

#define TAMANHO_TELA_X 640
#define TAMANHO_TELA_Y 480
#define pi 3.1415


void rotate(float angle, float *x, float *y, float innerAngle, float size, float offsetX, float offsetY) {
    *x = cos((innerAngle + angle) * pi/180) * size/2 + offsetX;
    *y = sin((innerAngle + angle) * pi/180) * size/2 + offsetY;
}


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

    font = al_create_builtin_font();
    fila_eventos = al_create_event_queue();

    janela = al_create_display(TAMANHO_TELA_X, TAMANHO_TELA_Y);

    al_register_event_source(fila_eventos, al_get_display_event_source(janela));

    al_register_event_source(fila_eventos, al_get_mouse_event_source()); // registrando mouse

    al_register_event_source(fila_eventos, al_get_keyboard_event_source()); // registrando teclado
    int rodando = 1;
    double lastTempo, tempo = al_get_time();
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

        showFps(tempo, lastTempo);

        // atualiza tela
        al_flip_display();

    }


    // Finaliza a janela
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);

    return 0;
}
