ifneq ("$(wildcard .local.mk)","")
include .local.mk
endif

# If RACK_DIR is not defined when calling the Makefile, default to two directories above
RACK_DIR ?= ../..

# FLAGS will be passed to both the C and C++ compiler
FLAGS += -I./midifile/include
CFLAGS +=
CXXFLAGS +=

# Careful about linking to shared libraries, since you can't assume much about the user's environment and library search path.
# Static libraries are fine, but they should be added to this plugin's build system.
LDFLAGS +=

# Add .cpp and .c files to the build
SOURCES += $(wildcard src/*.cpp)
SOURCES += $(filter-out midifile/src/Options.cpp, $(wildcard midifile/src/*.cpp))

include dep.mk

# Static libs
OBJECTS +=

# Dependencies
DEPS +=

# Add files to the ZIP package when running `make dist`
# The compiled plugin and "plugin.json" are automatically added.
DISTRIBUTABLES += $(wildcard LICENSE*) res


all: dep

clean-dep:
ifneq ("$(wildcard luajit/src)","")
	$(MAKE) -C luajit/src clean
endif
ifneq ("$(wildcard dep/lua/src)","")
	$(MAKE) -C dep/lua/src clean
endif

cleandep: clean-dep


# Include the VCV Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk

