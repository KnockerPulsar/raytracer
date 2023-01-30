set -xe

mkdir -p ./debug
cd ./debug
cmake -DCMAKE_BUILD_TYPE=Debug ../
cmake --build ./ -j12
cp compile_commands.json ../
cd ./
