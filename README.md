# k8s-freeflow


## Build Dev Image

```bash
cat <<EOF
============================================
Building Dev Image ...
============================================
EOF

export http_proxy=http://10.130.14.129:8080
docker build --build-arg http_proxy=$http_proxy \
             --build-arg https_proxy=$http_proxy \
             --build-arg HTTP_PROXY=$http_proxy \
             --build-arg HTTPS_PROXY=$http_proxy \
             -t rdma_dev:latest -f ./Dockerfile .
```

## Build FreeFlow in Docker

```bash
cat <<EOF
============================================
Building FreeFlow in Image ...
============================================
EOF

docker rm -f $(docker ps -a | awk '$2=="rdma_dev:latest" {print $1}')

docker run --net=host --name gang_rdma_dev --privileged -d -it \
    -v `pwd`:/k8s_freeflow -v /sys/class/:/sys/class/ -v /dev/:/dev/ \
    --device=/dev/infiniband/uverbs0 --device=/dev/infiniband/rdma_cm \
    rdma_dev:latest

docker exec -it gang_rdma_dev bash
```


```bash
go run etcd.go -cacert=/etc/kubernetes/ssl/ca.pem -cert=/etc/etcd/ssl/etcd.pem -key=/etc/etcd/ssl/etcd-key.pem
```


yum install python-devel make lsof redhat-rpm-config rpm-build ethtool kernel-devel-3.10.0-693.el7.x86_64 createrepo

export http_proxy=http://10.130.14.129:8080
yum install pciutils numactl-libs gtk2 atk cairo gcc-gfortran tcsh lsof ethtool tcl tk
libtool
ofed_info | grep MLNX_OFED_LINUX-4


docker run --net=host --name gang_rdma_dev --privileged -d -it \
  -v `pwd`:/k8s_freeflow -v /sys/class/:/sys/class/ -v /dev/:/dev/ \
  -v /lib/modules/3.10.0-693.el7.x86_64/:/lib/modules/3.10.0-693.el7.x86_64/ \
  -v /usr/src/kernels/3.10.0-693.el7.x86_64/:/usr/src/kernels/3.10.0-693.el7.x86_64/ \
  --device=/dev/infiniband/uverbs0 --device=/dev/infiniband/rdma_cm \
  rdma_dev:latest


  -v /lib/modules/3.10.0-693.el7.x86_64/build/:/lib/modules/3.10.0-693.el7.x86_64/build/ \