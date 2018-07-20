#!/bin/bash
set -xe

# Install MLNX Driver
RUN wget http://www.mellanox.com/downloads/ofed/MLNX_OFED-4.2-1.0.0.0/MLNX_OFED_LINUX-4.2-1.0.0.0-rhel7.4-x86_64.tgz
RUN gunzip < MLNX_OFED_LINUX-4.2-1.0.0.0-rhel7.4-x86_64.tgz | tar xvf -
RUN cd  MLNX_OFED_LINUX-4.2-1.0.0.0-rhel7.4-x86_64
RUN yes | ./mlnxofedinstall
RUN cd .. && rm -rf MLNX_OFED_LINUX-4.2-1.0.0.0-rhel7.4-x86_64 MLNX_OFED_LINUX-4.2-1.0.0.0-rhel7.4-x86_64.tgz
