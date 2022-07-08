all: exe

run: exe
	./main.out
exe: main
	gcc main.o help.o -o main.out -lallegro -lallegro_font -lallegro_primitives -lm
main: main.c help
	gcc -c main.c -o main.o
help: help.c
	gcc -c help.c -o help.o

clean:
	rm *.o *.out *.exe -rf