.PHONY: all clean

all: main.c
	gcc `sdl2-config --cflags --libs` -lm main.c -o axes

clean:
	rm -f axes
