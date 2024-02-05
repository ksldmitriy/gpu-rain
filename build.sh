#!/bin/bash

echo === CMAKE ===

cmake -S . -B ./out  

mv out/compile_commands.json .

echo === BUILD ===

make -j 16 -C ./out;
