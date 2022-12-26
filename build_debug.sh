set -xe

cd ./debug
cmake -DCMAKE_BUILD_TYPE=Debug ../
cmake --build ./ -j12
cp compile_commands.json ../
cd ./
