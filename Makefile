CFLAGS = -Wall -Wextra -O0 -g3 $(shell pkg-config --cflags ncurses glib-2.0)
LDLIBS = $(shell pkg-config --libs ncurses glib-2.0)
OBJS = ui.o check.o board.o dict.o utils.o

boggle-linux boggle-osx: CFLAGS += -Os -g0
debug: CFLAGS += -fsanitize=address,undefined
debug: LDLIBS += -fsanitize=address,undefined

all: boggle

%.o: %.c boggle.h Makefile
	$(CC) $(CFLAGS) -c -o $@ $<

checkword: $(filter-out ui.o,$(OBJS)) checkword.o

boggle: $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDLIBS)

debug: clean $(OBJS)
		$(CC) $(OBJS) -o $@ $(LDLIBS)
		rm -f *.o

install: boggle
	sudo cp boggle /usr/local/bin

# Package up as a static file for OSX
boggle-osx: clean $(OBJS)
	$(if $(filter Darwin,$(shell uname)),,$(error Not Mac))
	$(CC) -o $@ $(OBJS) \
	    /usr/lib/libncurses.5.4.dylib /opt/local/lib/libglib-2.0.a \
			/opt/local/lib/libintl.a /opt/local/lib/libiconv.a \
			-framework CoreFoundation -framework CoreServices
	strip $@
	upx --best --brute $@
	zip -9 boggle.zip $@ words.dat
	rm -f *.o

# Package up as a static file for Linux
boggle-linux: clean $(OBJS)
	$(if $(filter Linux,$(shell uname)),,$(error Not Linux))
	$(CC) -o $@ -static-libgcc $(OBJS)
	strip $@
	rm -f *.0

clean:
		rm -f *.o boggle debug boggle-osx boggle-linux checkword boggle.zip

.PHONY: clean all
