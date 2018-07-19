#!/bin/bash
set -xe

export http_proxy=http://10.130.14.129:8080
wget http://www.mellanox.com/downloads/ofed/MLNX_OFED-4.4-1.0.0.0/MLNX_OFED_LINUX-4.4-1.0.0.0-rhel7.3-x86_64.tgz
gunzip < MLNX_OFED_LINUX-4.4-1.0.0.0-rhel7.3-x86_64.tgz  | tar xvf -
cd MLNX_OFED_LINUX-4.4-1.0.0.0-rhel7.3-x86_64
yes | ./mlnxofedinstall


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
