CFLAGS=-Wall -Wextra -O0 -g3
LDLIBS=

# CFLAGS+=-fsanitize=address -fsanitize=undefined
# LDLIBS+=-fsanitize=address -fsanitize=undefined
#
# CFLAGS=-Wall -Wextra -O2
# LDLIBS=

CFLAGS+=$(shell pkg-config --cflags ncurses)
LDLIBS+=$(shell pkg-config --libs ncurses )

all: ui

check.o: check.c boggle.h

board.o: board.c boggle.h

checkword: check.o board.o checkword.o

check: check.o board.o

ui: ui.o check.o board.o

clean:
	rm -f *.o board boggle check
