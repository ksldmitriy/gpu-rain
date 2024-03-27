#!/bin/sh

echo === RESOURCES ===

./create-resources.sh

echo === CMAKE ===

cmake -S . -B ./out  

mv out/compile_commands.json .

echo === BUILD ===

make -j 16 -C ./out;
