#!/bin/bash

cd linux-executable
ls -la
tar -czpvf forbidden-desert-ubuntu-v$1.tar.gz *
cd ..

cd macOS-executable
ls -la
tar -czpvf forbidden-desert-macOS-v$1.tar.gz *
cd ..

cd windows-executable
zip forbidden-desert-windows-v$1.zip *
cd ..
