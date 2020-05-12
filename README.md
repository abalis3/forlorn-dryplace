<p align="center">
  <a href="https://github.com/abalis3/forlorn-dryplace/actions?query=branch%3Amaster+workflow%3A%22Build+and+Release%22">
    <img alt="Build and Release"
         src="https://github.com/abalis3/forlorn-dryplace/workflows/Build%20and%20Release/badge.svg?branch=master">
  </a> 
  <a href="https://github.com/semantic-release/semantic-release">
    <img alt="semantic-release"
         src="https://img.shields.io/badge/%20%20%F0%9F%93%A6%F0%9F%9A%80-semantic--release-e10079.svg">
  </a>
</p>

# forlorn-dryplace

An implementation of a favorite board game in C++ using raylib.

## To Download and Run Game

### Windows
- Go to the [releases](https://github.com/abalis3/forlorn-dryplace/releases) tab on github and find the desired release version
- Download the forbidden-desert-windows-[version].zip file under the release assets
- Extract the downloaded zip and enter the forbidden-desert folder within
- If you have an antivirus, it likely has to be disabled since this is not a trusted widespread-use program
- Run the forbidden-desert executable. When you click it, windows defender may warn it is not from a trusted publisher. If this happens, click 'More Info' and then the 'Run Anyway' button that appears

## To Contribute Code

### Windows Setup
- Install raylib with mingw installer (installs to C:\raylib)
- Add g++ to path (C:\raylib\mingw\bin)
- Clone this repo and run win-build.bat to create executable

### Linux Setup
- Install raylib (Use version 3.0.0!) following https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux
- Clone this repository and run linux-build.sh to create executable (output in bin/)

### MacOS Setup
- Clone this repository and enter its root directory
- Run the script ci/installMacDependencies.sh to install raylib into the project directory
- Run mac-build.sh to create executable (output in bin/)

