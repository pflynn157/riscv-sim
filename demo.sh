#!/bin/bash

cd build

./memgen 5400
../rvas ../program.asm test.bin

./setup

src/cpu test.bin memory.bin

./verify

cd ..


