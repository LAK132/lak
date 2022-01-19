@echo off
meson compile -C build lak_test %* || exit 1
