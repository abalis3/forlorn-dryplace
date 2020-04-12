if [ -d bin ]; then rm -rf bin; fi
mkdir bin

g++ -DRAYLIB_CPP_NO_MATH=1 -D__cplusplus=1 -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -I raylib/include -I include raylib/lib/libraylib.a src/*.cpp -o bin/forlorn-dryplace