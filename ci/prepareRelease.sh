#!/bin/bash

cd linux-executable
mv forbidden-desert forbidden-desert-executable
mkdir forbidden-desert
mv forbidden-desert-executable forbidden-desert/forbidden-desert
chmod +x forbidden-desert/forbidden-desert
cp -r ../res forbidden-desert
tar -czpvf forbidden-desert-ubuntu-v$1.tar.gz *
cd ..

cd macOS-executable
mv forbidden-desert forbidden-desert-executable
mkdir forbidden-desert
mv forbidden-desert-executable forbidden-desert/forbidden-desert
chmod +x forbidden-desert/forbidden-desert
cp -r ../res forbidden-desert
tar -czpvf forbidden-desert-macOS-v$1.tar.gz *
cd ..

cd windows-executable
mkdir forbidden-desert
mv forbidden-desert.exe forbidden-desert/forbidden-desert.exe
cp -r ../res forbidden-desert
zip -r forbidden-desert-windows-v$1.zip *
cd ..