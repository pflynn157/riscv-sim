#!/bin/bash

cd build

./memgen 4096
../rvas ../program.asm test.bin

./setup

src/cpu test.bin memory.bin

./verify

cd ..


