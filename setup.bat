@echo off
SetLocal EnableDelayedExpansion

set meson_args=

if "%~1"=="esp-idf" (
  shift
  goto esp-idf
)

:build-system
if "%~1"=="msvc" (
  set meson_args=!meson_args! --vsenv
  goto run
)

if "%~1"=="clang" (
  set CC=clang
  set CXX=clang++
  goto run
)

if "%~1"=="gcc" (
  set CC=gcc
  set CXX=g++
  goto run
)

goto usage

:esp-idf
echo [properties] > cross/esp/idf.ini
echo idf_path = '%IDF_PATH%' >> cross/esp/idf.ini
echo idf_tools_path = '%IDF_TOOLS_PATH%' >> cross/esp/idf.ini
echo [constants] >> cross/esp/idf.ini
echo idf_path = '%IDF_PATH%' >> cross/esp/idf.ini
echo idf_tools_path = '%IDF_TOOLS_PATH%' >> cross/esp/idf.ini
set meson_args=--cross-file "cross/esp/idf.ini" --cross-file "cross/esp/%~1.ini" --cross-file "cross/esp/idf-tools.ini" !meson_args!
shift
goto build-system

:run
shift
if not "%~1"=="--reconfigure" (
  rmdir /s /q build
)

:arg-loop
if "%1"=="" goto end-arg-loop
set meson_args=!meson_args! %1
shift
goto arg-loop
:end-arg-loop

meson setup build !meson_args!
goto :eof

:usage
echo setup.bat ^<cross target^> [native compiler] ^<setup args^>
echo examples:
echo setup.bat msvc
echo setup.bat esp-idf esp32-s3 msvc
echo setup.bat msvc --buildtype release
echo setup.bat gcc --buildtype debug
echo setup.bat clang --buildtype debugoptimized
exit /b 1
