if "%DESTDIR%"=="" (
if not exist "%MESON_INSTALL_PREFIX%\bin" mkdir "%MESON_INSTALL_PREFIX%\bin"
move "%MESON_INSTALL_PREFIX%\bin\vulkan.dll" "%MESON_INSTALL_PREFIX%\bin\vulkan-1.dll"
) else (
if not exist "%MESON_INSTALL_PREFIX%\bin" mkdir "%DESTDIR%\%MESON_INSTALL_PREFIX%\bin"
move "%DESTDIR%\%MESON_INSTALL_PREFIX%\bin\vulkan.dll" "%DESTDIR%\%MESON_INSTALL_PREFIX%\bin\vulkan-1.dll"
)
