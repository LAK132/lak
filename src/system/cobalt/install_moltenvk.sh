#!/bin/sh

mkdir -p "${DESTDIR}/${MESON_INSTALL_PREFIX}/share/vulkan/icd.d"
cp "${MESON_BUILD_ROOT}/subprojects/Cobalt/__CMake_build/_deps/vulkan-sdk-install/macOS/share/vulkan/icd.d/MoltenVK_icd.json" "${DESTDIR}/${MESON_INSTALL_PREFIX}/share/vulkan/icd.d/"

mkdir -p "${DESTDIR}/${MESON_INSTALL_PREFIX}/lib"
cp "${MESON_BUILD_ROOT}/subprojects/Cobalt/__CMake_build/_deps/vulkan-sdk-install/macOS/lib/libMoltenVK.dylib" "${DESTDIR}/${MESON_INSTALL_PREFIX}/lib/"
