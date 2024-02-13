ARCH=$(uname -m)
OSX_VERSION=$(sw_vers -productVersion)

if [ "$ARCH" == "arm64" ]; then
    VC_TUPLE=arm64-osx
else
    VC_TUPLE=x64-osx
fi

if [ -d bin ]; then rm -rf bin; fi
if [ -d src/pbuf/generated ]; then rm -rf src/pbuf/generated; fi
if [ -d ../shared-src/pbuf/generated ]; then rm -rf ../shared-src/pbuf/generated; fi
mkdir bin
mkdir src/pbuf/generated
mkdir ../shared-src/pbuf/generated

../vcpkg_installed/${VC_TUPLE}/tools/protobuf/protoc -I ./src/pbuf --cpp_out=./src/pbuf/generated ./src/pbuf/*.proto
../vcpkg_installed/${VC_TUPLE}/tools/protobuf/protoc -I ../shared-src/pbuf --cpp_out=../shared-src/pbuf/generated ../shared-src/pbuf/*.proto
g++ -std=c++17 -mmacosx-version-min=${OSX_VERSION} -DOSX_RELEASE_BUILD -DRAYLIB_CPP_NO_MATH=1 -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -I ../vcpkg_installed/${VC_TUPLE}/include/ -I include ../vcpkg_installed/${VC_TUPLE}/lib/*.a -I ../shared-src/ src/*.cpp ../shared-src/*.cpp src/pbuf/generated/*.cc ../shared-src/pbuf/generated/*.cc -o bin/forbidden-desert

