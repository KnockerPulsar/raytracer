mkdir -p build
cd build
cmake ../ && cmake --build ./
cd ../

cp build/compile_commands.json ./