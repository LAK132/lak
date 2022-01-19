# LAK

## License
This library is dual licensed under the Unlicense and MIT license. See LICENSE and UNLICENSE.
Attribution would be nice but is not required.

## Dependencies

The windowing portion of this library depends on SDL2.

The OpenGL portion of this library depends on gl3w.

To use LLVM on Windows, MSVC must be installed with the MFC and ATL components

## Compiling local build of LLVM

### Windows

To build the local copy of LLVM:
```
> setup_local_llvm.bat msvc
> compile_local_llvm.bat
```

After LLVM has been built once:
```
> setup_local_llvm.bat msvc
> compile.bat
```
