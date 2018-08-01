# k8s-freeflow

## TO-DO List

- [x] IP Hunter: write changed nodes and map(vip, pip) into ETCD periodically [CODE](https://github.com/gangliao/k8s-freeflow/blob/master/ip_hunter/ip2etcd.go)
- [x] IP Hunter's Docker Image [CODE](https://github.com/gangliao/k8s-freeflow/blob/master/ip_hunter/run.sh)

- [x] **Have yet Test** ffrouter update HOST_LIST and vip_map via RESTful API of ETCD's Watch mode periodically**. [CODE](https://github.com/gangliao/k8s-freeflow/blob/master/freeflow/ffrouter/ffrouter.cpp#L6-L168)

- [x] [ffouter docker image](https://github.com/gangliao/k8s-freeflow#build-freeflow-router-image)

- [x] [client docker image](https://github.com/gangliao/k8s-freeflow#build-freeflow-client-image)

- [x] GoogleTest **PASS**: ETCD V3 API's watch mode for ffrouter [CODE](https://github.com/gangliao/k8s-freeflow/blob/master/freeflow/ffrouter/etcd_test.cpp#L245-L297)

    The Watch API provides an event-based interface for asynchronously monitoring changes to keys. An etcd3 watch waits for changes to keys by continuously watching from a given revision, either current or historical, and streams key updates back to the client. https://coreos.com/etcd/docs/latest/learning/api.html

- [x] GoogleTest **PASS**: ETCD V3 API's range mode for ffrouter
- [x] GoogleTest **PASS**: ETCD V2 API's put mode for ffrouter

- [x] new ffrouter depends on curl, base64 and jsoncpp. [CODE](https://github.com/gangliao/k8s-freeflow/tree/master/freeflow/ffrouter/cmake)
- [x] the process of compilation passed. [CODE](https://github.com/gangliao/k8s-freeflow/blob/master/freeflow/ffrouter/CMakeLists.txt)

- [ ] **testing new ffouter with IP hunter and fix bug.**
- [ ] IP Hunter -> Kubernetes POD: ip_hunter_pod.yaml
- [ ] ffrouter -> Kubernetes Daemonset: ffrouter_daemonset.yaml
- [ ] Benchmark: baseline Kubeflow


## Environment

- Host OS: CentOS Linux release 7.4.1708 (Core)
- Host Kernel: 3.10.0-693.el7.x86_64
- Host MLNX_OFED_LINUX: MLNX_OFED_LINUX-4.2-1.0.0.0 

## Run IP Hunter

```bash
cd ip_hunter && ./run.sh
```


## Build Dev Image

To build docker image `rdma_dev`, please issue the following command:

```bash
OS_VERSION=$(lsb_release -r --short)
OS_ID=$(lsb_release -i --short  | awk '{print tolower($0)}')
KERNEL_VERSION=$(uname -r)

echo "$OS_ID:$OS_VERSION"

cat > Dockerfile << EOF
FROM $OS_ID:$OS_VERSION

MAINTAINER Gang Liao <gangliao@cs.umd.edu>

RUN yum install -y wget git curl sed grep vim make gcc-c++ \
    libcurl-devel libnl3-devel libtool && \
    yum clean all

RUN yum install -y libxml2-python pciutils numactl-libs gtk2 atk cairo \
    gcc-gfortran tcsh lsof ethtool tcl tk && \
    yum clean all

RUN wget https://cmake.org/files/v3.6/cmake-3.6.2.tar.gz
RUN tar xvf cmake-3.6.2.tar.gz && cd cmake-3.6.2/
RUN ./bootstrap && make -j8 && make install && ln -s /usr/local/bin/cmake /usr/bin/
RUN cd .. && rm -rf cmake-3.6.2 cmake-3.6.2.tar.gz 

# git credential to skip password typing
RUN git config --global credential.helper store
EOF

export http_proxy=http://10.130.14.129:8080
docker build --build-arg http_proxy=$http_proxy \
               --build-arg https_proxy=$http_proxy \
               --build-arg HTTP_PROXY=$http_proxy \
               --build-arg HTTPS_PROXY=$http_proxy \
               -t rdma_dev:$OS_ID$OS_VERSION -f ./Dockerfile .

# or directly no-proxy
# docker build -t rdma_dev:$OS_ID$OS_VERSION -f ./Dockerfile .
```

## Install MLNX_OFED_LINUX into Dev Image 

To simplify the compile process, here we use the same CentOS version inside the image.
You can check it on the host machine:

```bash
host$ cat /etc/centos-release

CentOS Linux release 7.4.1708 (Core)

host$ uname -r

3.10.0-693.el7.x86_64
```

To install `MLNX_OFED_LINUX` driver in the docker image, you must mount `/lib/modules/3.10.0-693.el7.x86_64/` to complete this process (soft link `/usr/src/kernels/3.10.0-693.el7.x86_64/` also need to mount).

```bash
host$ ll /lib/modules/3.10.0-693.el7.x86_64/
total 3144
lrwxrwxrwx.  1 root root     38 Mar 20 23:03 build -> /usr/src/kernels/3.10.0-693.el7.x86_64
drwxr-xr-x.  8 root root   4096 Oct 30  2017 extra
drwxr-xr-x. 12 root root   4096 Mar 20 23:03 kernel
-rw-r--r--   1 root root 789739 Jul 20 01:28 modules.alias
-rw-r--r--   1 root root 758327 Jul 20 01:28 modules.alias.bin
-rw-r--r--.  1 root root   1334 Aug 23  2017 modules.block
-rw-r--r--.  1 root root   6457 Aug 23  2017 modules.builtin
-rw-r--r--   1 root root   8263 Jul 20 01:28 modules.builtin.bin
-rw-r--r--   1 root root 270436 Jul 20 01:28 modules.dep
-rw-r--r--   1 root root 375753 Jul 20 01:28 modules.dep.bin
-rw-r--r--   1 root root    361 Jul 20 01:28 modules.devname
-rw-r--r--.  1 root root    132 Aug 23  2017 modules.drm
-rw-r--r--.  1 root root    110 Aug 23  2017 modules.modesetting
-rw-r--r--.  1 root root   1689 Aug 23  2017 modules.networking
-rw-r--r--.  1 root root  93026 Aug 23  2017 modules.order
-rw-r--r--   1 root root    218 Jul 20 01:28 modules.softdep
-rw-r--r--   1 root root 383057 Jul 20 01:28 modules.symbols
-rw-r--r--   1 root root 468898 Jul 20 01:28 modules.symbols.bin
lrwxrwxrwx.  1 root root      5 Mar 20 23:03 source -> build
drwxr-xr-x.  2 root root   4096 Aug 23  2017 updates
drwxr-xr-x.  2 root root   4096 Mar 20 23:03 vdso
drwxr-xr-x.  2 root root   4096 Aug 23  2017 weak-updates
```

```bash
host$ docker rm -f $(docker ps -a | awk '$2=="rdma_dev:$OS_ID$OS_VERSION" {print $1}')

host$ docker run --net=host --name gang_rdma_dev --privileged -d -it \
    -v `pwd`:/k8s_freeflow -v /sys/class/:/sys/class/ -v /dev/:/dev/ \
    -v /lib/modules/$KERNEL_VERSION/:/lib/modules/$KERNEL_VERSION/ \
    -v /usr/src/kernels/$KERNEL_VERSION/:/usr/src/kernels/$KERNEL_VERSION/ \
    --device=/dev/infiniband/uverbs0 --device=/dev/infiniband/rdma_cm \
    rdma_dev:$OS_ID$OS_VERSION

host$ docker exec -it gang_rdma_dev bash

docker-root$ export http_proxy=http://10.130.14.129:8080
docker-root$ /k8s_freeflow/scripts/install_driver.sh


# Commit rdma dev
host$ container_id=$(docker ps | grep rdma_dev:$OS_ID$OS_VERSION | awk '{print $1}')
host$ docker commit -a "Gang Liao <gangliao@cs.umd.edu>" -m "install MLNX_OFED_LINUX" $container_id gangliao/rdma_dev:$OS_ID$OS_VERSION
# host$ docker push
```

**Note:** `install_driver.sh` will install MLNX_OFED_LINUX driver, please make sure the
version of download package is same to the driver of host.

```bash
host$ ofed_info | grep MLNX_OFED_LINUX | awk -F '/' '{print $2}' | uniq

MLNX_OFED_LINUX-4.2-1.0.0.0
```

## Build FreeFlow Client Image

```bash
host$ docker rm -f $(docker ps -a | grep rdma_dev:$OS_ID$OS_VERSION | awk '{print $1}')
host$ docker rm -f $(docker ps -a | grep gangliao/rdma_dev:$OS_ID$OS_VERSION | awk '{print $1}')

host$ docker run --net=host --name gang_rdma_dev --privileged -d -it \
    -v `pwd`:/k8s_freeflow -v /sys/class/:/sys/class/ -v /dev/:/dev/ \
    --device=/dev/infiniband/uverbs0 --device=/dev/infiniband/rdma_cm \
    gangliao/rdma_dev:$OS_ID$OS_VERSION

host$ docker exec -it gang_rdma_dev bash

docker-root$ /k8s_freeflow/scripts/build_client.sh

host$ container_id=$(docker ps -a | grep gangliao/rdma_dev:$OS_ID$OS_VERSION | awk '{print $1}')
host$ docker commit -a "Gang Liao <gangliao@cs.umd.edu>" -m "build freeflow-client" $container_id gangliao/freeflow-client:$OS_ID$OS_VERSION
# host$ docker push
```

## Build FreeFlow Router Image

```bash
host$ docker rm -f $(docker ps -a | grep rdma_dev:$OS_ID$OS_VERSION | awk '{print $1}')
host$ docker rm -f $(docker ps -a | grep gangliao/rdma_dev:$OS_ID$OS_VERSION | awk '{print $1}')

host$ docker run --net=host --name gang_rdma_dev --privileged -d -it \
    -v `pwd`:/k8s_freeflow -v /sys/class/:/sys/class/ -v /dev/:/dev/ \
    --device=/dev/infiniband/uverbs0 --device=/dev/infiniband/rdma_cm \
    gangliao/rdma_dev:$OS_ID$OS_VERSION

host$ docker exec -it gang_rdma_dev bash

docker-root$ /k8s_freeflow/scripts/build_router.sh

host$ container_id=$(docker ps -a | grep gangliao/rdma_dev:$OS_ID$OS_VERSION | awk '{print $1}')
host$ docker commit -a "Gang Liao <gangliao@cs.umd.edu>" -m "build freeflow-router" $container_id gangliao/freeflow-router:$OS_ID$OS_VERSION
# host$ docker push
```

