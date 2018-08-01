FROM golang:1.10

MAINTAINER Gang Liao <gangliao@cs.umd.edu>

RUN wget https://raw.githubusercontent.com/gangliao/k8s-freeflow/master/kubernetes/ip2etcd.go -P /

COPY ca.pem /etc/kubernetes/ssl/

ENTRYPOINT [ "go run ./ip2etcd.go -cacert=/etc/kubernetes/ssl/ca.pem" ]
