#!/bin/bash
set -xe

MAJOR_VER=$(lsb_release -r --short | cut -d. -f1)
MINOR_VER=$(lsb_release -r --short | cut -d. -f2)
VER=$MAJOR_VER.$MINOR_VER

# Install MLNX Driver
wget http://www.mellanox.com/downloads/ofed/MLNX_OFED-4.2-1.0.0.0/MLNX_OFED_LINUX-4.2-1.0.0.0-rhel$VER-x86_64.tgz
gunzip < MLNX_OFED_LINUX-4.2-1.0.0.0-rhel$VER-x86_64.tgz | tar xvf -
cd  MLNX_OFED_LINUX-4.2-1.0.0.0-rhel$VER-x86_64
echo 'Y' | ./mlnxofedinstall
cd .. && rm -rf MLNX_OFED_LINUX-4.2-1.0.0.0-rhel$VER-x86_64 MLNX_OFED_LINUX-4.2-1.0.0.0-rhel$VER-x86_64.tgz
