#!/bin/sh
TARGET=i686-elf
PREFIX=~/${TARGET}
BINUTILS=2.24
GCC=4.9.0
GMP=5.1.3
MPC=1.0.2
MPFR=3.1.2

HOME=$(pwd)
mkdir -p $PREFIX
cd $PREFIX

wget -c http://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS}.tar.gz
wget -c http://ftp.gnu.org/gnu/gcc/gcc-${GCC}/gcc-${GCC}.tar.gz
wget -c http://ftp.gnu.org/gnu/gmp/gmp-${GMP}.tar.gz
wget -c http://ftp.gnu.org/gnu/mpc/mpc-${MPC}.tar.gz
wget -c http://ftp.gnu.org/gnu/mpfr/mpfr-${MPFR}.tar.gz

rm -rf binutils-${BINUTILS}
rm -rf gcc-${GCC}
rm -rf build-binutils
rm -rf build-gcc
rm -rf bin include lib libexec share $TARGET

tar xzf binutils-${BINUTILS}.tar.gz

mkdir build-binutils
cd build-binutils
../binutils-${BINUTILS}/configure --target=${TARGET} --prefix=${PREFIX} --disable-nls --disable-werror
make all -j$(nproc)
make install
cd ..

tar xzf gcc-${GCC}.tar.gz
tar xzf gmp-${GMP}.tar.gz
tar xzf mpc-${MPC}.tar.gz
tar xzf mpfr-${MPFR}.tar.gz

mv gmp-${GMP} gcc-${GCC}/gmp
mv mpc-${MPC} gcc-${GCC}/mpc
mv mpfr-${MPFR} gcc-${GCC}/mpfr

mkdir build-gcc
cd build-gcc
export PATH=${PATH}:${PREFIX}/bin
../gcc-${GCC}/configure --target=${TARGET} --prefix=${PREFIX} --disable-nls --without-headers --enable-languages=c --disable-werror
make all-gcc -j$(nproc)
make all-target-libgcc -j$(nproc)
make install-gcc
make install-target-libgcc
cd ..

rm -rf build-binutils
rm -rf build-gcc
rm -rf binutils-${BINUTILS}
rm -rf gcc-${GCC}

cd $HOME
