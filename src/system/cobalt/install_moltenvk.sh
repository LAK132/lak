#!/bin/sh

mkdir -p "${MESON_INSTALL_DESTDIR_PREFIX}/share/vulkan/icd.d"
cp "${MESON_BUILD_ROOT}/subprojects/Cobalt/__CMake_build/_deps/vulkan-sdk-install/macOS/share/vulkan/icd.d/MoltenVK_icd.json" "${MESON_INSTALL_DESTDIR_PREFIX}/share/vulkan/icd.d/"

mkdir -p "${MESON_INSTALL_DESTDIR_PREFIX}/lib"
cp "${MESON_BUILD_ROOT}/subprojects/Cobalt/__CMake_build/_deps/vulkan-sdk-install/macOS/lib/libMoltenVK.dylib" "${MESON_INSTALL_DESTDIR_PREFIX}/lib/"
