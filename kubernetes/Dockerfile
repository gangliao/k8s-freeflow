FROM golang:1.10

MAINTAINER Gang Liao <gangliao@cs.umd.edu>

RUN wget https://raw.githubusercontent.com/gangliao/k8s-freeflow/master/kubernetes/ip2etcd.go -P /

COPY ca.pem /etc/kubernetes/ssl/
COPY ip2etcd.sh /
COPY ip2etcd.go /

RUN cd / && go get .

ENTRYPOINT [ "/ip2etcd.sh" ]
