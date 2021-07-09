@echo off
SetLocal EnableDelayedExpansion

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

rmdir /s /q build
meson setup build || exit 1
