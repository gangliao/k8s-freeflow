# Copyright (c) 2018 Gang Liao <gangliao@cs.umd.edu> All Rights Reserve.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

INCLUDE(ExternalProject)

SET(JSONCPP_SOURCES_DIR ${CMAKE_CURRENT_BINARY_DIR}/extern_jsoncpp)
SET(JSONCPP_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/jsoncpp)
SET(JSONCPP_INCLUDE_DIR "${JSONCPP_INSTALL_DIR}/include" CACHE PATH "jsoncpp include directory." FORCE)

IF(WIN32)
    SET(JSONCPP_LIBRARIES "${JSONCPP_INSTALL_DIR}/lib/jsoncpp.lib" CACHE FILEPATH "jsoncpp library." FORCE)
ELSE(WIN32)
    SET(JSONCPP_LIBRARIES "${JSONCPP_INSTALL_DIR}/lib/libjsoncpp.a" CACHE FILEPATH "jsoncpp library." FORCE)
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${JSONCPP_INCLUDE_DIR})

ExternalProject_Add(
    extern_jsoncpp
    ${EXTERNAL_PROJECT_LOG_ARGS}
    GIT_REPOSITORY   https://github.com/open-source-parsers/jsoncpp
    GIT_TAG          1.8.4
    PREFIX           ${JSONCPP_SOURCES_DIR}
    UPDATE_COMMAND   ""
    ${EXTERNAL_PROJECT_CMAKE_ARGS}
    CMAKE_ARGS       -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    CMAKE_ARGS       -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    CMAKE_ARGS       -DCMAKE_AR=${CMAKE_AR}
    CMAKE_ARGS       -DCMAKE_RANLIB=${CMAKE_RANLIB}
    CMAKE_ARGS       -DCMAKE_INSTALL_PREFIX=${JSONCPP_INSTALL_DIR}
    CMAKE_ARGS       -DCMAKE_INSTALL_LIBDIR=${JSONCPP_INSTALL_DIR}/lib
    CMAKE_ARGS       -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    CMAKE_ARGS       -DBUILD_TESTING=OFF
    CMAKE_ARGS       -DCMAKE_BUILD_TYPE=Release
    CMAKE_CACHE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${JSONCPP_INSTALL_DIR}
                     -DCMAKE_INSTALL_LIBDIR:PATH=${JSONCPP_INSTALL_DIR}/lib
                     -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
                     -DCMAKE_BUILD_TYPE:STRING=Release
)

ADD_LIBRARY(jsoncpp STATIC IMPORTED GLOBAL)
SET_PROPERTY(TARGET jsoncpp PROPERTY IMPORTED_LOCATION ${JSONCPP_LIBRARIES})
ADD_DEPENDENCIES(jsoncpp extern_jsoncpp)

MESSAGE(STATUS "jsoncpp library: ${JSONCPP_LIBRARIES}")
