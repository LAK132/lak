@echo off
SetLocal EnableDelayedExpansion

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

cd build || exit 1
meson compile || exit 1
