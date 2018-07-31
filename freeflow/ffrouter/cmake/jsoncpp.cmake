# MIT License

# Copyright (c) 2018 Gang Liao <gangliao@cs.umd.edu>

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

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
