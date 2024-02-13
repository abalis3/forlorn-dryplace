#!/bin/bash

cd linux-executable
mv forbidden-desert forbidden-desert-executable
mkdir forbidden-desert
mv forbidden-desert-executable forbidden-desert/forbidden-desert
chmod +x forbidden-desert/forbidden-desert
cp -r ../client/res forbidden-desert
tar -czpvf forbidden-desert-ubuntu-v$1.tar.gz *
cd ..

cd macOS-x86-executable
cp -r ../client/ci/mac-app/* .
mkdir -p Forbidden\ Desert.app/Contents/MacOS
mv forbidden-desert Forbidden\ Desert.app/Contents/MacOS/
chmod +x Forbidden\ Desert.app/Contents/MacOS/forbidden-desert
cp -r ../client/res Forbidden\ Desert.app/Contents/MacOS
tar -czpvf forbidden-desert-macOS-x86-v$1.tar.gz *
cd ..

cd macOS-arm64-executable
cp -r ../client/ci/mac-app/* .
mkdir -p Forbidden\ Desert.app/Contents/MacOS
mv forbidden-desert Forbidden\ Desert.app/Contents/MacOS/
chmod +x Forbidden\ Desert.app/Contents/MacOS/forbidden-desert
cp -r ../client/res Forbidden\ Desert.app/Contents/MacOS
tar -czpvf forbidden-desert-macOS-arm64-v$1.tar.gz *
cd ..

cd windows-build
mkdir forbidden-desert
mv forbidden-desert.exe forbidden-desert/forbidden-desert.exe
mv raylib.dll forbidden-desert/raylib.dll
mv libprotobuf.dll forbidden-desert/libprotobuf.dll
cp -r ../client/res forbidden-desert
zip -r forbidden-desert-windows-v$1.zip *
cd ..
