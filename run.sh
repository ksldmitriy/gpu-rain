#!/bin/sh

trap terminate SIGINT
terminate(){
    pkill gpu-rain
    exit
}

echo === RUN ===

./bin/gpu-rain
