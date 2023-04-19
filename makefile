modules = timer geometry gui entity
objs = $(addsuffix .o, $(modules))
libs = -lallegro -lallegro_font -lallegro_primitives -lm -lallegro_ttf
err = -Wall -Wpedantic -Wextra -Werror
opt = -O3

all: main
	./main

main: $(modules)
	gcc main.c -o main $(objs) $(libs) $(err) $(opt)

$(modules): %: %.c
	gcc -c $@.c -o $@.o $(libs) $(opt) $(err)

.PHONY: clean
clean:
	rm *.o *.out *.exe main -rf

