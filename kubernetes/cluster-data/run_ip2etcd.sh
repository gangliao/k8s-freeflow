#!/bin/bash
go run ./ip2etcd.go \
    -kubeconfig=/root/.kube/config \
    -endpoints=https://10.142.104.73:2379 \
    -cacert=/etc/kubernetes/ssl/ca.pem
