#!/bin/bash

#cd build

./memgen 5500
../rvas ../program1.asm test1.bin
../rvas ../program2.asm test2.bin

./setup

src/cpu test1.bin test2.bin memory.bin

./verify

cd ..


