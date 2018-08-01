#!/bin/bash

set -xe

export http_proxy=http://10.130.14.129:8080

docker run -it -d -b `pwd`:/go/ golang:1.10 --name gang_golang

docker exec -it gang_golang "/go/run_ip2etcd.sh"
