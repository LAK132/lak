cd "$1" && autoreconf -is && ./configure --prefix="$2" --enable-static --enable-shared && make -j$(nproc) && make install || exit 1
