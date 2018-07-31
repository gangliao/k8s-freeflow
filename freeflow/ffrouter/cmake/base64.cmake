INCLUDE(ExternalProject)

SET(BASE64_SOURCES_DIR ${CMAKE_CURRENT_BINARY_DIR}/extern_base64)

INCLUDE_DIRECTORIES(${BASE64_SOURCES_DIR}/src/extern_base64)

ExternalProject_Add(
    extern_base64
    ${EXTERNAL_PROJECT_LOG_ARGS}
    GIT_REPOSITORY    "https://github.com/littlstar/b64.c"
    GIT_TAG           "7ae6c416f7d88db8454b6a8f0baca540f9188d95"
    PREFIX            ${BASE64_SOURCES_DIR}
    UPDATE_COMMAND    ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND     ""
    INSTALL_COMMAND   ""
    TEST_COMMAND      ""
)

if (${CMAKE_VERSION} VERSION_LESS "3.3.0")
    set(dummyfile ${CMAKE_CURRENT_BINARY_DIR}/base64_dummy.c)
    file(WRITE ${dummyfile} "const char * dummy_base64 = \"${dummyfile}\";")
    add_library(base64 STATIC ${dummyfile})
else()
    add_library(base64 INTERFACE)
endif()

add_dependencies(base64 extern_base64)

MESSAGE(STATUS "base64 head-only library: ${BASE64_SOURCES_DIR}/src/extern_base64/b64.h")
