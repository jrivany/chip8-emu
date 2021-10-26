# CHIP8 Emu

A very basic, messy and probably horrible, chip8 emulator.

Currently only setup to build on osx

What more needs to be said?

# Build:

```
make
```

# Run:

```
bin/chip8-emu [-d] <romfile>
```

The `-d` flag enables a basic step based debug mode.
Press return on the SDL window to step forward.

Check out: https://github.com/kripod/chip8-roms for some roms.

Note: as of now, because I'm lazy, the max rom size is `BUFSIZ` (1024 bytes on my machine).
