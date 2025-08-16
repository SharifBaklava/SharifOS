# === Build Configuration ===

SYSTEM_HEADER_PROJECTS = libc kernel
PROJECTS = libc kernel

MAKE ?= make
HOST ?= $(shell ./scripts/default-host.sh)

AR := $(HOST)-ar
AS := $(HOST)-as
CC := $(HOST)-gcc
CXX := $(HOST)-g++

PREFIX := /usr
EXEC_PREFIX := $(PREFIX)
BOOTDIR := /boot
LIBDIR := $(EXEC_PREFIX)/lib
INCLUDEDIR := $(PREFIX)/include

BUILDDIR := $(shell pwd)/build
DISTDIR := $(shell pwd)/dist
SYSROOT := $(BUILDDIR)/sysroot
ISODIR := $(BUILDDIR)/isodir

CFLAGS := -O0 -g
CPPFLAGS := -g -O0

CC := $(CC) --sysroot=$(SYSROOT)
CXX := $(CXX) --sysroot=$(SYSROOT)

ifeq ($(findstring -elf,$(HOST)), -elf)
    CC := $(CC) -isystem=$(INCLUDEDIR)
    CXX := $(CXX) -isystem=$(INCLUDEDIR)
endif

# === Export environment variables for sub-makes ===
export MAKE HOST AR AS CC CXX PREFIX EXEC_PREFIX BOOTDIR LIBDIR INCLUDEDIR BUILDDIR DISTDIR SYSROOT CFLAGS CPPFLAGS

# === Targets ===

.PHONY: all build-env run-env clean docs build iso headers run debug

all: iso

build-env:
	cd buildenv && docker build -t sharifos-env .

run-env:
	docker run -it --rm -v $(shell pwd):/mnt/share -w /mnt/share sharifos-env

docs:
	doxygen Doxyfile

iso: build
	@echo ">> Building ISO image..."
	@if [ ! -d "$(ISODIR)/boot/grub" ]; then \
		mkdir -p "$(ISODIR)/boot/grub"; \
	fi
	@if [ ! -d "$(DISTDIR)" ]; then \
		mkdir -p "$(DISTDIR)"; \
	fi
	@cp "$(SYSROOT)/boot/sharifos.kernel" "$(ISODIR)/boot/sharifos.kernel"
	@cp grub.cfg "$(ISODIR)/boot/grub/grub.cfg"
	grub-mkrescue -o "$(DISTDIR)/sharifos.iso" "$(ISODIR)"

build: headers
	@echo ">> Building all projects..."
	@for PROJECT in $(PROJECTS); do \
		(cd $$PROJECT && DESTDIR="$(SYSROOT)" $(MAKE) install); \
	done

headers:
	@echo ">> Installing headers..."
	@if [ ! -d "$(SYSROOT)" ]; then \
		mkdir -p "$(SYSROOT)"; \
	fi
	@for PROJECT in $(SYSTEM_HEADER_PROJECTS); do \
		(cd $$PROJECT && $(MAKE) install-headers DESTDIR="$(SYSROOT)"); \
	done

clean:
	@echo ">> Cleaning build artifacts..."
	@rm -rf "$(BUILDDIR)" "$(DISTDIR)"
	@for PROJECT in $(SYSTEM_HEADER_PROJECTS); do \
		$(MAKE) -C $$PROJECT clean; \
	done

run:
	qemu-system-i386 -cdrom "$(DISTDIR)/sharifos.iso" -m 1G

debug:
	qemu-system-i386 -cdrom "$(DISTDIR)/sharifos.iso" -m 256M -s -S
