@echo off

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64

set COMMON_COMPILE_FLAGS= -TC -std:c11 -arch:AVX2 -Zi -Od -MDd -EHsc -nologo -I"src" -Fo"build\\" -Fd"build\\"
set LINKER_FLAGS= 

REM BUILD MAIN PROGRAM
cl %COMMON_COMPILE_FLAGS% src\aoc_2023.c -link %LINKER_FLAGS% -OUT:build\\aoc_2023.exe
