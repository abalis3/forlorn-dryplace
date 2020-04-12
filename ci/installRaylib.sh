#!/bin/bash

sudo apt-get install mesa-common-dev libx11-dev libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev #libasound2-dev

RAYLIB_URL=$(curl -s https://api.github.com/repos/raysan5/raylib/releases/tags/3.0.0 \
    | grep browser_download_url \
    | grep Linux \
    | grep 64 \
    | cut -d '"' -f 4)

wget $RAYLIB_URL

mkdir -p raylib
tar xvzf raylib-3.0.0-Linux-amd64.tar.gz --strip 1 --directory raylib
rm raylib-3.0.0-Linux-amd64.tar.gz