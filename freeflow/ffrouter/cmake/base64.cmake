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

SET(BASE64_SOURCES_DIR ${CMAKE_CURRENT_BINARY_DIR}/extern_base64)
SET(BASE64_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/base64)
SET(BASE64_INCLUDE_DIR "${BASE64_INSTALL_DIR}/include" CACHE PATH "base64 include directory." FORCE)

IF(WIN32)
    SET(BASE64_LIBRARIES "${BASE64_INSTALL_DIR}/lib/base64.lib" CACHE FILEPATH "base64 library." FORCE)
ELSE(WIN32)
    SET(BASE64_LIBRARIES "${BASE64_INSTALL_DIR}/lib/libbase64.a" CACHE FILEPATH "base64 library." FORCE)
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${BASE64_INCLUDE_DIR})

ExternalProject_Add(
    extern_base64
    ${EXTERNAL_PROJECT_LOG_ARGS}
    GIT_REPOSITORY   https://github.com/gangliao/b64.c
    PREFIX           ${BASE64_SOURCES_DIR}
    UPDATE_COMMAND   ""
    ${EXTERNAL_PROJECT_CMAKE_ARGS}
    CMAKE_ARGS       -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    CMAKE_ARGS       -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    CMAKE_ARGS       -DCMAKE_AR=${CMAKE_AR}
    CMAKE_ARGS       -DCMAKE_RANLIB=${CMAKE_RANLIB}
    CMAKE_ARGS       -DCMAKE_INSTALL_PREFIX=${BASE64_INSTALL_DIR}
    CMAKE_ARGS       -DCMAKE_INSTALL_LIBDIR=${BASE64_INSTALL_DIR}/lib
    CMAKE_ARGS       -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    CMAKE_ARGS       -DBUILD_TESTING=OFF
    CMAKE_ARGS       -DCMAKE_BUILD_TYPE=Release
    CMAKE_CACHE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${BASE64_INSTALL_DIR}
                     -DCMAKE_INSTALL_LIBDIR:PATH=${BASE64_INSTALL_DIR}/lib
                     -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
                     -DCMAKE_BUILD_TYPE:STRING=Release
)

ADD_LIBRARY(base64 STATIC IMPORTED GLOBAL)
SET_PROPERTY(TARGET base64 PROPERTY IMPORTED_LOCATION ${BASE64_LIBRARIES})
ADD_DEPENDENCIES(base64 extern_base64)

MESSAGE(STATUS "base64 library: ${BASE64_LIBRARIES}")
