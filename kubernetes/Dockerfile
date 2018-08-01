FROM golang:1.10

MAINTAINER Gang Liao <gangliao@cs.umd.edu>

ADD ./*  $HOME/

ENV GOBIN="$GOPATH/bin"
RUN go get .

ENTRYPOINT [ "./run_ip2etcd.sh" ]
