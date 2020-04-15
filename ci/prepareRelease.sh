#!/bin/bash

cd linux-executable
tar -czvf forbidden-desert-ubuntu-v$1.tar.gz *
cd ..

cd macOS-executable
tar -czvf forbidden-desert-macOS-v$1.tar.gz *
cd ..

cd windows-executable
zip forbidden-desert-windows-v$1.zip *
cd ..
