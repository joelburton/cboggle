CFLAGS=-Wall -Wextra -O0 -g3 -fsanitize=address -fsanitize=undefined $(shell pkg-config --cflags ncurses glib-2.0)
LDLIBS=-fsanitize=address -fsanitize=undefined $(shell pkg-config --libs ncurses glib-2.0)

all: boggle

%.o: %.c boggle.h Makefile
	$(CC) $(CFLAGS) -c $< -o $@

checkword: check.o board.o checkword.o dict.o utils.o

boggle: ui.o check.o board.o dict.o utils.o
	$(CC) $+ -o $@ $(LDLIBS)

clean:
	rm -f *.o boggle checkword boggle-static boggle.zip

# Package up as a static file for OSX

deploy:
	rm -f *.o
	make all CFLAGS="$(shell pkg-config --cflags ncurses glib-2.0)" LDLIBS="$(shell pkg-config --libs ncurses glib-2.0)"
	$(CC) -O2 -o boggle-static ui.o check.o board.o dict.o utils.o \
	    /usr/lib/libncurses.5.4.dylib /opt/local/lib/libglib-2.0.a \
		/opt/local/lib/libintl.a /opt/local/lib/libiconv.a \
		-framework CoreFoundation -framework CoreServices
	strip boggle-static
	upx --best --brute boggle-static
	zip -9 boggle.zip boggle-static words.dat

# Package up as an in-place for Linux

here:
	rm -f *.o
	make all CFLAGS="$(shell pkg-config --cflags ncurses glib-2.0)" LDLIBS="$(shell pkg-config --libs ncurses glib-2.0)"
	$(CC) -O2 -o boggle -static-libgcc ui.o check.o board.o dict.o utils.o $(shell pkg-config --libs ncurses glib-2.0)
	strip boggle
