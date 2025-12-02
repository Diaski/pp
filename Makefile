SRC = main.c graphics.c init.c io.c physics.c funcs.c actors.c map.c game.c
# Prefer clang when present, otherwise fall back to system cc
CC = $(shell command -v clang >/dev/null 2>&1 && echo clang || echo cc)

# Prefer pkg-config for ncurses flags; fallback to common system paths
NCURSES_CFLAGS := $(shell pkg-config --cflags ncursesw 2>/dev/null || pkg-config --cflags ncurses 2>/dev/null || echo "-I/usr/include")
NCURSES_LIBS := $(shell pkg-config --libs ncursesw 2>/dev/null || pkg-config --libs ncurses 2>/dev/null || echo "-lncursesw")

CFLAGS = -O0 -g -Wall -Werror -Wextra $(NCURSES_CFLAGS) -fsanitize=address
LDFLAGS =

LDLIBS = $(NCURSES_LIBS)

all: swallow

swallow: $(SRC)
	python3 count_chars.py  
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o swallow $(LDLIBS) 

clean:
	rm -f swallow  

.phony: all clean