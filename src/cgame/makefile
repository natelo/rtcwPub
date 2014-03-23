CC = gcc
CFLAGS = -O3

SOURCES = $(shell ls cg_*.c ../game/bg_animation.c ../game/bg_misc.c ../game/bg_pmove.c ../game/bg_slidemove.c ../game/q_math.c ../game/q_shared.c ../ui/ui_shared.c)
MODULES = $(SOURCES:.c=.o)

all: gameclean cgame.mp.i386.so

gameclean:
	rm -rf ../game/*.o

cgame.mp.i386.so: $(MODULES)
	$(CC) -s -shared -Wl,-soname,$@ -o $@ $?

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.so
