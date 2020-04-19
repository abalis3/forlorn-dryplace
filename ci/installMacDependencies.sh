#!/bin/bash

wget https://github.com/raysan5/raylib/releases/download/3.0.0/raylib-3.0.0-macOS.tar.gz

mkdir -p raylib
tar xvzf raylib-3.0.0-macOS.tar.gz --strip 1 --directory raylib
rm raylib-3.0.0-macOS.tar.gz