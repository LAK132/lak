@echo off
SetLocal EnableDelayedExpansion

call msvc.bat

rmdir /s /q build
meson setup build || exit 1
