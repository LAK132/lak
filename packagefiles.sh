#! /bin/sh
meson subprojects packagefiles --apply $* || exit 1
