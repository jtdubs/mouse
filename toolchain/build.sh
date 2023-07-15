#!/bin/bash

set -e
set -x

BINUTILS_TAG=binutils-2_40
GCC_TAG=releases/gcc-13.1.0
AVRLIBC_TAG=avr-libc-2_1_0-release
AVRDUDE_TAG=v7.1

if [ ! -d binutils-gdb ]; then
  git clone git://sourceware.org/git/binutils-gdb.git
fi

if [ ! -d gcc ]; then
  git clone git://gcc.gnu.org/git/gcc.git
fi

if [ ! -d avr-libc ]; then
  git clone git@github.com:avrdudes/avr-libc
fi

if [ ! -d avrdude ]; then
  git clone git@github.com:avrdudes/avrdude
fi

if [ ! -e avrxmega3-v12.diff.xz ]; then
  curl -L https://savannah.nongnu.org/patch/download.php?file_id=52334 -o avrxmega3-v12.diff.xz
fi

pushd binutils-gdb
  if [ "$(git describe --tags)" != "$BINUTILS_TAG" ]; then
    git reset --hard
    git clean -dfx
    git fetch --tags
    git checkout $BINUTILS_TAG
    mkdir build
    pushd build
      ../configure \
        --prefix=/opt/avr-toolchain \
        --target=avr \
        --disable-nls \
        --disable-werror \
        --build=`../config.guess`
      mkdir -p gas/doc # workaround build bug.... :p
      make -j $(nproc)
      sudo make install-strip
    popd
    rm -Rf build
  fi
popd

export PATH=/opt/avr-toolchain/bin:$PATH

pushd gcc
  if [ "$(git describe --tags)" != "$GCC_TAG" ]; then
    git reset --hard
    git clean -dfx
    git fetch --tags
    git checkout $GCC_TAG
    sh ./contrib/download_prerequisites
    mkdir build
    pushd build
      ../configure \
        --prefix=/opt/avr-toolchain \
        --libdir=/opt/avr-toolchain/lib \
        --target=avr \
        --enable-languages=c,c++ \
        --enable-fixed-point \
        --disable-nls \
        --disable-shared \
        --disable-libssp \
        --disable-libada \
        --with-dwarf2 \
        --with-avrlibc=yes \
        --enable-plugin \
        --build=`../config.guess`
      make -j $(nproc)
      sudo make install-strip
    popd
    rm -Rf build
  fi
popd

pushd avr-libc
  if [ "$(git describe --tags)" != "$AVRLIBC_TAG" ]; then
    git reset --hard
    git clean -dfx
    git fetch --tags
    git checkout $AVRLIBC_TAG
    xzcat ../avrxmega3-v12.diff.xz | patch -p0
    ./bootstrap
    mkdir build
    pushd build
      ../configure \
        --prefix=/opt/avr-toolchain \
        --host=avr \
        --with-debug-info=dwarf-4 \
        --build=`../config.guess`
      make -j $(nproc)
      sudo --preserve-env=PATH make install
    popd
    rm -Rf build
  fi
popd

pushd avrdude
  if [ "$(git describe --tags)" != "$AVRDUDE_TAG" ]; then
    git reset --hard
    git clean -dfx
    git fetch --tags
    git checkout $AVRDUDE_TAG
    mkdir build
    pushd build
      cmake -D CMAKE_INSTALL_PREFIX=/opt/avr-toolchain -D CMAKE_BUILD_TYPE=RelWithDebInfo ..
      cmake --build .
      sudo cmake --build . --target install
    popd
    rm -Rf build
  fi
popd
