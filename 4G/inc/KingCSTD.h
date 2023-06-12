#ifndef __KING_CSTD_H__
#define __KING_CSTD_H__

#include "KingDef.h"

#define PRINTF_MSG_MAX_LEN      512

typedef struct
{
    void* (*KING_memset)(void* dst, int value, size_t count);
    void* (*KING_memcpy)(void *dst, const void *src, size_t count);
    void* (*KING_memmove)( void *dst, const void *src, size_t count);
    int (*KING_memcmp)(const void* src1, const void* src2, size_t count);
    void* (*KING_memchr)(const void* src, int ch, size_t count);
    void* (*KING_memrchr)(const void *src, int ch, size_t count);
    void* (*KING_malloc)(size_t size);    
    void* (*KING_realloc)(void *ptr, size_t size);
    void (*KING_free)(void *ptr);
    size_t (*KING_strlen)(const char* src);
    char* (*KING_strcpy)(char *dst, const char *src);
    char* (*KING_strncpy)( char *dst, const char *src, size_t count );
    char* (*KING_strcat)(char* dst, const char* src);
    char* (*KING_strncat)(char* dst, const char* src,  size_t count);
    int (*KING_strcmp)(const char* str1, const char* str2);
    int (*KING_strncmp)(const char* str1, const char* str2, size_t count);
    char* (*KING_strstr)(const char* str1, const char* str2);
    int (*KING_vsprintf)(char* string, const char* format, va_list arg);
    int (*KING_vsnprintf)(char* str, size_t size, const char* format, va_list ap);
    int (*KING_tolower)(int ch);
    int (*KING_toupper)(int ch);
    int (*KING_atoi)(const char*string);
    long int (*KING_atol)(const char*string);
    int (*KING_rand)(void);
    void (*KING_srand)(unsigned int seed);
    double (*KING_sin)(double x);
    double (*KING_asin)(double x);
    double (*KING_cos)(double x);
    double (*KING_acos)(double x);
    double (*KING_sqrt)(double x);
    double (*KING_tan)(double x);
    double (*KING_exp)(double x);
    double (*KING_log)(double x);
    double (*KING_log10)(double x);
    double (*KING_pow)(double x, double y);
    double (*KING_fabs)(double x);
    bool (*KING_isalpha)(int ch);
    bool (*KING_iscntrl)(int ch);
    bool (*KING_isgraph)(int ch);
    bool (*KING_islower)(int ch);
    bool (*KING_isupper)(int ch);
    bool (*KING_isprint)(int ch);
    bool (*KING_ispunct)(int ch);
    bool (*KING_isspace)(int ch);
    bool (*KING_isxdigit)(int ch);
    wchar* (*KING_wstrcpy)(wchar* dst, const wchar* src);
    wchar* (*KING_wstrncpy)(wchar* dst, const wchar* src, size_t count);
    wchar* (*KING_wstrcat)(wchar* dst, const wchar* src);
    wchar* (*KING_wstrncat)(wchar* dst, const wchar* src, size_t count);
    int (*KING_wstrcmp)(const wchar* src1, const wchar* src2);
    int (*KING_wstrncmp)(const wchar* src1, const wchar* src2, size_t count);
    size_t (*KING_wstrlen)(const wchar* src);
    int (*KING_vsscanf)(const char *s, const char* format, va_list ap);
} KING_CSTD_API_S;

#ifdef USE_KING_SDK_STD_API
extern KING_CSTD_API_S* pCSTDApi;
#define vsprintf            pCSTDApi->KING_vsprintf
#define vsnprintf           pCSTDApi->KING_vsnprintf
//#define memset(dst, value, size) pCSTDApi->KING_memset(dst, value, size)
//#define memcpy pCSTDApi->KING_memcpy
#define memmove             pCSTDApi->KING_memmove
#define memcmp              pCSTDApi->KING_memcmp
#define memchr              pCSTDApi->KING_memchr
#define memrchr             pCSTDApi->KING_memrchr
#define malloc              pCSTDApi->KING_malloc
#define realloc             pCSTDApi->KING_realloc
#define free                pCSTDApi->KING_free
//#define strcpy pCSTDApi->KING_strcpy
#define strncpy             pCSTDApi->KING_strncpy
#define strcat              pCSTDApi->KING_strcat
#define strncat             pCSTDApi->KING_strncat
#define strcmp              pCSTDApi->KING_strcmp
#define strncmp             pCSTDApi->KING_strncmp
#define strstr              pCSTDApi->KING_strstr
#define strlen              pCSTDApi->KING_strlen
#define rand                pCSTDApi->KING_rand
#define srand               pCSTDApi->KING_srand
#define sin(x)              pCSTDApi->KING_sin(x)
#define asin                pCSTDApi->KING_asin
#define cos                 pCSTDApi->KING_cos
#define acos                pCSTDApi->KING_acos
#define sqrt                pCSTDApi->KING_sqrt
#define tan                 pCSTDApi->KING_tan
#define exp                 pCSTDApi->KING_exp
#define log                 pCSTDApi->KING_log
#define log10               pCSTDApi->KING_log10
#define pow                 pCSTDApi->KING_pow
#define fabs                pCSTDApi->KING_fabs
#define isalpha             pCSTDApi->KING_isalpha
#define iscntrl             pCSTDApi->KING_iscntrl
#define isgraph             pCSTDApi->KING_isgraph
#define islower             pCSTDApi->KING_islower
#define isprint             pCSTDApi->KING_isprint
#define ispunct             pCSTDApi->KING_ispunct
#define isspace             pCSTDApi->KING_isspace
#define isupper             pCSTDApi->KING_isupper
#define isxdigit            pCSTDApi->KING_isxdigit
#define tolower             pCSTDApi->KING_tolower
#define toupper             pCSTDApi->KING_toupper
#define atoi                pCSTDApi->KING_atoi
#define atol                pCSTDApi->KING_atol
#define wstrcpy             pCSTDApi->KING_wstrcpy
#define wstrncpy            pCSTDApi->KING_wstrncpy
#define wstrcat             pCSTDApi->KING_wstrcat
#define wstrncat            pCSTDApi->KING_wstrncat
#define wstrcmp             pCSTDApi->KING_wstrcmp
#define wstrncmp            pCSTDApi->KING_wstrncmp
#define wstrlen             pCSTDApi->KING_wstrlen
#define vsscanf             pCSTDApi->KING_vsscanf
//#define raise(x) (0)

int sprintf(char *str, const char *fmt, ...);
int snprintf(char *str, size_t size, const char *fmt, ...);
int sscanf(const char *str, const char *fmt, ...);
char *strcpy(char* dest, const char *src);
char* strchr(const char *src, int ch);
char* strrchr(const char *src, int ch);
void* memset(void *dst, int value, size_t count);
void *memcpy(void *dst, const void *src, size_t count);
int raise(int signum);
void* calloc(size_t nmemb, size_t size);
char *strtok(char *s, const char *delim);
int strcasecmp(char *s1, char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);
double atof(const char *str);
double floor(double x);
double ceil(double x);
double round(double r);
unsigned long strtoul(const char *nptr, char **endptr, int base);
long strtol(const char *nptr, char **endptr, unsigned int base);

#endif /* USE_KING_SDK_STD_API */

#endif

