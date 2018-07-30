cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(freeflow-router VERSION 0.1.0)

set(FFROUTER_SOURCE_DIR   ${CMAKE_CURRENT_SOURCE_DIR})
set(FFROUTER_BINARY_DIR   ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/bazel.cmake/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(bazel)
include(jsoncpp)

cc_binary(router
          SRCS main.cpp ffrouter.cpp rdma_api.cpp verbs_cmd.cpp shared_memory.cpp
          DEPS rdmacm ibverbs)