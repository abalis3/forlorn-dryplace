@ECHO OFF
rmdir /s /q bin > NUL 2>&1
rmdir /s /q "src/pbuf/generated" > NUL 2>&1
rmdir /s /q "../shared-src/pbuf/generated" > NUL 2>&1
mkdir bin
mkdir "src/pbuf/generated"
mkdir "../shared-src/pbuf/generated"


start /B /WAIT ../vcpkg_installed/x86-windows/tools/protobuf/protoc -I ./src/pbuf --cpp_out=./src/pbuf/generated ./src/pbuf/*.proto
start /B /WAIT ../vcpkg_installed/x86-windows/tools/protobuf/protoc -I ../shared-src/pbuf --cpp_out=../shared-src/pbuf/generated ../shared-src/pbuf/*.proto
msbuild /verbosity:quiet /p:OutDir=bin/ /p:configuration=release forbidden-desert.vcxproj
rmdir /s /q release
copy ..\vcpkg\installed\x86-windows\bin\raylib.dll .\bin > NUL
copy ..\vcpkg\installed\x86-windows\bin\libprotobuf.dll .\bin > NUL
echo Built executable in 'bin/forbidden-desert.exe'
