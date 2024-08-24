#!/usr/bin/env bash

compile_move()
{
  file=$1
  basename="${file%.*}" # Get the executable file name

  gcc $file -o $basename # Compile and create .exe file
  mv $basename ./build/ # Move .exe file to build folder
}

main="./src/main.c"
compile_move $main

for file in ./src/commands/*
do
  compile_move $file
done

./build/main # Run Program
