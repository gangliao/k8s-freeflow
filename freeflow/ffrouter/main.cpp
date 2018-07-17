// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "constant.h"
#include "ffrouter.h"

int main(int argc, char **argv)
{
    char *router_name = NULL;
    if (argc < 2)
    {
        printf("WARNING: router name not specified. Using \"ffrouter\"\n");
        router_name = "ffrouter";
    }
    else
    {
        router_name = argv[1];
    }

    FreeFlowRouter ffr(router_name);
    ffr.start();
}
