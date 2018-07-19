#!/bin/bash
set -xe

cd freeflow

# Install Memory Pool
cd libmempool && make && make install && cd ..

# Free