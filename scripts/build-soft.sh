#!/bin/sh
cd ../soft.git
./autogen.sh
./configure --prefix=/home/sof/bin
make -j$(nproc)
make install
cd ../sof.git
