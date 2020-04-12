#!/bin/bash

RAYLIB_URL=$(curl -s https://api.github.com/repos/raysan5/raylib/releases/tags/3.0.0 \
    | grep browser_download_url \
    | grep macOS \
    | cut -d '"' -f 4)

wget $RAYLIB_URL

mkdir -p raylib
tar xvzf raylib-3.0.0-macOS.tar.gz --strip 1 --directory raylib
rm raylib-3.0.0-macOS.tar.gz