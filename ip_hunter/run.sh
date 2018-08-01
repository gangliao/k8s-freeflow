#!/bin/bash

set -xe

docker run -it -d -v `pwd`:/go/ golang:1.10 --name gang_golang

docker exec -it gang_golang "./run_ip2etcd.sh"
