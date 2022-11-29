#!/bin/bash

#cd build

./memgen 5500
../rvas ../test1.asm test1.bin
../rvas ../test2.asm test2.bin

src/cpu test1.bin test2.bin memory.bin

cd ..


