if not exist "%MESON_INSTALL_DESTDIR_PREFIX%\bin" mkdir "%MESON_INSTALL_DESTDIR_PREFIX%\bin"
copy "%MESON_SOURCE_ROOT%\subprojects\Cobalt\External\Cache\DirectXShaderCompilerOfficial\bin\arm64\dxcompiler.dll" "%MESON_INSTALL_DESTDIR_PREFIX%\bin\"
