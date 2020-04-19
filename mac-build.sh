if [ -d bin ]; then rm -rf bin; fi
mkdir bin

if [ $# == 1 ] && [ $1 == "CI" ]; then
g++ -std=c++11 -DOSX_RELEASE_BUILD -DRAYLIB_CPP_NO_MATH=1 -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -I raylib/include -I include raylib/lib/libraylib.a src/*.cpp -o bin/forbidden-desert
else
g++ -std=c++11 -DRAYLIB_CPP_NO_MATH=1 -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -I raylib/include -I include raylib/lib/libraylib.a src/*.cpp -o bin/forbidden-desert
fi