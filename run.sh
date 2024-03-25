#!/bin/bash

trap terminate SIGINT
terminate(){
    pkill lofi-rain
    exit
}

echo === RUN ===

./bin/lofi-rain
