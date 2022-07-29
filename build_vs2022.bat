git pull --recurse-submodules
git submodule update --init --recursive
mkdir build
cd build
cmake .. -G "Visual Studio 16 2022" -A x64
cmake --build . --config Release
