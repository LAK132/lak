@echo off
meson subprojects packagefiles --apply %* || exit /b 1
