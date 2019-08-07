ifneq ("$(wildcard .local.mk)","")
include .local.mk
endif

# If RACK_DIR is not defined when calling the Makefile, default to two directories above
RACK_DIR ?= ../..

# FLAGS will be passed to both the C and C++ compiler
FLAGS += -I./dep/midifile/include -I./dep/luajit/src
CFLAGS +=
CXXFLAGS +=

# Careful about linking to shared libraries, since you can't assume much about the user's environment and library search path.
# Static libraries are fine, but they should be added to this plugin's build system.
LDFLAGS +=

# Add .cpp and .c files to the build
#SOURCES += $(wildcard src/*.cpp)
SOURCES += src/plugin.cpp src/util.cpp
SOURCES += $(filter-out dep/midifile/src/Options.cpp, $(wildcard dep/midifile/src/*.cpp))

SOURCES += src/Tourette.cpp
SOURCES += src/MIDIPlayer.cpp
SOURCES += src/Lua.cpp

# Static libs
libluajit := dep/luajit/src/libluajit.a
OBJECTS += $(libluajit)

# Dependencies
DEPS += $(libluajit)

$(libluajit):
	cd dep/luajit && $(MAKE) CFLAGS="-fPIC"

# Add files to the ZIP package when running `make dist`
# The compiled plugin and "plugin.json" are automatically added.
DISTRIBUTABLES += $(wildcard LICENSE*) res

# Include the VCV Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk

