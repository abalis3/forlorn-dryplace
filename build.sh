if [ -d bin ]; then rm -rf bin; fi
mkdir bin

if [ $# == 1 ] && [ $1 == "CI" ]; then
g++ -DRAYLIB_CPP_NO_MATH=1 -I raylib/include/ -I include/ raylib/lib/*.a src/core_basic_window.cpp -lGL -lm -lpthread -ldl -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -o bin/forlorn-dryplace
else
g++ -I include/ src/core_basic_window.cpp -lraylib -lGL -lm -lpthread -ldl -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -o bin/forlorn-dryplace
fi