CC = gcc
CFLAGS = -O3

SOURCES = $(shell ls ui_*.c ../game/bg_misc.c ../game/q_math.c ../game/q_shared.c)
MODULES = $(SOURCES:.c=.o)

all: gameclean ui.mp.i386.so

gameclean:
	rm -rf ../game/*.o

ui.mp.i386.so: $(MODULES)
	$(CC) -s -shared -Wl,-soname,$@ -o $@ $?

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.so
