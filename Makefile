IDIR = include
CC = gcc
CFLAGS = -I.

BINARY = Chip-84

ODIR = obj
LDIR = lib

NFDLIB = libnfd.a
PLATLIBS = 
ifeq ($(OS), Windows_NT)
	NFDLIB = nfd.lib
	PLATLIBS = -lmingw32 -lSDL2main
endif
LDLIBS = $(PLATLIBS) -lSDL2 -lSDL2_ttf $(LDIR)/$(NFDLIB) $(shell pkg-config --cflags --libs gtk+-3.0)

_DEPS = chip8.h
DEPS = $(patsubst %, $(IDIR)/%, $(_DEPS))
_OBJ = main.o chip8.o
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))

$(ODIR)/%.o: %.c $(_DEPS)
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BINARY): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)
	
.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(BINARY) -r $(ODIR)
