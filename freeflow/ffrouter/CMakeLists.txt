cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(freeflow-router VERSION 0.1.0)

find_package(CURL       REQUIRED)
find_package(Threads    REQUIRED)

set(FFROUTER_SOURCE_DIR   ${CMAKE_CURRENT_SOURCE_DIR})
set(FFROUTER_BINARY_DIR   ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/bazel.cmake/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


include(bazel)
include(jsoncpp)
include(curl)


cc_binary(router SRCS main.cpp ffrouter.cpp rdma_api.cpp verbs_cmd.cpp shared_memory.cpp)
target_link_libraries(router -L/usr/lib rdmacm ibverbs glog curl jsoncpp)

cc_test(etcd_test SRCS etcd_test.cpp DEPS jsoncpp glog curl)
