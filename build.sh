#!/bin/bash
set -xe

cd freeflow

# Install Memory Pool
cd libmempool && make && make install && cd ..

cd libraries/libmlx4-1.2.1mlnx1
./autogen.sh && ./configure && make
make install
cd ..


cd libraries/libibverbs-1.2.1mlnx 
./autogen.sh && ./configure && make


