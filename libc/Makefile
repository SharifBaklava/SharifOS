DEFAULT_HOST != ../scripts/default-host.sh
HOST ?= DEFAULT_HOST
HOSTARCH != ../scripts/target-triplet-to-arch.sh $(HOST)

CFLAGS ?= -O2 -g
CPPFLAGS ?=
LDFLAGS ?=
LIBS ?=

DESTDIR ?=
PREFIX ?= /usr/local
EXEC_PREFIX ?= $(PREFIX)
INCLUDEDIR ?= $(PREFIX)/include
LIBDIR ?= $(EXEC_PREFIX)/lib

CFLAGS := $(CFLAGS) -ffreestanding -Wall -Wextra
CPPFLAGS := $(CPPFLAGS) -D__is_libc -Iinclude
LIBK_CFLAGS := $(CFLAGS)
LIBK_CPPFLAGS := $(CPPFLAGS) -D__is_libk

ARCHDIR = arch/$(HOSTARCH)
LIBCBUILDDIR := $(BUILDDIR)/libc

include $(ARCHDIR)/make.config

CFLAGS := $(CFLAGS) $(ARCH_CFLAGS)
CPPFLAGS := $(CPPFLAGS) $(ARCH_CPPFLAGS)
LIBK_CFLAGS := $(LIBK_CFLAGS) $(KERNEL_ARCH_CFLAGS)
LIBK_CPPFLAGS := $(LIBK_CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)

# Source object names (relative to source tree)
FREEOBJS_SRC := \
	$(ARCH_FREEOBJS) \
	stdio/printf.o \
	stdio/putchar.o \
	stdio/puts.o \
	stdlib/abort.o \
	string/memcmp.o \
	string/memcpy.o \
	string/memmove.o \
	string/memset.o \
	string/strlen.o

HOSTEDOBJS_SRC := \
	$(ARCH_HOSTEDOBJS)

OBJS := $(addprefix $(LIBCBUILDDIR)/, $(FREEOBJS_SRC) $(HOSTEDOBJS_SRC))
LIBK_OBJS := $(patsubst %.o, %.libk.o, $(addprefix $(LIBCBUILDDIR)/, $(FREEOBJS_SRC)))

BINARIES := $(addprefix $(LIBCBUILDDIR)/, libk.a)

.PHONY: all clean install install-headers install-libs
.SUFFIXES: .o .libk.o .c .S .cpp

all: $(BINARIES)

$(LIBCBUILDDIR)/libc.a: $(OBJS)
	mkdir -p $(LIBCBUILDDIR)
	$(AR) rcs $@ $(OBJS)

$(LIBCBUILDDIR)/libk.a: $(LIBK_OBJS)
	mkdir -p $(LIBCBUILDDIR)
	$(AR) rcs $@ $(LIBK_OBJS)

# Object rules
$(LIBCBUILDDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

$(LIBCBUILDDIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) -MD -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

$(LIBCBUILDDIR)/%.o: %.S
	mkdir -p $(dir $@)
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

$(LIBCBUILDDIR)/%.libk.o: %.c
	mkdir -p $(dir $@)
	$(CC) -MD -c $< -o $@ -std=gnu11 $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

$(LIBCBUILDDIR)/%.libk.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) -MD -c $< -o $@ -std=gnu11 $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

$(LIBCBUILDDIR)/%.libk.o: %.S
	mkdir -p $(dir $@)
	$(CC) -MD -c $< -o $@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

clean:
	rm -f $(BINARIES) *.a
	rm -f $(OBJS) $(LIBK_OBJS)
	rm -f $(OBJS:.o=.d) $(LIBK_OBJS:.o=.d)

install: install-headers install-libs

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

install-libs: $(BINARIES)
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp $(BINARIES) $(DESTDIR)$(LIBDIR)

-include $(OBJS:.o=.d)
-include $(LIBK_OBJS:.o=.d)