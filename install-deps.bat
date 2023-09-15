@ECHO OFF
git submodule init
git submodule update
CALL ./vcpkg/bootstrap-vcpkg.bat
START /WAIT /B vcpkg/vcpkg.exe install