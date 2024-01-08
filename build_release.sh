#!/usr/bin/env sh
set -xe

mkdir -p ./release
cd ./release
cmake -DCMAKE_BUILD_TYPE=Release ../
cmake --build ./ -j12
cp compile_commands.json ../
cd ./
