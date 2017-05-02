CFLAGS=-Wall -Wextra -O0 -g3 -fsanitize=address -fsanitize=undefined
LDLIBS=-fsanitize=address -fsanitize=undefined

# CFLAGS=-Wall -Wextra -O2
# LDLIBS=

CFLAGS+=$(shell pkg-config --cflags ncurses glib-2.0)
LDLIBS+=$(shell pkg-config --libs ncurses glib-2.0)

all: boggle

%.o: %.c boggle.h Makefile
	$(CC) $(CFLAGS) -c $< -o $@

checkword: check.o board.o checkword.o

boggle: ui.o check.o board.o dict.o

clean:
	rm -f *.o boggle checkword
