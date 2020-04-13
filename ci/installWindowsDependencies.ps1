cscript ci/downloadWindowsRaylib.js
Expand-Archive raylib-*.zip -DestinationPath .
rm raylib-*.zip
Rename-Item raylib-3.0.0-Win32-mingw raylib