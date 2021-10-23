# Credit: https://gist.github.com/4d6178/8121960
# project name which will be used as a name of the resulting executable
PROJECT := chip8-emu

SRCEXT := .c
HEXT := .h
# some common directories
IDIR := include
SDIR := src
ODIR := $(SDIR)/obj
LDIR := lib
BINDIR := bin

# compiler to use
CC = clang

# libraries to link with
LIBS=$(shell pkg-config --libs sdl2)

# compiter flags
CFLAGS = -Wall -Wextra -Werror -pedantic -I$(IDIR) $(shell pkg-config --cflags sdl2)

# linker flags
LDFLAGS := $(LIBS)

# generate list of object files corresponding to source files
OBJECTS = $(patsubst $(SDIR)/%$(SRCEXT),$(ODIR)/%.o,$(wildcard $(SDIR)/*$(SRCEXT)))

# mode-specific flags

# ensure that all required directories exist and build executable
.PHONY: build
build: compileflags directories executable

.PHONY: debug
debug: CFLAGS += -g -O0 -DDEBUG
debug: LDFLAGS += -g
debug: build

.PHONY: release
release: CFLAGS += -O2
release: build

.PHONY: all
all: debug release

# rule for object files
$(ODIR)/%.o: $(SDIR)/%$(SRCEXT)
	$(CC) $(CFLAGS) -c -o $@ $<

# rule for the resulting file
$(BINDIR)/$(PROJECT): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

executable: $(BINDIR)/$(PROJECT)

.PHONY: clean
clean:
	@rm -f $(OBJECTS)

.PHONY: directories
directories:
	@mkdir -p $(ODIR)
	@mkdir -p $(BINDIR)

compileflags:
	$(shell echo $(LDFLAGS) $(CFLAGS) | tr ' ' '\n' > compile_flags.txt)