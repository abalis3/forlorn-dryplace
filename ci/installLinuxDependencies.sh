#!/bin/bash

sudo apt-get install mesa-common-dev libx11-dev libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev #libasound2-dev

wget https://github.com/raysan5/raylib/releases/download/3.0.0/raylib-3.0.0-Linux-amd64.tar.gz

mkdir -p raylib
tar xvzf raylib-3.0.0-Linux-amd64.tar.gz --strip 1 --directory raylib
rm raylib-3.0.0-Linux-amd64.tar.gz