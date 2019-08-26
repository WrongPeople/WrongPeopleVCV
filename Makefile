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
SOURCES += src/LuaModule.cpp

# Static libs
libluajit := dep/luajit/src/libluajit.a
OBJECTS += $(libluajit)

# Dependencies
DEPS += $(libluajit)

# Add files to the ZIP package when running `make dist`
# The compiled plugin and "plugin.json" are automatically added.
DISTRIBUTABLES += $(wildcard LICENSE*) res

# Include the VCV Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk


ifneq (, $(findstring -clang, $(CC)))
	LJ_HOST_CC ?= clang
	LJ_CC ?= clang
	LJ_CROSS ?= $(subst -clang,,$(CC))-
else ifneq (, $(findstring -gcc, $(CC)))
	LJ_HOST_CC ?= gcc
	LJ_CC ?= gcc
	LJ_CROSS ?= $(subst -gcc,,$(CC))-
else
	LJ_HOST_CC ?= $(CC)
	LJ_CC ?= $(CC)
	LJ_CROSS ?=
endif

ifdef ARCH_LIN
	LJ_TARGET_SYS ?= Linux
endif
ifdef ARCH_MAC
	LJ_TARGET_SYS ?= Darwin
endif
ifdef ARCH_WIN
	LJ_TARGET_SYS ?= Windows
endif

$(libluajit):
	$(MAKE) -C dep/luajit/src BUILDMODE=static CFLAGS=-fPIC MACOSX_DEPLOYMENT_TARGET=10.7 Q= E="@:" \
	HOST_CC="$(LJ_HOST_CC)" CC="$(LJ_CC)" CROSS="$(LJ_CROSS)" TARGET_SYS="$(LJ_TARGET_SYS)"


# It's no possible to override `clean` target without warnings.
clean-dep:
	$(MAKE) -C dep/luajit/src clean

cleanall: clean clean-dep

