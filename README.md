# Chip-84 SDL Edition

Chip-84 is a CHIP-8 and SCHIP-8 interpreter originally written for the TI-84 Plus CE calculator and has been ported to many different systems.  This is the SDL port.

## Building

### Linux
Clone the repository and install its dependencies:

```bash
sudo apt-get install libgtk-3-dev libsdl2-dev libsdl2-ttf-dev
```
Then run `make`:

```bash
make
```

### Windows
[Install and setup MSYS2](https://github.com/orlp/dev-on-windows/wiki/Installing-GCC--&-MSYS2) if you haven't already.
Clone the repository and install its dependencies using MSYS2:

```bash
pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_ttf
```

Then run `make` using MinGW:

```bash
make
```

## Usage

Ctrl+O - Open a ROM

### Command-Line Arguments
`-h, --help` - Display available command line arguments.\
`-n, --nogui` - Start Chip-84 without UI elements, leaving only the display shown.\
`-o, --open [path]` - Directly load a ROM upon launch.\
`-c, --cpf [number]` - Start Chip-84 with an initial cycles per frame value.\
`-s, --screenscale [number]` - Set the scale of the display.

## License
[MIT](https://choosealicense.com/licenses/mit/)