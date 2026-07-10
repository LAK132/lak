if "%DESTDIR%"=="" (
if not exist "%MESON_INSTALL_PREFIX%\bin" mkdir "%MESON_INSTALL_PREFIX%\bin"
copy "%MESON_SOURCE_ROOT%\subprojects\Cobalt\External\Cache\DirectXShaderCompilerOfficial\bin\x86\dxcompiler.dll" "%MESON_INSTALL_PREFIX%\bin\"
) else (
if not exist "%MESON_INSTALL_PREFIX%\bin" mkdir "%DESTDIR%\%MESON_INSTALL_PREFIX%\bin"
copy "%MESON_SOURCE_ROOT%\subprojects\Cobalt\External\Cache\DirectXShaderCompilerOfficial\bin\x86\dxcompiler.dll" "%DESTDIR%\%MESON_INSTALL_PREFIX%\bin\"
)
