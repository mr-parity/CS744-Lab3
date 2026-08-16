#!/bin/bash

echo "Starting setup..."

#install git
echo "Installing git..."
sudo apt-get install git 

echo "Installing Build-essential..."
sudo apt-get install build-essential

echo "Installing gdb-multiarch..."
sudo apt-get install gdb-multiarch

echo "Installing qemu-system-misc"
sudo apt-get install qemu-system-misc -y

echo "Installing gcc-riscv64-linuxgnu"
sudo apt-get install gcc-riscv64-linux-gnu -y

echo "installing binutils-riscv64-gnu"
sudo apt-get install binutils-riscv64-linux-gnu -y

echo "All Installation finished successfully!"

echo "Testing the qemu"
qemu-system-riscv64 --version


