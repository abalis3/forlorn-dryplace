if [ -d bin ]; then rm -rf bin; fi
mkdir bin

g++ -std=c++11 -DRAYLIB_CPP_NO_MATH=1 -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -I raylib/include -I include raylib/lib/libraylib.a src/*.cpp -o bin/forbidden-desert