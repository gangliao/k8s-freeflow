// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <map>
#include <string>

#include "constant.h"
#include "log.h"

class ShmPiece
{
   public:
    std::string name;
    int size;
    int shm_fd;
    void* ptr;

    ShmPiece(const char* name, int size);
    ~ShmPiece();

    bool open();
    void remove();
};

#endif /* SHARED_MEMORY_H */
