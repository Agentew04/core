modules = timer geometry gui entity
objs = $(addsuffix .o, $(modules))
libs = -lallegro -lallegro_font -lallegro_primitives -lm -lallegro_ttf
err = -Wall -Wpedantic -Wextra -Werror -Wpedantic
opt = -O3

all: $(modules)
	gcc main.c -o main $(objs) $(libs) $(err) $(opt) 
	./main

$(modules):
	gcc -c $@.c -o $@.o $(libs) $(opt)

.PHONY: clean
clean:
	rm *.o *.out *.exe main -rf

