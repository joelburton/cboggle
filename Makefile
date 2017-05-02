CFLAGS=-Wall -Wextra -O0 -g3 -fsanitize=address -fsanitize=undefined
LDLIBS=-fsanitize=address -fsanitize=undefined

# CFLAGS=-Wall -Wextra -O2
# LDLIBS=

CFLAGS+=$(shell pkg-config --cflags ncurses glib-2.0)
LDLIBS+=$(shell pkg-config --libs ncurses glib-2.0)

all: boggle

%.o: %.c boggle.h Makefile
	$(CC) $(CFLAGS) -c $< -o $@

checkword: check.o board.o checkword.o dict.o

boggle: ui.o check.o board.o dict.o
	$(CC) $+ -o $@ $(LDLIBS) 

clean:
	rm -f *.o boggle checkword boggle-static boggle.zip

deploy:
	rm -f *.o
	make all CFLAGS="$(shell pkg-config --cflags ncurses glib-2.0)" LDLIBS="$(shell pkg-config --cflags ncurses glib-2.0)"
	$(CC) -O2 -o boggle-static ui.o check.o board.o dict.o /opt/local/lib/libncurses.a /opt/local/lib/libglib-2.0.a /opt/local/lib/libintl.a /opt/local/lib/libiconv.a -framework CoreFoundation -framework CoreServices
	strip boggle-static
	upx --best --brute boggle-static
	zip -9 boggle.zip boggle-static words.dat

here:
	rm -f *.o
	make all CFLAGS="$(shell pkg-config --cflags ncurses glib-2.0)" LDLIBS="$(shell pkg-config --libs ncurses glib-2.0)"
	$(CC) -O2 -o boggle -static-libgcc ui.o check.o board.o dict.o $(shell pkg-config --libs ncurses glib-2.0)
	strip boggle
