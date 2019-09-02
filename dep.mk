include $(RACK_DIR)/arch.mk


luajit = luajit/src/libluajit.a
lua = dep/lua/src/liblua.a


# On OSX main executable need to be linked with these flags: `-pagezero_size 10000 -image_base 100000000`.
ifdef ARCH_MAC
	LUA_IMPL ?= lua
else
	LUA_IMPL ?= luajit
endif

FLAGS += -I./$(dir $($(LUA_IMPL)))
OBJECTS := $($(LUA_IMPL)) $(OBJECTS)
DEPS += $($(LUA_IMPL))


ifneq (, $(findstring -clang, $(CC)))
	LUA_CROSS ?= $(subst -clang,,$(CC))-
	LUA_CC ?= clang
	LUAJIT_HOST_CC ?= clang
else ifneq (, $(findstring -gcc, $(CC)))
	LUA_CROSS ?= $(subst -gcc,,$(CC))-
	LUA_CC ?= gcc
	LUAJIT_HOST_CC ?= gcc
else
	LUA_CROSS ?=
	LUA_CC ?= $(CC)
	LUAJIT_HOST_CC ?= $(CC)
endif


LUA_CFLAGS = -O3 -march=nocona -funsafe-math-optimizations -fomit-frame-pointer -fno-stack-protector -fPIC
LUAJIT_CFLAGS =
LUASTD_CFLAGS = -Wall -DLUA_COMPAT_5_2

ifdef ARCH_LIN
	LUAJIT_TARGET_SYS ?= Linux
	LUASTD_CFLAGS += -DLUA_USE_LINUX
endif
ifdef ARCH_MAC
	export MACOSX_DEPLOYMENT_TARGET=10.7
	LUAJIT_TARGET_SYS ?= Darwin
	LUASTD_CFLAGS += -DLUA_USE_MACOSX
endif
ifdef ARCH_WIN
	LUAJIT_TARGET_SYS ?= Windows
endif


$(luajit):
	$(MAKE) -C luajit/src Q= E="@:" BUILDMODE=static CCOPT= CFLAGS="$(LUA_CFLAGS) $(LUAJIT_CFLAGS)" \
	HOST_CC="$(LUAJIT_HOST_CC)" CC="$(LUA_CC)" CROSS="$(LUA_CROSS)" TARGET_SYS="$(LUAJIT_TARGET_SYS)" libluajit.a


dep/lua:
	$(WGET) https://www.lua.org/ftp/lua-5.3.5.tar.gz -P dep
	$(SHA256) dep/lua-5.3.5.tar.gz 0c2eed3f960446e1a3e4b9a1ca2f3ff893b6ce41942cf54d5dd59ab4b3b058ac
	$(UNTAR) dep/lua-5.3.5.tar.gz -C dep
	mv dep/lua-5.3.5 dep/lua
	rm dep/lua-5.3.5.tar.gz

$(lua): dep/lua
	$(MAKE) -C dep/lua/src CC="$(LUA_CROSS)$(LUA_CC)" AR="$(LUA_CROSS)ar rcus" RANLIB="@:" \
	CFLAGS="$(LUA_CFLAGS) $(LUASTD_CFLAGS)" a

