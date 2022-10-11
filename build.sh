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

if [ -f ./cpu ]; then
    rm cpu
fi

g++ cpu.cpp -o cpu
if [ $? != 0 ] ; then
    cd ..
    exit 1
fi

cd ..

mv riscv-sim/cpu ./cpu
cp riscv-as/build/src/rvas ./rvas
./rvas riscv-sim/input.asm

echo  ""
echo "Done!"
echo ""