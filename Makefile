INCLUDES = $(shell pkg-config --cflags ncurses)
CFLAGS = -Wall -Wextra -O0 -g3 $(INCLUDES)
LDLIBS = $(shell pkg-config --libs ncurses)
OBJS = ui.o check.o board.o dict.o utils.o

boggle-linux boggle-osx: CFLAGS = -Os -g0 $(INCLUDES)
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
	sudo mkdir -p /usr/local/share/boggle
	sudo cp words.dat /usr/local/share/boggle

# Package up as a static file for OSX
boggle-osx: clean $(OBJS)
	$(if $(filter Darwin,$(shell uname)),,$(error Not Mac))
	$(CC) -o $@ $(OBJS) \
	  /usr/lib/libncurses.5.4.dylib \
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

.PHONY: clean all install
