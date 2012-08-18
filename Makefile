# Hi and welcome to the "schizophrenia Makefile". This Makefile is for compiling
# schizophrenia on unix based systems or Windows with Mingw32/Cygwin (not tested)

# === First of all some defines. If your compilation is slow or instable, check
# === the defines and changes them for your Target.

# -DFAST_MULTIPLICATION enables a faster multiplication for fixed point
# arithmetics, it may look ugly on bigger resolution or may be too inaccurate.

# -DFAST_DIVISION does the same as -DFAST_MULTIPLICATION, but for division.

# -DDEBUG shows some debug informations => slower and may be confusing

# === Now some globale variables. These are the default values for compilation on
# === a Linux machine. The other targets then change, where their have differences

# DYNAMIC says, which libraries will be linked dynamicly. Most of the time these
# are all used libraries, but some systems also need static linking, too. Because
# as default no library is linked static, STATIC is not defined yet.
DYNAMIC = -lSDL_ttf -lSDL_image -lSDL -lm -lsparrow3d

# CFLAGS defines some globale flags for gcc. Even on the gp2x with only 16 KB
# CPU Cache, -O3 is faster than -Os. So most things you don't have to change
CFLAGS = -O2 -fsingle-precision-constant -fPIC

# GENERAL_TWEAKS are some flags for gcc, which should make the compilation
# faster, but some of them are just poinsoness snake oil - they may help a bit,
# but could also kill you. ^^
GENERAL_TWEAKS = -ffast-math -DDEBUG

# every device using SMALL_RESOLUTION_DEVICES in the compilation will enable
# the faster multiplication and division
SMALL_RESOLUTION_DEVICES = -DFAST_MULTIPLICATION -DFAST_DIVISION

# The default Compiler is gcc with debug symbols and a X86CPU (that's a define
# for sparrow or your game, not used by gcc.
CPP = gcc -g -march=native -DX86CPU $(GENERAL_TWEAKS)

# SDL sets some SDL options with the program "sdl-config".
SDL = `sdl-config --cflags`

# SPARROW_LIB determines, where the sparrow library is.
SPARROW_LIB = -L../sparrow3d

# SPARROW_INCLUDE determines, where the sparrow includes are
SPARROW_INCLUDE = -I../sparrow3d

# SDL_PATH determines, where the SDL includes are (necessary if SDL is in subfolders)
SDL_PATH = -I/usr/include/SDL

# INCLUDE determines, where all the other includes are
# INCLUDE = -I/usr/include

# LIB determines, where all the other libraries are
# LIB = -L/usr/lib

# Where to put the executables to?
BUILD_PATH = ./build/

# === The Targets. Set your own paths on your PC!
# == GP2X/WIZ ==
ifeq ($(TARGET),open2x)
CPP = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-gcc -DMOBILE_DEVICE -DARMCPU -DGP2X -DF100 $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
SDL = `/opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/sdl-config --cflags`
INCLUDE = -I/opt/open2x/gcc-4.1.1-glibc-2.3.6/include
LIB = -L/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib -Wl,-rpath=/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib
endif
ifeq ($(TARGET),gp2x)
CPP = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-gcc -DMOBILE_DEVICE -DARMCPU -DGP2X -DF100 $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
STATIC = -Wl,-Bstatic -lSDL -lm -Wl,-Bdynamic
DYNAMIC = -lSDL_image -lSDL_ttf -lfreetype -lpng -lz -ljpeg
SDL = `/opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/sdl-config --cflags`
INCLUDE = -I/opt/open2x/gcc-4.1.1-glibc-2.3.6/include
LIB = -L/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib -Wl,-rpath=/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib
endif
ifeq ($(TARGET),wiz)
CPP = /opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/arm-open2x-linux-gcc -DMOBILE_DEVICE -DARMCPU -DGP2X -DWIZ $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
STATIC = -Wl,-Bstatic -lpng -Wl,-Bdynamic
DYNAMIC = -lSDL_ttf -lSDL_image -lSDL -lm -lsparrow3d
SDL = `/opt/open2x/gcc-4.1.1-glibc-2.3.6/bin/sdl-config --cflags`
INCLUDE = -I/opt/open2x/gcc-4.1.1-glibc-2.3.6/include
LIB = -L/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib -Wl,-rpath=/opt/open2x/gcc-4.1.1-glibc-2.3.6/lib
endif
# == Caanoo ==
ifeq ($(TARGET),caanoo)
CPP = /opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/bin/arm-gph-linux-gnueabi-gcc -DMOBILE_DEVICE -DARMCPU -DCAANOO $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
SDL = -I/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
INCLUDE = -I/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/include
LIB = -L/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/lib -Wl,-rpath=/opt/caanoo/gcc-4.2.4-glibc-2.7-eabi/arm-gph-linux-gnueabi/sys-root/usr/lib
endif
# == Dingux ==
ifeq ($(TARGET),dingux)
CPP = /opt/opendingux-toolchain/usr/bin/mipsel-linux-gcc -DMOBILE_DEVICE -DDINGUX $(GENERAL_TWEAKS) $(SMALL_RESOLUTION_DEVICES)
SDL = -I/opt/opendingux-toolchain/usr/mipsel-unknown-linux-uclibc/sys-include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
INCLUDE = -I/opt/opendingux-toolchain/usr/mipsel-unknown-linux-uclibc/sys-include
LIB = -L/opt/opendingux-toolchain/usr/lib -Wl,-rpath=/opt/opendingux-toolchain/usr/lib
endif
# == Pandora ==
ifeq ($(TARGET),pandora)
CPP = /opt/pandora/arm-2011.03/bin/arm-none-linux-gnueabi-gcc -DMOBILE_DEVICE -DARMCPU -DPANDORA $(GENERAL_TWEAKS)
SDL = `/opt/pandora/arm-2011.03/usr/bin/sdl-config --cflags`
INCLUDE = -I/opt/pandora/arm-2011.03/usr/include
LIB = -L/opt/pandora/arm-2011.03/usr/lib -Wl,-rpath=/opt/pandora/arm-2011.03/usr/lib
endif

all: schizophrenia

targets:
	@echo "gp2x, open2x (like gp2x, but dynamic compiled => smaller), wiz caanoo, dingux, pandora, maemo5, maemo6"

schizophrenia: schizophrenia.c level.o physics.o feedback.o
	$(CPP) $(CFLAGS) schizophrenia.c level.o physics.o feedback.o $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE) $(LIB) $(SDL_LIB) $(SPARROW_LIB) $(STATIC) $(DYNAMIC) -o $(BUILD_PATH)schizophrenia

level.o: level.c level.h
	$(CPP) $(CFLAGS) -fPIC -c level.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

physics.o: physics.c physics.h
	$(CPP) $(CFLAGS) -fPIC -c physics.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

feedback.o: feedback.c feedback.h
	$(CPP) $(CFLAGS) -fPIC -c feedback.c $(SDL) $(INCLUDE) $(SDL_INCLUDE) $(SPARROW_INCLUDE)

clean:
	rm -f *.o
	rm -f $(BUILD_PATH)schizophrenia
