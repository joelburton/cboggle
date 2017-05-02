cboggle
=======

A straightforward, ncurses-based boggle game.

Uses an interesting list-based DAWG as a nice compromise of
memory and speed -- the entire list of legal words fits in
a little less than half a megabyte, yet a boggle board can
be evaluated and scored about a tenth of a millisecond on
reasonable 2017 laptops.

To compile this, you'll need:

- a C99-or-better C compiler (OSX and Linux have by default)

- ncurses-dev (OSX and Linux often have this; can install for Linux via
  your package manager)

- glibc-dev (can install for OSX via MacPorts or for Linux via your
  package manager)
