@echo off
(
echo [binaries]
echo llvm-config = '%cd%/toolchain/llvm/bin/llvm-config.exe'
)>local-llvm-config.ini && setup.bat %* --native-file local-llvm-config.ini || exit 1
