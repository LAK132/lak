#! /bin/sh
rm -rf build
case $1 in
  *)
    cross_args=
  ;;
esac
case $1 in
  clang)
    shift
    CC=clang CXX=clang++ CC_FOR_BUILD=clang CXX_FOR_BUILD=clang++ meson setup build $cross_args $@ || exit 1
  ;;

  gcc)
    shift
    CC=gcc CXX=g++ CC_FOR_BUILD=gcc CXX_FOR_BUILD=g++ meson setup build $cross_args $@ || exit 1
  ;;

  msvc)
    shift
    meson setup build --vsenv $cross_args $@ || exit 1
  ;;

  auto)
    shift
    meson setup build $cross_args $@ || exit 1
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
