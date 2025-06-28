#!/bin/sh
set -e
. ./scripts/build.sh
ISODIR=${BUILDDIR}/isodir
mkdir -p ${ISODIR}/boot/grub
mkdir -p ${DISTDIR}

cp ${SYSROOT}/boot/sharifos.kernel ${ISODIR}/boot/sharifos.kernel
cat > ${ISODIR}/boot/grub/grub.cfg << EOF
menuentry "sharifos" {
	multiboot2 /boot/sharifos.kernel
	boot
}
EOF
grub-mkrescue -o ${DISTDIR}/sharifos.iso ${ISODIR}
