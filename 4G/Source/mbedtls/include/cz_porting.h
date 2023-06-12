#include "kingtls.h"

void* cz_mbedtls_alloc(uint32 n, uint32 size);

void cz_mbedtls_free(void *ptr);

int cz_mbedtls_fprintf(uint32 *stream, const char *format, ...);

int cz_mbedtls_printf(const char *format, ...);

typedef time_t mbedtls_time_t;

//#define MBEDTLS_PLATFORM_CALLOC_MACRO       cz_mbedtls_alloc
//#define MBEDTLS_PLATFORM_FREE_MACRO         cz_mbedtls_free
#define MBEDTLS_PLATFORM_FPRINTF_MACRO      cz_mbedtls_fprintf
#define MBEDTLS_PLATFORM_PRINTF_MACRO       cz_mbedtls_printf

