#!/bin/bash
set -xe

cd freeflow

# Install Memory Pool
cd libmempool && make && make install && cd ..


cd libraries/libmlx4-1.2.1mlnx1
./autogen.sh
./configure --prefix=/usr/ --libdir=/usr/lib/ --sysconfdir=/etc/
make && make install
cd ..


cd libraries/libibverbs-1.2.1mlnx 
./autogen.sh
./configure --prefix=/usr/ --libdir=/usr/lib/ --sysconfdir=/etc/
make && make install
cd ..


cd libraries/librdmacm-1.1.0mlnx
./autogen.sh
./configure --prefix=/usr/ --libdir=/usr/lib/ --sysconfdir=/etc/
make && make install
cd ..
