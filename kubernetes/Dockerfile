FROM golang:1.10

MAINTAINER Gang Liao <gangliao@cs.umd.edu>

COPY cluster-data/config /root/.kube/
COPY cluster-data/ca.pem /etc/kubernetes/ssl/
COPY cluster-data/run_ip2etcd.sh /go/
COPY ip2etcd.go /go/

ENV GOBIN="$GOPATH/bin"
RUN go get .

ENTRYPOINT [ "./run_ip2etcd.sh" ]
