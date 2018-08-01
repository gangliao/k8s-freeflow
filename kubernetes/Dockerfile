FROM golang:1.10

MAINTAINER Gang Liao <gangliao@cs.umd.edu>

COPY config /root/.kube/
COPY ca.pem /etc/kubernetes/ssl/
COPY ip2etcd.sh /go/
COPY ip2etcd.go /go/

ENV GOBIN="$GOPATH/bin"
RUN go get .

ENTRYPOINT [ "ip2etcd.sh" ]
