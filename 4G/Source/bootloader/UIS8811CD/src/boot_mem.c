/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#include "boot_app.h"
#include "boot_platform.h"
#include "boot_trace.h"
#include "boot_mem.h"
#include "boot_compiler.h"

void bootShowMemInfo(void)
{
    if(s_bootAppParamPtr == NULL)
        return;
    
    s_bootAppParamPtr->memApi->showMemInfo();
}

void *malloc(unsigned size)
{
    if(s_bootAppParamPtr == NULL)
        return NULL;
    
    return s_bootAppParamPtr->memApi->malloc(size);
}
void *calloc(unsigned nmemb, unsigned size)
{
    if(s_bootAppParamPtr == NULL)
        return NULL;
    
    return s_bootAppParamPtr->memApi->calloc(nmemb, size);
}
void *realloc(void *ptr, unsigned size)
{
    if(s_bootAppParamPtr == NULL)
        return NULL;
    return s_bootAppParamPtr->memApi->realloc(ptr, size);
}
void free(void *ptr)
{
    if(s_bootAppParamPtr != NULL)
        s_bootAppParamPtr->memApi->free(ptr);
}



