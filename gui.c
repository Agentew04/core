#include "gui.h"
#include "entity.h"
#include "timer.h"
#include "allegro5/allegro_ttf.h"
#include "main.h"

void showFps(ALLEGRO_FONT *font, double tempo, double lastTempo){

    double fpsD  = 1/(tempo-lastTempo);
    int fpsI = (int)fpsD;
    al_draw_textf(font, 
        al_map_rgb(255,255,255),
        0,al_get_display_height(al_get_current_display())-al_get_font_line_height(font),
        ALLEGRO_ALIGN_LEFT,
        "FPS: %d", fpsI);
}

void showHud(ALLEGRO_FONT *font, ALLEGRO_DISPLAY *janela, Player *player, Timer *timer, double tempo, double lastTempo){
    // draw timer and fps
    showFps(font, tempo, lastTempo);
    showTimer(font, janela, timer);

    // current level
    ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR green = al_map_rgb(0, 255, 0);
    ALLEGRO_COLOR red = al_map_rgb(255, 0, 0);
    al_draw_textf(font,
        white,
        0,0,
        ALLEGRO_ALIGN_LEFT,
        "Score: %d", player->score);
    al_draw_textf(font, 
        white,
        al_get_display_width(janela)/2, 0,
        ALLEGRO_ALIGN_CENTER,
        "Level: %d", player->level);

    al_draw_textf(font,
        white,
        0, al_get_font_line_height(font), 
        ALLEGRO_ALIGN_LEFT,
        "Vidas: %d", player->livesRemaining);

    al_draw_textf(font,
        white,
        al_get_display_width(janela), al_get_font_line_height(font),
        ALLEGRO_ALIGN_RIGHT,
        "Tamanho: %.0f", player->volume);
    
    al_draw_text(font,
        player->canPause ? green : red,
        al_get_display_width(janela), al_get_display_height(janela)-al_get_font_line_height(font),
        ALLEGRO_ALIGN_RIGHT,
        player->canPause ? "Pode pausar" : "Não pode pausar");
}

void showGame(Player *player, Enemy **enemies, int enemyCount, double lastTempo, ALLEGRO_FONT *font){
    if(player->isPaused){
        showPause(font);
    }else{
        resumeTimer(player->timer);
        updateCollision(enemies, enemyCount, player);
        updateMovement(enemies, enemyCount, player);   
        updateOther(player);     
        respawnEnemies(enemies, enemyCount);

        al_clear_to_color(al_map_rgb(0, 0, 0));
        ALLEGRO_COLOR topFade = al_map_rgb(34, 77, 92);
        ALLEGRO_COLOR bottomFade = al_map_rgb(3, 6, 7);
        draw_vertical_gradient_rect(0,0, SCR_W, SCR_H, topFade, bottomFade);
        
        showPlayer(player, al_get_current_display());
        showHud(font, al_get_current_display(), player, player->timer, al_get_time(), lastTempo);
        showEnemies(enemies, enemyCount);
        int died = player->livesRemaining<=0;
        int win = player->volume >= MAX_VOLUME;
        if(died || win)
            pauseTimer(player->timer);
    }
}

void showPause(ALLEGRO_FONT *font){
    ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
    al_clear_to_color(black);
    ALLEGRO_DISPLAY *janela = al_get_current_display();
    ALLEGRO_FONT *italic = al_load_font("Roboto/Roboto-Italic.ttf", al_get_font_line_height(font), 0);
    float fontH = al_get_font_line_height(font);
    al_draw_text(font,
        white,
        al_get_display_width(janela)/2, al_get_display_height(janela)/2,
        ALLEGRO_ALIGN_CENTER,
        "Jogo pausado");
    al_draw_text(italic,
        white,
        al_get_display_width(janela)/2, al_get_display_height(janela)/2 + fontH,
        ALLEGRO_ALIGN_CENTER,
        "Pressione 'ESC' para continuar");
    
    al_destroy_font(italic);
}

void showMainMenu(ALLEGRO_FONT *font){
    ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
    al_clear_to_color(black);
    ALLEGRO_DISPLAY *janela = al_get_current_display();
    ALLEGRO_FONT *fontBlack = al_load_font("Roboto/Roboto-Black.ttf", 50, 0);
    float width = al_get_display_width(janela);
    float height = al_get_display_height(janela);

    // draw title
    al_draw_text(fontBlack,
        white,
        width/2, al_get_font_line_height(fontBlack),
        ALLEGRO_ALIGN_CENTER,
        "Core: A defensive game");

    // render buttons
    Button bPlay = {
        .text = "Jogar",
        .p1 = { .x=width/2*0.9f, .y=(height/2)*0.9f},
        .p2 = { .x=width/2*1.1f, .y=(height/2)*1.1f}
    };
    Button bHelp = {
        .text = "Ajuda",
        .p1 = { .x=width/2*0.9f, .y=(2*height/3)*0.92f},
        .p2 = { .x=width/2*1.1f, .y=(2*height/3)*1.08f}
    };
    showButton(bPlay, font);
    showButton(bHelp, font);

    al_destroy_font(fontBlack);
}

