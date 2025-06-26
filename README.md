# SharifOs
## build cross compiler environment
```bash
cd buildenv
docker build -t sharifos-env .
cd ..
docker run -it --rm -v $(pwd):/mnt/share sharifos-env
```
## build kernel ISO
```bash
# in container!
cd /mnt/share
make clean
./build.sh # First one fails for some reason
./build.sh
./iso.sh

```
## run 
```bash
qemu-system-i386 -cdrom myos.iso -m 256M -s -S
gdb ./kernel/myos.kernel
```