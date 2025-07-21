# Respect externally set variables
DEFAULT_HOST != ../scripts/default-host.sh
HOST ?= DEFAULT_HOST
HOSTARCH != ../scripts/target-triplet-to-arch.sh $(HOST)

# Core flags (conditionally extended below)
CFLAGS   ?= -O2 -g
CPPFLAGS ?=
LDFLAGS  ?=
LIBS     ?=

# Install paths
DESTDIR     ?=
PREFIX      ?= /usr/local
EXEC_PREFIX ?= $(PREFIX)
INCLUDEDIR  ?= $(PREFIX)/include
LIBDIR      ?= $(EXEC_PREFIX)/lib

# Compiler flags
CFLAGS   += -ffreestanding -Wall -Wextra
CPPFLAGS += -D__is_libc -Iinclude

LIBK_CFLAGS   := $(CFLAGS)
LIBK_CPPFLAGS := $(CPPFLAGS) -D__is_libk

# Architecture-specific settings
ARCHDIR := arch/$(HOSTARCH)
LIBCBUILDDIR := $(BUILDDIR)/libc
include $(ARCHDIR)/make.config

# Append architecture flags
CFLAGS        += $(ARCH_CFLAGS)
CPPFLAGS      += $(ARCH_CPPFLAGS)
LIBK_CFLAGS   += $(KERNEL_ARCH_CFLAGS)
LIBK_CPPFLAGS += $(KERNEL_ARCH_CPPFLAGS)

# Source discovery
SRC_DIRS   := stdio stdlib string
SRC_FILES  := $(foreach dir, $(SRC_DIRS), $(shell find $(dir) -type f \( -name '*.c' -o -name '*.cpp' \)))

FREEOBJS_SRC := $(ARCH_FREEOBJS) $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(SRC_FILES)))
HOSTEDOBJS_SRC := $(ARCH_HOSTEDOBJS)

# Full object paths
OBJS      := $(addprefix $(LIBCBUILDDIR)/, $(FREEOBJS_SRC) $(HOSTEDOBJS_SRC))
LIBK_OBJS := $(patsubst %.o, %.libk.o, $(addprefix $(LIBCBUILDDIR)/, $(FREEOBJS_SRC)))

# Final binaries
BINARIES := $(addprefix $(LIBCBUILDDIR)/, libk.a)

.PHONY: all clean install install-headers install-libs
.SUFFIXES: .o .libk.o .c .S .cpp

all: $(BINARIES)

# Static libraries
$(LIBCBUILDDIR)/libc.a: $(OBJS)
	mkdir -p $(LIBCBUILDDIR)
	$(AR) rcs $@ $^

$(LIBCBUILDDIR)/libk.a: $(LIBK_OBJS)
	mkdir -p $(LIBCBUILDDIR)
	$(AR) rcs $@ $^

# Build rules
$(LIBCBUILDDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -MD -std=gnu11 -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

$(LIBCBUILDDIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

$(LIBCBUILDDIR)/%.o: %.S
	mkdir -p $(dir $@)
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

$(LIBCBUILDDIR)/%.libk.o: %.c
	mkdir -p $(dir $@)
	$(CC) -MD -std=gnu11 -c $< -o $@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

$(LIBCBUILDDIR)/%.libk.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) -MD -c $< -o $@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

$(LIBCBUILDDIR)/%.libk.o: %.S
	mkdir -p $(dir $@)
	$(CC) -MD -c $< -o $@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

# Clean
clean:
	rm -f $(BINARIES) *.a
	rm -f $(OBJS) $(LIBK_OBJS)
	rm -f $(OBJS:.o=.d) $(LIBK_OBJS:.o=.d)

# Install targets
install: install-headers install-libs

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

install-libs: $(BINARIES)
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp $(BINARIES) $(DESTDIR)$(LIBDIR)

# Include dependency files
-include $(OBJS:.o=.d)
-include $(LIBK_OBJS:.o=.d)
