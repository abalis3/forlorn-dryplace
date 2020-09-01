if [ -d bin ]; then rm -rf bin; fi
if [ -d src/pbuf/generated ]; then rm -rf src/pbuf/generated; fi
mkdir bin
mkdir src/pbuf/generated

../vcpkg/installed/x64-linux/tools/protobuf/protoc -I ./src/pbuf --cpp_out=./src/pbuf/generated ./src/pbuf/*.proto
g++ -DRAYLIB_CPP_NO_MATH=1 -I ../vcpkg/installed/x64-linux/include/ -I include/ -I ../shared-src/ -L ../vcpkg/installed/x64-linux/lib/ src/*.cpp ../shared-src/*.cpp ./src/pbuf/generated/*.cc -no-pie -Wl,-Bstatic -lraylib -lprotobuf -lstdc++fs -Wl,-Bdynamic -lGL -lm -lpthread -ldl -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -o bin/forbidden-desert

