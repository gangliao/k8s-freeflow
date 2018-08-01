#!/bin/bash
go run ./ip2etcd.go \
    -cacert=./cluster-data/ca.pem \
    -kubeconfig=./cluster-data/config \
    -endpoints=https://10.142.104.73:2379
