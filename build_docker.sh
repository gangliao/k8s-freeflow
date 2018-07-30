#!/bin/bash
set -xe

VERSION=$(lsb_release -r --short)
OSID=$(lsb_release -i --short  | awk '{print tolower($0)}')

echo "$OSID:$VERSION"

cat > Dockerfile << EOF
FROM $OSID:$VERSION

MAINTAINER Gang Liao <gangliao@cs.umd.edu>

RUN yum install -y wget git curl sed grep vim make gcc-c++ libnl3-devel libtool && \
    yum clean all

RUN yum install -y pciutils numactl-libs gtk2 atk cairo gcc-gfortran tcsh lsof ethtool tcl tk && \
    yum clean all

# git credential to skip password typing
RUN git config --global credential.helper store
EOF

export http_proxy=http://10.130.14.129:8080
docker build --build-arg http_proxy=$http_proxy \
               --build-arg https_proxy=$http_proxy \
               --build-arg HTTP_PROXY=$http_proxy \
               --build-arg HTTPS_PROXY=$http_proxy \
               -t rdma_dev:$OSID$VERSION -f ./Dockerfile .

# or directly no-proxy
# docker build -t rdma_dev:$OSID$VERSION -f ./Dockerfile .