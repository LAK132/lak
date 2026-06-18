#! /bin/sh
rm -rf build
case $1 in
  emscripten)
    shift
    cross_args=--cross-file=cross/emscripten.txt
  ;;

  clang)
    export CC=clang
    export CXX=clang++
    cross_args=
  ;;

  gcc)
    export CC=gcc
    export CXX=g++
    cross_args=
  ;;

  *)
    cross_args=
  ;;
esac
case $1 in
  clang)
    shift
    export CC_FOR_BUILD=clang
    export CXX_FOR_BUILD=clang++
    meson setup $cross_args build $@ || exit 1
  ;;

  gcc)
    shift
    export CC_FOR_BUILD=gcc
    export CXX_FOR_BUILD=g++
    meson setup $cross_args build $@ || exit 1
  ;;

  msvc)
    shift
    meson setup $cross_args build --vsenv $@ || exit 1
  ;;

  auto)
    shift
    meson setup $cross_args build $@ || exit 1
  ;;

  *)
    echo "./setup.sh <cross target> [native compiler] <setup args>"
    echo "examples:"
    echo "./setup.sh auto # let meson automatically find the compiler"
    echo "./setup.sh msvc"
    echo "./setup.sh msvc --buildtype release"
    echo "./setup.sh gcc --buildtype debug"
    echo "./setup.sh clang --buildtype debugoptimized"
    exit 1
  ;;
esac