void showEnd(ALLEGRO_FONT *font, Player *player, int win){
    ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR red = al_map_rgb(255, 0, 0);
    ALLEGRO_COLOR green = al_map_rgb(0, 255, 0);
    al_clear_to_color(black);
    ALLEGRO_DISPLAY *janela = al_get_current_display();
    ALLEGRO_FONT *fontBlack = al_load_font("Roboto/Roboto-MediumItalic.ttf", 50, 0);
    float width = al_get_display_width(janela);
    float height = al_get_display_height(janela);

    al_draw_text(fontBlack,
        win ? green : red,
        width/2, al_get_font_line_height(fontBlack)*2,
        ALLEGRO_ALIGN_CENTER,
        win ? "Você ganhou! :)" : "Game Over :(");
    al_draw_text(fontBlack,
        white,
        width/2, al_get_font_line_height(fontBlack)*5,
        ALLEGRO_ALIGN_CENTER,
        "Deseja jogar novamente?");
    al_draw_textf(fontBlack,
        white,
        width/2, al_get_font_line_height(fontBlack)*3,
        ALLEGRO_ALIGN_CENTER,
        "Pontuação: %d", player->score);
    al_draw_textf(fontBlack,
        white,
        width/2, al_get_font_line_height(fontBlack)*4,
        ALLEGRO_ALIGN_CENTER,
        "Tempo decorrido: %02d:%02d", getMinutes(player->timer), getSeconds(player->timer));
    Button bYes = {
        .text = "Sim",
        .p1 = { .x=width/2*0.9f, .y=(3*height/5)*0.9f},
        .p2 = { .x=width/2*1.1f, .y=(3*height/5)*1.1f}
    };
    Button bNo = {
        .text = "Não",
        .p1 = { .x=width/2*0.9f, .y=(4*height/5)*0.92f},
        .p2 = { .x=width/2*1.1f, .y=(4*height/5)*1.08f}
    };
    showButton(bYes, font);
    showButton(bNo, font);
    al_destroy_font(fontBlack);
}

void showButton(Button b, ALLEGRO_FONT *font){
    ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
    ALLEGRO_DISPLAY *janela = al_get_current_display();
    al_draw_filled_rectangle(b.p1.x, b.p1.y, b.p2.x, b.p2.y, white);
    float textX = b.p1.x + (b.p2.x - b.p1.x)/2;
    float textY = b.p1.y + (b.p2.y - b.p1.y)/2 - al_get_font_line_height(font)/2;
    al_draw_text(font,
        black,
        textX, textY,
        ALLEGRO_ALIGN_CENTER,
        b.text);
}

void handleButtons(int *currentMenu, Point mouse, Player **playerPtr){
    ALLEGRO_DISPLAY *janela = al_get_current_display();
    float width = al_get_display_width(janela);
    float height = al_get_display_height(janela);
    Button buttons[5] = {{
        .text = "Jogar", // 0
        .p1 = { .x=width/2*0.9f, .y=(height/2)*0.9f},
        .p2 = { .x=width/2*1.1f, .y=(height/2)*1.1f}
    },{
        .text = "Ajuda", // 1
        .p1 = { .x=width/2*0.9f, .y=(2*height/3)*0.92f},
        .p2 = { .x=width/2*1.1f, .y=(2*height/3)*1.08f}
    },{
        .text = "Voltar", // 2
        .p1 = { .x=width/2*0.9f, .y=(2*height/3)*0.92f},
        .p2 = { .x=width/2*1.1f, .y=(2*height/3)*1.08f}
    },{
        .text = "Sim", // 3
        .p1 = { .x=width/2*0.9f, .y=(3*height/5)*0.9f},
        .p2 = { .x=width/2*1.1f, .y=(3*height/5)*1.1f}
    },{ 
        .text = "Não", // 4
        .p1 = { .x=width/2*0.9f, .y=(4*height/5)*0.92f},
        .p2 = { .x=width/2*1.1f, .y=(4*height/5)*1.08f}
    }
    };
    for(int i=0; i<5; i++){
        Rectangle rect = {
            .p1 = buttons[i].p1,
            .p2 = buttons[i].p2
        };
        if(pointInsideRect(mouse, rect)){
            if (((i==0 || i==1) && *currentMenu==MENU_MAIN) ||
                ((i==2) && *currentMenu==MENU_HELP) ||
                ((i==3 || i==4) && (*currentMenu==MENU_END || *currentMenu==MENU_WIN))){
                switch (i){
                    case 3:
                        free(*playerPtr);
                        *playerPtr = generatePlayer();
                    case 0:
                        *currentMenu = MENU_GAME;
                        break;
                    case 1:
                        *currentMenu = MENU_HELP;
                        break;
                    case 4:
                        free(*playerPtr);
                        *playerPtr = generatePlayer();
                    case 2:
                        *currentMenu = MENU_MAIN;
                        break;
                }
                break;
            }
        }
    }
}

void showHelp(ALLEGRO_FONT *font){
    ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
    ALLEGRO_DISPLAY *janela = al_get_current_display();
    float width = al_get_display_width(janela);
    float height = al_get_display_height(janela);
    al_clear_to_color(black);

    const char *title = "Como jogar:";
    const char *t1 = "Gire o mouse para direcionar o escudo";
    const char *t2 = "Pressione o botão esquerdo do mouse para atirar";
    const char *t3 = "Segure espaço para usar a armadura";
    const char *t4 = "Aperte ESC para pausar e despausar";

    float fontH = al_get_font_line_height(font);
    al_draw_text(font,
        white,
        width/2, fontH,
        ALLEGRO_ALIGN_CENTER,
        title);
    al_draw_text(font,
        white,
        width/2, fontH*5,
        ALLEGRO_ALIGN_CENTER,
        t1);
    al_draw_text(font,
        white,
        width/2, fontH*6,
        ALLEGRO_ALIGN_CENTER,
        t2);
    al_draw_text(font,
        white,
        width/2, fontH*7,
        ALLEGRO_ALIGN_CENTER,
        t3);
    al_draw_text(font,
        white,
        width/2, fontH*8,
        ALLEGRO_ALIGN_CENTER,
        t4);
    
    Button bBack = {
        .text = "Voltar", 
        .p1 = { .x=width/2*0.9f, .y=(2*height/3)*0.92f},
        .p2 = { .x=width/2*1.1f, .y=(2*height/3)*1.08f}
    };
    showButton(bBack, font);

}





