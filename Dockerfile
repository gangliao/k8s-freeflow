FROM centos:7.4.1708

MAINTAINER Gang Liao <gangliao@cs.umd.edu>

RUN yum install -y wget git curl sed grep vim make gcc-c++ libnl3-devel libtool && \
    yum clean all

RUN yum install -y pciutils numactl-libs gtk2 atk cairo gcc-gfortran tcsh lsof ethtool tcl tk && \
    yum clean all

# git credential to skip password typing
RUN git config --global credential.helper store
