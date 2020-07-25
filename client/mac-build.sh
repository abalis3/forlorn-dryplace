if [ -d bin ]; then rm -rf bin; fi
if [ -d src/pbuf/generated ]; then rm -rf src/pbuf/generated; fi
mkdir bin
mkdir src/pbuf/generated

../vcpkg/installed/x64-osx/tools/protobuf/protoc -I ./src/pbuf --cpp_out=./src/pbuf/generated ./src/pbuf/*.proto
g++ -std=c++11 -DOSX_RELEASE_BUILD -DRAYLIB_CPP_NO_MATH=1 -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -I ../vcpkg/installed/x64-osx/include/ -I include ../vcpkg/installed/x64-osx/lib/*.a src/*.cpp src/pbuf/generated/*.cc -o bin/forbidden-desert

