# SharifOS
![SharifOS Logo](resources/images/logo.png)
## Build Cross-Compiler Environment

```bash
make build-env


```

## Build Kernel & ISO
```bash
make run-env
make clean
# First build may fail due to setup — run twice if needed
make
```
## Run
```bash
make run
```

## Debug
```bash
make debug
gdb ./kernel/sharifos.kernel
```

## Documentation
```bash
make docs
```

## Clean Build Artifacts
```bash
make clean
```
## Optional Scripts
```bash
./build.sh   # Builds kernel and headers (wrapper around `make`)
./iso.sh     # Creates ISO image (wrapper around `make iso`)
```