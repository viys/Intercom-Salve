#include "cz_porting.h"

void* cz_mbedtls_alloc(uint32 n, uint32 size)
{
    int ret = -1;
    void *p = NULL;
    
    ret = KING_MemAlloc(&p, n * size);
    
    //KING_SysLog("cz_mbedtls_alloc: ret=%d", ret);
    if (ret == 0 && p != NULL)
    {
        memset(p, 0, n * size);
    }
    return p;
}

void cz_mbedtls_free(void *ptr)
{
    if (ptr != NULL)
        KING_MemFree(ptr);
}


int cz_mbedtls_fprintf(uint32 *stream, const char *format, ...)
{
    KING_SysLog(format);

    return 0;
}

int cz_mbedtls_printf(const char *format, ...)
{
    KING_SysLog(format);

    return 0;
}




