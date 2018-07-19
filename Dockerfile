FROM centos:centos7.3.1611
MAINTAINER Gang Liao <gangliao@cs.umd.edu>

RUN yum update -y && \
    yum install -y wget git pkg-config curl sed grep vim infiniband-diags perftest make \
    locales clang-format libtool bsdmainutils libevent-dev net-tools gcc-c++ libnl3-devel && \
    yum clean all

# librdmacm-devel librdmacm-utils librdmacm-dev libibverbs-dev ibverbs-utils
# libibverbs-dev libmlx4-dev libmlx5-dev
# Set the locale
ENV localedef -i en_US -f UTF-8 en_US.UTF-8
# git credential to skip password typing
RUN git config --global credential.helper store
