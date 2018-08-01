#!/bin/bash

set -xe

go run ./ip2etcd.go \
    -cacert="/go/cluster-data/ca.pem" \
    -kubeconfig="/go/cluster-data/config" \
    -endpoints="https://10.142.104.73:2379"
