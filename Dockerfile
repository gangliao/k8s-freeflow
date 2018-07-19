FROM centos:7.4.1708

MAINTAINER Gang Liao <gangliao@cs.umd.edu>

RUN yum install -y wget git curl sed grep vim make gcc-c++ libnl3-devel libtool && \
    yum clean all

RUN yum install -y pciutils numactl-libs gtk2 atk cairo gcc-gfortran tcsh lsof ethtool tcl tk && \
    yum clean all

# Install MLNX Driver
RUN wget http://www.mellanox.com/downloads/ofed/MLNX_OFED-4.2-1.0.0.0/MLNX_OFED_LINUX-4.2-1.0.0.0-rhel7.4-x86_64.tgz
RUN gunzip < MLNX_OFED_LINUX-4.2-1.0.0.0-rhel7.4-x86_64.tgz | tar xvf -
RUN cd  MLNX_OFED_LINUX-4.2-1.0.0.0-rhel7.4-x86_64
RUN yes | ./mlnxofedinstall

# git credential to skip password typing
RUN git config --global credential.helper store
