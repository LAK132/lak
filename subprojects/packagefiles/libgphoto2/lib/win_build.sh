export MSYSTEM=UCRT64 && /usr/bin/bash --login -c "\`cygpath -u '$1'\`/lib/build.sh \"\`cygpath -u '$1'\`\" \"\`cygpath -u '$2'\`\" && cd \"\`cygpath -u '$2'\`/lib\" && cp ../bin/libgphoto2*.dll . && gendef libgphoto2*.dll && cp /ucrt64/bin/libiconv-*.dll . && cp /ucrt64/bin/libintl-*.dll . && cp /ucrt64/bin/libltdl-*.dll . && cp /ucrt64/bin/libsystre-*.dll . && cp /ucrt64/bin/libtre-*.dll . || exit 1" || exit 1