#!/bin/sh
echo "============================="
echo "Building the assembler...."
echo "============================="

cd riscv-as
if [ ! -d ./build ]
then
  mkdir build
fi
cd build
cmake ..
make

cd ../..

echo "Done!"
echo ""

echo "============================="
echo "Building the simulator...."
echo "============================="
cd riscv-sim

g++ cpu.cpp -o cpu

cd ..

echo  ""
echo "Done!"
echo ""