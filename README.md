# LAK

## License
This library is dual licensed under the Unlicense and MIT license. See LICENSE and UNLICENSE.
Attribution would be nice but is not required.

## Basic setup

### Windows

```
> ./setup.bat msvc
> ./compile.bat
```

### WSL

```
> ./win_setup.sh msvc
> ./win_compile.sh
```

### Linux

```
> ./setup.sh gcc
> ./compile.sh
```

## Build with tests

```
> ./setup.bat msvc --buildtype=debug -Dlak_enable_tests=true
> ./compile.bat
```

## Build with libgphoto2

### Windows

Requires msys64

```
> ./setup.bat msvc -Dlak_enable_libgphoto2=true -Dlak_msys_prefix=C:/msys64
> ./compile.bat install-msys-libgphoto2-dependencies
> ./compile.bat
```

### Other

Requires libgphoto2 dependencies to be preinstalled.

```
> ./setup.sh gcc -Dlak_enable_libgphoto2=true -Dlak_msys_prefix=C:/msys64
> ./compile.sh
```

## Build with llvm

To use LLVM on Windows, MSVC must be installed with the MFC and ATL components

```
> ./setup.bat msvc -Dlak_enable_llvm=true -Dlak_msys_prefix=C:/msys64
> ./compile.bat
```
