@echo off
meson compile -C build compile-llvm %* || exit 1
