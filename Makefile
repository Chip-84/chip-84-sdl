CC = gcc
CFLAGS = -I.

BINARY = Chip-84

ODIR = obj
PLATLIBS = 
ifeq ($(OS), Windows_NT)
	PLATLIBS = -lmingw32 -lSDL2main -lcomdlg32 -lole32
endif
LDLIBS = $(PLATLIBS) -lSDL2 -lSDL2_ttf

DEPS = chip8.h
_OBJ = main.o chip8.o tinyfiledialogs.o
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BINARY): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)
	
.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(BINARY) -r $(ODIR)
