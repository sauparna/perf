# This Makefile is set up to build the everybit library, as well as the test
# harness, in one of two modes.  In debug mode, the C compiler does virtually
# no optimizations (-O0) and embeds a bunch of debugging symbols to make work
# with GDB easier.  In release mode, the compiler will do every optimization it
# can (-O3) and keep debugging symbols out; this should decrease the size of
# the final binary and make it perform better.
#
# To compile in debug mode, type "make DEBUG=1".  To to compile in release
# mode, type "make DEBUG=0" or simply "make".
#
# If everything gets wacky and you need a sane place to start from, you can
# type "make clean", which will remove all compiled code.
#
# This code is portable to compilers other than icc--indeed, it should compile
# under any compiler which implements the C99 standard.  You can specify a
# different compiler--e.g., GCC--by passing CC=whatever on the command line.
# If you use a compiler whose option syntax is not GCC-compatible (e.g.,
# clang), you may need to specify CFLAGS and LDFLAGS explicitly as well.
#
# If you want to do something wacky with your compiler flags--like enabling
# debug symbols but keeping optimizations on--you can specify CFLAGS or LDFLAGS
# on the command line.  If you want to use a predefined mode but augment the
# predefined CFLAGS or LDFLAGS, you can specify EXTRA_CFLAGS or EXTRA_LDFLAGS
# on the command line.

# List all of your source files here (but not your headers), separated by
# spaces.  You'll have to add to this list every time you create a new
# source file.
SRC := 	bitarray.c 	\
		ktiming.c	\
		main.c		\
		tests.c

# Set the name of your binary.  Change it if you like.
PRODUCT := everybit

################################################################################
# These configuration options change how your code (listed above) is compiled
# every time you type "make".  You may have to change these values to complete
# some assignments; you should also feel free to experiment with them.
################################################################################

# This option sets which compiler your code will be compiled with.  Likely
# choices are icc, icpc, gcc, g++, clang
CC := clang

# These flags will be applied to your code any time it is built.
# We need _POSIX_C_SOURCE >= 2 to use getopt
CFLAGS := -std=c99 -Wall -D_POSIX_C_SOURCE=200112L

# These flags are applied only if you build your code with "make DEBUG=1".  -g
# generates debugging symbols, -DDEBUG defines the preprocessor symbol "DEBUG"
# (so that you can use "#ifdef DEBUG" in your code), and -O0 disables compiler
# optimizations, so that the binary generated more directly corresponds to your
# source code.
CFLAGS_DEBUG := -O0 -g -DDEBUG

# In the release version, we ask for many optimizations; -O3 sets the
# optimization level to three.  -DNDEBUG defines the NDEBUG macro,
# which disables assertion checks.
CFLGAS_RELEASE := -O3 -DNDEBUG

# These flags are used to invoke Clang's address sanitizer. To get a
# reasonable performance add -O1 or higher. To get nicer stack traces
# in error messages add -fno-omit-frame-pointer. To get perfect stack
# traces you may need to disable inlining (just use -O1) and tail call
# elimination (-fno-optimize-sibling-calls).
CFLAGS_ASAN := -O1 -g -DDEBUG -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls

# Instrument for code coverage
CFLAGS_GCOV := -O0 -g -DDEBUG -fprofile-arcs -ftest-coverage

# These flags are applied when linking object files together into your binary.
# If you need to link against libraries, add the appropriate flags here.
LDFLAGS := -flto -fuse-ld=lld -lm

# We need to link against the timing library for whatever OS we're on.
PLATFORM = $(shell uname)
ifeq ($(PLATFORM),Linux)
	LDFLAGS += -lrt
else ifeq ($(PLATFORM),Darwin)
	LDFLAGS += -arch x86_64 -framework CoreServices
endif

################################################################################
# You probably won't need to change anything below this line, but if you're
# curious about how makefiles work, or if you'd like to customize the behavior
# of your makefile, go ahead and take a peek.
################################################################################

# You shouldn't need to touch this.  This keeps track of whether you are
# building in a release or debug configuration, and sets CFLAGS appropriately.
# (This mechanism is based on one from the original materials for 6.197 by
# Ceryen Tan and Marek Olszewski.)

# Determine which profile--debug or release--we should build against, and set
# CFLAGS appropriately.
#
# At this time, we also update the .buildmode stamp, which records if the last
# build was in debug or release mode.  This is a bit hackish--we set all C
# compiler outputs to depend on .buildmode, and then we touch .buildmode if it
# should change.  Touching .buildmode invalidates all the compiler outputs, so
# they all get built again in the correct mode.  Credit to Ceryen Tan and Marek
# Olszewski, who developed this code for 6.197 back in the day.
OLDMODE=$(shell cat .buildmode 2> /dev/null)
ifeq ($(DEBUG),1)
  CFLAGS := $(CFLAGS_DEBUG) $(CFLAGS)
  ifneq ($(OLDMODE),debug)
    $(shell echo debug > .buildmode)
  endif
else ifeq ($(ASAN),1)
  CFLAGS := $(CFLAGS_ASAN) $(CFLAGS)
  LDFLAGS := $(LDFLAGS) -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls
  ifneq ($(OLDMODE),asan)
    $(shell echo asan > .buildmode)
  endif
else ifeq ($(GCOV),1)
  CFLAGS := $(CFLAGS_GCOV) $(CFLAGS)
  LDFLAGS := $(LDFLAGS) -fprofile-arcs -ftest-coverage
  ifneq ($(OLDMODE),gcov)
    $(shell echo gcov > .buildmode)
  endif
else
  CFLAGS := $(CFLAGS_RELEASE) $(CFLAGS)
  ifneq ($(OLDMODE),nodebug)
    $(shell echo nodebug > .buildmode)
  endif
endif

# When you invoke make without an argument, make behaves as though you had
# typed "make all", and builds whatever you have listed here.  (It knows to
# pick "make all" because "all" is the first rule listed.)
all:	$(PRODUCT)

# This special "target" will remove the binary and all intermediate files.
clean::
	rm -f $(OBJ) $(PRODUCT) .buildmode \
        $(addsuffix .gcda, $(basename $(SRC))) \
        $(addsuffix .gcno, $(basename $(SRC))) \
        $(addsuffix .gcov, $(SRC) fasttime.h)

# This rule generates a list of object names.  Each of your source files (but
# not your header files) produces a single object file when it's compiled.  In
# a later step, all of those object files are linked together to produce the
# binary that you run.
OBJ = $(addsuffix .o, $(basename $(SRC)))

# These rules tell make how to automatically generate rules that build the
# appropriate object-file from each of the source files listed in SRC (above).
%.o : %.c .buildmode
	$(CC) $(CFLAGS) -c $< -o $@
%.o : %.cc .buildmode
	$(CC) $(CFLAGS) -c $< -o $@
%.o : %.cpp .buildmode
	$(CC) $(CFLAGS) -c $< -o $@

# This rule tells make that it can produce your binary by linking together all
# of the object files produced from your source files and any necessary
# libraries.
$(PRODUCT): $(OBJ) .buildmode
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

test:	$(PRODUCT)
	../test.py $(PRODUCT)

testquiet:	$(PRODUCT)
	../test.py --quiet $(PRODUCT)

.PHONY:	all clean
