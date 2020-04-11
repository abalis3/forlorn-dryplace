@ECHO OFF
rmdir /s /q bin
mkdir bin

g++ -DRAYLIB_CPP_NO_MATH=1 -I include/ src/*.cpp -lraylib -lopengl32 -lgdi32 -lwinmm -static -lpthread -o bin/forlorn-dryplace.exe
echo.
echo Built executable in 'bin/forlorn-dryplace.exe'