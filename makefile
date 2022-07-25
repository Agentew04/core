libs = -lallegro -lallegro_font -lallegro_primitives -lm
err = -Wall -Wpedantic -Wextra -Werror
opt = -O3

all:
	gcc main.c -o main $(libs) $(err) $(opt) 
	./main


.PHONY: clean
clean:
	rm *.o *.out *.exe main -rf

