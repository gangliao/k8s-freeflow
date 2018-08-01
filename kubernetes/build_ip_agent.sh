#!/bin/bash

set -xe

export http_proxy=http://10.130.14.129:8080
docker build --build-arg http_proxy=$http_proxy \
             --build-arg https_proxy=$http_proxy \
             --build-arg HTTP_PROXY=$http_proxy \
             --build-arg HTTPS_PROXY=$http_proxy \
             -t gangliao/golang:1.10 -f ./Dockerfile .
