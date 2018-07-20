# k8s-freeflow

## Environment

- Host OS: CentOS Linux release 7.4.1708 (Core)
- Host Kernel: 3.10.0-693.el7.x86_64
- Host MLNX_OFED_LINUX: MLNX_OFED_LINUX-4.2-1.0.0.0 

## Build Dev Image

```bash
host$ export http_proxy=http://10.130.14.129:8080
host$ docker build --build-arg http_proxy=$http_proxy \
               --build-arg https_proxy=$http_proxy \
               --build-arg HTTP_PROXY=$http_proxy \
               --build-arg HTTPS_PROXY=$http_proxy \
               -t rdma_dev:centos7.4.1708 -f ./Dockerfile .
# or directly no-proxy
# docker build -t rdma_dev:latest -f ./Dockerfile .
```

## Install MLNX_OFED_LINUX into Dev Image 

To simplify the compile process, here we use the same CentOS version inside the image.
You can check it on the host machine:

```bash
host$ cat /etc/centos-release

CentOS Linux release 7.4.1708 (Core)

host$ uname -a

Linux nmyjs_186_118 3.10.0-693.el7.x86_64 #1 SMP Tue Aug 22 21:09:27 UTC 2017 x86_64 x86_64 x86_64 GNU/Linux
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
host$ docker rm -f $(docker ps -a | awk '$2=="rdma_dev:centos7.4.1708" {print $1}')

host$ docker run --net=host --name gang_rdma_dev --privileged -d -it \
    -v `pwd`:/k8s_freeflow -v /sys/class/:/sys/class/ -v /dev/:/dev/ \
    -v /lib/modules/3.10.0-693.el7.x86_64/:/lib/modules/3.10.0-693.el7.x86_64/ \
    -v /usr/src/kernels/3.10.0-693.el7.x86_64/:/usr/src/kernels/3.10.0-693.el7.x86_64/ \
    --device=/dev/infiniband/uverbs0 --device=/dev/infiniband/rdma_cm \
    rdma_dev:centos7.4.1708

host$ docker exec -it gang_rdma_dev bash

docker-root$ ./k8s_freeflow/install_driver.sh

host$ docker tag rdma_dev:centos7.4.1708 gangliao/rdma_dev:centos7.4.1708
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
host$ docker rm -f $(docker ps -a | awk '$2=="gangliao/rdma_dev:centos7.4.1708" {print $1}')

host$ docker run --net=host --name gang_rdma_dev --privileged -d -it \
    -v `pwd`:/k8s_freeflow -v /sys/class/:/sys/class/ -v /dev/:/dev/ \
    --device=/dev/infiniband/uverbs0 --device=/dev/infiniband/rdma_cm \
    gangliao/rdma_dev:centos7.4.1708

host$ docker exec -it gang_rdma_dev bash

docker-root$ ./k8s_freeflow/build-client.sh

host$ docker tag gangliao/rdma_dev:centos7.4.1708 gangliao/freeflow-client:centos7.4.1708
# host$ docker push
```

## Build FreeFlow Router Image

```bash
host$ docker rm -f $(docker ps -a | awk '$2=="gangliao/rdma_dev:centos7.4.1708" {print $1}')

host$ docker run --net=host --name gang_rdma_dev --privileged -d -it \
    -v `pwd`:/k8s_freeflow -v /sys/class/:/sys/class/ -v /dev/:/dev/ \
    --device=/dev/infiniband/uverbs0 --device=/dev/infiniband/rdma_cm \
    gangliao/rdma_dev:centos7.4.1708

host$ docker exec -it gang_rdma_dev bash

docker-root$ ./k8s_freeflow/build-router.sh

host$ docker tag gangliao/rdma_dev:centos7.4.1708 gangliao/freeflow-router:centos7.4.1708
# host$ docker push
```



## router-ip-checker

```bash
go run etcd.go -cacert=/etc/kubernetes/ssl/ca.pem -cert=/etc/etcd/ssl/etcd.pem -key=/etc/etcd/ssl/etcd-key.pem
```
