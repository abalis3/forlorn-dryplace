if [ -d bin ]; then rm -rf bin; fi
mkdir bin
g++ -I include/ src/core_basic_window.cpp -lraylib -lGL -lm -lpthread -ldl -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -o bin/forlorn-dryplace
