# Chip-84 SDL Edition

Chip-84 is a CHIP-8 and SCHIP-8 interpreter originally written for the TI-84 Plus CE calculator and has been ported to many different systems.  This is the SDL port.

## Building

Clone the repository and install these dependencies: `libsdl2-dev`, `libsdl2-ttf-dev`:

```bash
sudo apt-get install libsdl2-dev
sudo apt-get install libsdl2-ttf-dev
```
Then run `make`:

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