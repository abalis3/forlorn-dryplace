./scripts/installLinuxDependencies.sh
git submodule init
git submodule update
./vcpkg/bootstrap-vcpkg.sh
./vcpkg/vcpkg install

