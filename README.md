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
make
```
## run 
```bash
qemu-system-i386 -cdrom myos.iso
```