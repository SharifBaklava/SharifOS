#!/bin/sh
set -e
. ./scripts/iso.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom sharifos.iso
