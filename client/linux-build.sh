if [ -d bin ]; then rm -rf bin; fi
if [ -d src/pbuf/generated ]; then rm -rf src/pbuf/generated; fi
if [ -d ../shared-src/pbuf/generated ]; then rm -rf ../shared-src/pbuf/generated; fi
mkdir bin
mkdir src/pbuf/generated
mkdir ../shared-src/pbuf/generated

../vcpkg_installed/x64-linux/tools/protobuf/protoc -I ./src/pbuf --cpp_out=./src/pbuf/generated ./src/pbuf/*.proto
../vcpkg_installed/x64-linux/tools/protobuf/protoc -I ../shared-src/pbuf --cpp_out=../shared-src/pbuf/generated ../shared-src/pbuf/*.proto
g++ -std=c++17 -DRAYLIB_CPP_NO_MATH=1 -I ../vcpkg_installed/x64-linux/include/ -I include/ -I ../shared-src/ -L ../vcpkg_installed/x64-linux/lib/ src/*.cpp ../shared-src/*.cpp ./src/pbuf/generated/*.cc ../shared-src/pbuf/generated/*.cc -no-pie -Wl,-Bstatic -lraylib -lprotobuf -lstdc++fs -Wl,-Bdynamic -lGL -lm -lpthread -ldl -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -o bin/forbidden-desert

