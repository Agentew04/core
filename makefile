modules = fps
objs = $(addsuffix .o, $(modules))
libs = -lallegro -lallegro_font -lallegro_primitives -lm
err = -Wall -Werror -Wpedantic -Wextra
opt = -O3

all: main
	./main
main: main.o $(modules)
	gcc $< -o $@ $(objs) $(err) $(opt) $(libs)

main.o: main.c 
	gcc -c $^ -o $@ $(err) $(opt)

$(modules): %: %.c
	gcc -c $^ -o $@.o $(err) $(opt)

.PHONY: clean
clean:
	rm *.o *.out main -rf

