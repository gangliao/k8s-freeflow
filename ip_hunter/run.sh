#!/bin/bash

set -xe

docker rm -f $(docker ps -a | awk '$2=="golang:1.10" {print $1}')

docker run --name gang_golang -it -d -v `pwd`:/go/ golang:1.10

docker exec -it gang_golang "./run_ip2etcd.sh"
