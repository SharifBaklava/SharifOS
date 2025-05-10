FROM ubuntu:24.04
RUN apt update && \
	apt install -y build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev  texinfo curl
WORKDIR /usr/src

ENV GCC_VERSION=13.2.0
ENV BINUTILS_VERSION=2.41
RUN curl -O https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.gz && \
	tar -xzf gcc-${GCC_VERSION}.tar.gz && \
	curl -O https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.gz && \
	tar -xzf binutils-${BINUTILS_VERSION}.tar.gz

ENV PREFIX="/root/opt/cross"
ENV TARGET=i686-elf
ENV PATH="$PREFIX/bin:$PATH"
RUN mkdir -p $PREFIX
RUN mkdir build-binutils && \
	cd build-binutils && \
	../binutils-${BINUTILS_VERSION}/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror--disable-werror \
	&& \
	make && \
	make install
RUN mkdir build-gcc && \
	cd build-gcc && \
	../gcc-${GCC_VERSION}/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers --disable-hosted-libstdcxx \
	&& \
	make all-gcc && \
	make all-target-libgcc && \
	make all-target-libstdc++-v3 && \
	make install-gcc && \
	make install-target-libgcc && \
	make install-target-libstdc++-v3

RUN apt install nasm grub-pc-bin grub-common xorriso
CMD "/bin/bash"