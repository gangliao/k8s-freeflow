FROM golang:1.10

MAINTAINER Gang Liao <gangliao@cs.umd.edu>

COPY ca.pem /etc/kubernetes/ssl/
COPY ip2etcd.sh /go/
COPY ip2etcd.go /go/

RUN go get .

ENTRYPOINT [ "ip2etcd.sh" ]
