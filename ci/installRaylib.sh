#!/bin/bash

sudo apt-get install libasound2-dev mesa-common-dev libx11-dev libxrandr-dev libxi-dev xorg-dev
sudo ln -s /usr/lib/x86_64-linux-gnu/libGL.so.1 /usr/lib/libGL.so

RAYLIB_URL=$(curl -s https://api.github.com/repos/raysan5/raylib/releases/tags/3.0.0 \
    | grep browser_download_url \
    | grep Linux \
    | grep 64 \
    | cut -d '"' -f 4)

wget $RAYLIB_URL

mkdir -p raylib
tar xvzf raylib-3.0.0-Linux-amd64.tar.gz --strip 1 --directory raylib
rm raylib-3.0.0-Linux-amd64.tar.gz