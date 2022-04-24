
# Preprocessor defs
# ggdb / g             	-> enable debugging
# DGAMMA_CORRECTION			-> Turn gamma correction on or off. 
# DFAST_EXIT						-> Compile with fast thread exiting on, seems to lower performance a bit
defines := -DGAMMA_CORRECTION -DFAST_EXIT -Ofast -ffloat-store -march=native -frename-registers -funroll-loops -fopenmp 


# Define custom functions
rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
platformpth = $(subst /,$(PATHSEP),$1)

# Set global macros
buildDir := bin
executable := Raytracer
target := $(buildDir)/$(executable)
sources := $(call rwildcard,src/,*.cpp)
sources += $(wildcard vendor/rlImGui/*.cpp)
sources += $(wildcard vendor/rlImGui/imgui/*.cpp)
objects := $(patsubst src/%, $(buildDir)/%, $(patsubst %.cpp, %.o, $(sources)))
depends := $(patsubst %.o, %.d, $(objects))

# Added -ggdb for enabling debugging, note that it causes less performance
compileFlags := -std=c++17 -I C:\raylib\raylib\src 
compileFlags += $(defines)

linkFlags = -L lib/$(platform) -l raylib

ifdef MACRO_DEFS
    macroDefines := -D $(MACRO_DEFS)
endif

# Check for Windows
ifeq ($(OS), Windows_NT)
	# Set Windows macros
	platform := Windows
	CXX ?= g++
	linkFlags += -Wl,--allow-multiple-definition -pthread -lopengl32 -lgdi32 -lwinmm -mwindows -static -static-libgcc -static-libstdc++
	libGenDir := src
	THEN := &&
	PATHSEP := \$(BLANK)
	MKDIR := -mkdir
	RM := -del /q
	COPY = -robocopy "$(call platformpth,$1)" "$(call platformpth,$2)" $3
else
	# Check for MacOS/Linux
	UNAMEOS := $(shell uname)
	ifeq ($(UNAMEOS), Linux)
		# Set Linux macros
		platform := Linux
		CXX ?= g++
# Note: Must use -L/path/to/lib for static linking
# https://stackoverflow.com/questions/9078513/how-do-you-properly-link-to-a-static-library-using-g
		linkFlags += -l GL -l m -l pthread -l dl -l rt -l X11 -L/lib/Linux
	endif
	ifeq ($(UNAMEOS), Darwin)
		# Set macOS macros
		platform := macOS
		CXX ?= clang++
		linkFlags += -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
		libGenDir := src
	endif

	# Set UNIX macros
	THEN := ;
	PATHSEP := /
	MKDIR := mkdir -p
	RM := rm -rf
	COPY = cp $1$(PATHSEP)$3 $2
endif

# Lists phony targets for Makefile
.PHONY: all setup submodules execute clean

# Default target, compiles, executes and cleans
all: $(target) execute clean

# Just builds 
build: $(target)

# Sets up the project for compiling, generates includes and libs
setup: include lib

# Pull and update the the build submodules
submodules:
	git submodule update --init --recursive

# Copy the relevant header files into includes
include: submodules
	$(MKDIR) $(call platformpth, ./include)
	$(call COPY,vendor/raylib-cpp/vendor/raylib/src,./include,raylib.h)
	$(call COPY,vendor/raylib-cpp/vendor/raylib/src,./include,raymath.h)
	$(call COPY,vendor/raylib-cpp/include,./include,*.hpp)

# Build the raylib static library file and copy it into lib
lib: submodules
	cd vendor/raylib-cpp/vendor/raylib/src $(THEN) "$(MAKE)" PLATFORM=PLATFORM_DESKTOP
	$(MKDIR) $(call platformpth, lib/$(platform))
	$(call COPY,vendor/raylib-cpp/vendor/raylib/$(libGenDir),lib/$(platform),libraylib.a)

# Link the program and create the executable
$(target): $(objects)
	$(CXX) $(objects) -o $(target) $(linkFlags)

# Add all rules from dependency files
-include $(depends)

# Compile objects to the build directory
$(buildDir)/%.o: src/%.cpp Makefile
	$(MKDIR) $(call platformpth, $(@D))
	$(CXX)  -MMD -MP -c $(compileFlags) $< -o $@ $(macroDefines)

# Run the executable
execute:
	$(target) $(ARGS)

# Clean up all relevant files
clean:
	$(RM) $(call platformpth, $(buildDir)/*)
