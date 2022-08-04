Como instalar Core:

* Se tiver Makefile instalado:
    - Abra seu terminal na pasta do jogo e digite: "make"

* Senão:
    - Executar o seguinte comando para compilar o jogo(é um comando só, divido em linhas para legibilidade):
    "gcc timer.c geometry.c gui.c entity.c main.c -o main 
    -lallegro -lallegro_font -lallegro_primitives -lm -lallegro_ttf 
    -Wall -Wpedantic -Wextra -Werror -Wpedantic -O3"

    - Após a compilação, executar o seguinte comando para abrir o jogo:
    "./main"