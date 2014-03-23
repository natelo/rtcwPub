CC = gcc
CFLAGS = -O3 -DGAMEDLL

SOURCES = $(shell ls g_*.c q_*.c ai_*.c bg_animation.c bg_misc.c bg_pmove.c bg_slidemove.c ../botai/*.c)
MODULES = $(SOURCES:.c=.o)

all: gameclean qagame.mp.i386.so

gameclean:
	rm -rf *.o

qagame.mp.i386.so: $(MODULES)
	$(CC) -s -shared -Wl,-soname,$@ -o $@ $?

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.so
