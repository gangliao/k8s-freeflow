#!/bin/bash

set -xe

export http_proxy=http://10.130.14.129:8080
export https_proxy=http://10.130.14.129:8080

export GOBIN=$GOPATH/bin

go get .

go run ./ip2etcd.go \
    -cacert="/go/cluster-data/ca.pem" \
    -kubeconfig="/go/cluster-data/config" \
    -endpoints="https://10.142.104.73:2379"
