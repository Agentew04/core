modules = timer geometry hud a
objs = $(addsuffix .o, $(modules))
libs = -lallegro -lallegro_font -lallegro_primitives -lm
err = -Wall -Wpedantic -Wextra -Werror
opt = -O3

all: $(modules)
	gcc main.c -o main $(objs) $(libs) $(err) $(opt) 
	./main

$(modules):
	gcc -c $@.c -o $@.o $(libs) $(opt)

.PHONY: clean
clean:
	rm *.o *.out *.exe main -rf

