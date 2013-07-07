# Hi and welcome to the "schizophrenia Makefile". This Makefile is for compiling
# schizophrenia on unix based systems or Windows with Mingw32/Cygwin (not tested)

# -DDEBUG shows some debug informations => slower and may be confusing

# === Now some globale variables. These are the default values for compilation on
# === a Linux machine. The other targets then change, where their have differences

# DYNAMIC says, which libraries will be linked dynamicly. Most of the time these
# are all used libraries, but some systems also need static linking, too. Because
# as default no library is linked static, STATIC is not defined yet.
DYNAMIC = -lSDL_ttf -lSDL_image -lSDL -lm

# CFLAGS defines some globale flags for gcc. Even on the gp2x with only 16 KB
# CPU Cache, -O3 is faster than -Os. So most things you don't have to change
CFLAGS = -O3 -fsingle-precision-constant -fPIC

# GENERAL_TWEAKS are some flags for gcc, which should make the compilation
# faster, but some of them are just poinsoness snake oil - they may help a bit,
# but could also kill you. ^^
GENERAL_TWEAKS = -ffast-math -DDEBUG

# The default Compiler is gcc with debug symbols and a X86CPU (that's a define
# for sparrow or your game, not used by gcc.
CPP = gcc -g -march=native -DX86CPU $(GENERAL_TWEAKS)

# SDL sets some SDL options with the program "sdl-config".
SDL = `sdl-config --cflags`

SPARROW_FOLDER = ../sparrow3d

ifdef TARGET
include $(SPARROW_FOLDER)/target-files/$(TARGET).mk
BUILD = ./build/$(TARGET)/schizophrenia
SPARROW_LIB = $(SPARROW_FOLDER)/build/$(TARGET)/sparrow3d
else
TARGET = "Default (change with make TARGET=otherTarget. See All targets with make targets)"
BUILD = ./testing
SPARROW_LIB = $(SPARROW_FOLDER)
endif
LIB += -L$(SPARROW_LIB)
INCLUDE += -I$(SPARROW_FOLDER)
DYNAMIC += -lsparrow3d -lsparrowSound

# Where to put the executables to?

all: schizophrenia
	@echo "=== Built for Target "$(TARGET)" ==="
	
targets:
	@echo "The targets are the same like for sparrow3d. :P"

schizophrenia: schizophrenia.c level.o physics.o feedback.o settings.o makeBuildDir
	cp $(SPARROW_LIB)/libsparrow3d.so $(BUILD)
	cp $(SPARROW_LIB)/libsparrowSound.so $(BUILD)
	$(CPP) $(CFLAGS) schizophrenia.c level.o physics.o feedback.o settings.o $(SDL) $(INCLUDE) $(LIB) $(STATIC) $(DYNAMIC) -o $(BUILD)/schizophrenia

makeBuildDir:
	 @if [ ! -d $(BUILD:/schizophrenia=/) ]; then mkdir $(BUILD:/schizophrenia=/);fi
	 @if [ ! -d $(BUILD) ]; then mkdir $(BUILD);fi

level.o: level.c level.h
	$(CPP) $(CFLAGS) -fPIC -c level.c $(SDL) $(INCLUDE)

physics.o: physics.c physics.h
	$(CPP) $(CFLAGS) -fPIC -c physics.c $(SDL) $(INCLUDE)

feedback.o: feedback.c feedback.h
	$(CPP) $(CFLAGS) -fPIC -c feedback.c $(SDL) $(INCLUDE)

settings.o: settings.c settings.h
	$(CPP) $(CFLAGS) -fPIC -c settings.c $(SDL) $(INCLUDE)

clean:
	rm -f *.o
	rm -f $(BUILD)/schizophrenia
