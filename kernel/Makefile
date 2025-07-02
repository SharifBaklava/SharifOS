DEFAULT_HOST != ../scripts/default-host.sh
HOST ?= DEFAULT_HOST
HOSTARCH != ../scripts/target-triplet-to-arch.sh $(HOST)

CFLAGS ?= -O2 -g -fstack-protector
CPPFLAGS ?=
LDFLAGS ?=
LIBS ?=

DESTDIR ?=
PREFIX ?= /usr/local
EXEC_PREFIX ?= $(PREFIX)
BOOTDIR ?= $(EXEC_PREFIX)/boot
INCLUDEDIR ?= $(PREFIX)/include

CFLAGS := $(CFLAGS) -ffreestanding -Wall -Wextra
CPPFLAGS := $(CPPFLAGS) -D__is_kernel -Iinclude
LIBS := $(LIBS) -nostdlib -lk -lgcc

ARCHDIR = arch/$(HOSTARCH)

include $(ARCHDIR)/make.config

CFLAGS := $(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPPFLAGS := $(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS := $(LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
LIBS := $(LIBS) $(KERNEL_ARCH_LIBS)

KBUILDDIR = $(BUILDDIR)/kernel
KERNEL_OBJS = \
	$(addprefix $(KBUILDDIR)/, $(KERNEL_ARCH_OBJS)) \
	$(KBUILDDIR)/kernel/kernel.o \
	$(KBUILDDIR)/$(ARCHDIR)/get_ram_size.o


OBJS = \
	$(KBUILDDIR)/$(ARCHDIR)/crti.o \
	$(KBUILDDIR)/$(ARCHDIR)/crtbegin.o \
	$(KERNEL_OBJS) \
	$(KBUILDDIR)/$(ARCHDIR)/crtend.o \
	$(KBUILDDIR)/$(ARCHDIR)/crtn.o \
	$(KBUILDDIR)/$(ARCHDIR)/get_ram_size.o

LINK_LIST = \
	$(LDFLAGS) \
	$(KBUILDDIR)/$(ARCHDIR)/crti.o \
	$(KBUILDDIR)/$(ARCHDIR)/crtbegin.o \
	$(KERNEL_OBJS) \
	$(LIBS) \
	$(KBUILDDIR)/$(ARCHDIR)/crtend.o \
	$(KBUILDDIR)/$(ARCHDIR)/crtn.o


.PHONY: all clean install install-headers install-kernel
.SUFFIXES: .o .cpp .s .c

all: $(KBUILDDIR)/sharifos.kernel

$(KBUILDDIR)/sharifos.kernel: $(OBJS) $(ARCHDIR)/linker.ld
	mkdir -p $(KBUILDDIR)
	$(CXX) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(LINK_LIST)

# Compile rules
$(KBUILDDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

$(KBUILDDIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	echo "Compiling $<"
	$(CXX) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

$(KBUILDDIR)/%.o: %.s
	mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

# crtbegin/crtend copy from compiler runtime
$(KBUILDDIR)/$(ARCHDIR)/crtbegin.o $(KBUILDDIR)/$(ARCHDIR)/crtend.o:
	mkdir -p $(dir $@)
	OBJ=`$(CXX) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

clean:
	rm -f $(KBUILDDIR)/sharifos.kernel
	rm -f $(OBJS)
	rm -f $(OBJS:.o=.d)

install: install-headers install-kernel

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

install-kernel: $(KBUILDDIR)/sharifos.kernel
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp $(KBUILDDIR)/sharifos.kernel $(DESTDIR)$(BOOTDIR)

# Include dependency files
-include $(OBJS:.o=.d)
