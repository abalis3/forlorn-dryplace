@ECHO OFF
rmdir /s /q bin
mkdir bin

if "%*"=="CI" (
    g++ -m64 -DRAYLIB_CPP_NO_MATH=1 -I raylib/include/ -I include/ -L raylib/lib src/*.cpp -lopengl32 -static -lraylib_static -lpthread -lwinmm -lgdi32 -o bin/forbidden-desert.exe
    echo.
    echo Built executable for CI
) else (
    g++ -DRAYLIB_CPP_NO_MATH=1 -I include/ src/*.cpp -lraylib -lopengl32 -lgdi32 -lwinmm -static -lpthread -o bin/forbidden-desert.exe
    echo.
    echo Built executable in 'bin/forbidden-desert.exe'
)