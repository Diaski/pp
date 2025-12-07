# Prefer clang
CC = clang

# Config
# Modify your CFLAGS line to include -D_DEFAULT_SOURCE
CFLAGS = -O0 -g -Wall -Werror -Wpedantic -Wshadow

# Linker flags (Sanitizers must be linked here too!)
LDFLAGS = -fsanitize=address -fsanitize=undefined

LDLIBS = $(shell pkg-config --libs ncursesw 2>/dev/null || echo "-lncurses")

# Define sources and object files
SRC = main.c game.c player.c enemy.c star.c leaderboard.c io.c graphics.c map.c init.c physics.c
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)

all: swallow

# Link step
swallow: $(OBJ)
	python3 count_chars.py
	$(CC) $(LDFLAGS) $(OBJ) -o $@ $(LDLIBS)

# Compile step (Implicit rule, but good to be explicit)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f swallow $(OBJ) $(DEP)

lint:
	clang-tidy $(SRC) -- $(CFLAGS)

-include $(DEP)

.PHONY: all clean lint
