/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "app_main.h"
#include "kingcstd.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("csdt: "fmt, ##args); } while(0)

/*******************************************************************************
 ** Functions
 ******************************************************************************/
void cstd_test(void)
{
    int ret = -1;
    char dstStr[60];
    char *temStr = NULL;

    memset(dstStr, 0x00, 60);
    memcpy(dstStr, "123456789qwertyuiasdfghwertfgh", strlen("123456789qwertyuiasdfghwertfgh"));
    LogPrintf("cstd_test() KING_memcpy dstStr %s", dstStr);
    memset(dstStr, 0x00, 60);
    memmove(dstStr, "123456789qwertyuiasdfghwertfgh", 15);
    LogPrintf("cstd_test() KING_memmove dstStr %s", dstStr);
    ret = memcmp("12345", "3456", 5);
    LogPrintf("cstd_test() KING_memcmp ret %d", ret);
    ret = memcmp("12345", "1234567", 5);
    LogPrintf("cstd_test() 11 KING_memcmp ret %d", ret);
    temStr = memchr("QAZWSXEDCRFVTGBYHN", 'W', 10);
    if (temStr != NULL)
    {
        LogPrintf("cstd_test() KING_memchr temStr %s", temStr);
    }
    temStr = memrchr("QAZWSXEDCRFVTGBWYHN", 'W', 10);
    if (temStr != NULL)
        LogPrintf("cstd_test() KING_memrchr temStr %s", temStr);
    
    temStr = memrchr("QAZWSXEDCRFVTGBWYHN", 'X', 10);
    if (temStr != NULL)
        LogPrintf("cstd_test() 11 KING_memrchr temStr %s", temStr);
    memset(dstStr, 0x00, 60);
    strcpy(dstStr, "XCVBASDFGQWERTY");
    if (dstStr != NULL)
        LogPrintf("cstd_test() KING_strcpy dstStr %s", dstStr);
    
    memset(dstStr, 0x00, 60);
    strncpy(dstStr, "XCVBASDFGQWERTYGJJTT", strlen("XCVBASDFGQWERTYGJJTT"));
    if (dstStr != NULL)
        LogPrintf("cstd_test() KING_strncpy dstStr %s", dstStr);

    memset(dstStr, 0x00, 60);
    strcat(dstStr, "XCVBASDFGQWERTYGJJTT");
    if (dstStr != NULL)
        LogPrintf("cstd_test() KING_strcat dstStr %s", dstStr);

    memset(dstStr, 0x00, 60);
    strncat(dstStr, "XCVBASDFGQWERTYGJJTT", strlen("XCVBASDFGQWERTYGJJTT"));
    if (dstStr != NULL)
        LogPrintf("cstd_test() KING_strncat dstStr %s", dstStr);

    ret = strcmp("12345", "3456");
    LogPrintf("cstd_test() KING_strcmp ret %d", ret);
    ret = strcmp("12345", "12345");
    LogPrintf("cstd_test() 11 KING_strcmp ret %d", ret);

    ret = strncmp("12345", "3456", 5);
    LogPrintf("cstd_test() KING_strncmp ret %d", ret);
    ret = strncmp("12345", "12345", 3);
    LogPrintf("cstd_test() 11 KING_strncmp ret %d", ret);

    temStr = strstr("QAZWSXEDCRFVTGBYHN", "W");
    if (temStr != NULL)
    {
        LogPrintf("cstd_test() KING_strstr temStr %s", temStr);
    }
    memset(dstStr, 0x00, 60);
    sprintf(dstStr, "ret %d", ret);
    LogPrintf("cstd_test() KING_sprintf dstStr %s", dstStr);
    
    memset(dstStr, 0x00, 60);
    snprintf(dstStr, 60, "ret %d", ret);
    LogPrintf("cstd_test() KING_snprintf dstStr %s", dstStr);
    
    LogPrintf("cstd_test() KING_tolower ret %d", tolower('D'));

    LogPrintf("cstd_test() 11 KING_tolower ret %d", tolower('d'));
    
    LogPrintf("cstd_test() KING_toupper ret %d", toupper('D'));

    LogPrintf("cstd_test() 11 KING_toupper ret %d", toupper('d'));
    
    LogPrintf("cstd_test() KING_atoi ret %d", atoi("12"));
    
    LogPrintf("cstd_test() KING_atol ret %ld", atol("124df"));

    LogPrintf("cstd_test() KING_rand ret %d", rand());
    
    LogPrintf("cstd_test() KING_fabs 0  %lf", fabs(0));
    LogPrintf("cstd_test() KING_fabs 1  %lf", fabs(1456));
    LogPrintf("cstd_test() KING_fabs 2  %lf", fabs(-23));
    LogPrintf("cstd_test() KING_isalpha %d", isalpha((int)'a'));
    LogPrintf("cstd_test() 1 KING_isalpha %d", isalpha((int)'*'));
    LogPrintf("cstd_test() KING_iscntrl %d", iscntrl(0x7F));
    LogPrintf("cstd_test() 1 KING_iscntrl %d", iscntrl(0x50));
    LogPrintf("cstd_test() KING_isgraph %d", isgraph('3'));
    LogPrintf("cstd_test() 1 KING_isgraph %d", isgraph(' '));
    LogPrintf("cstd_test() KING_islower %d", islower((int)'A'));
    LogPrintf("cstd_test() 1 KING_islower %d", islower((int)'a'));
    LogPrintf("cstd_test() KING_isupper %d", isupper((int)'A'));
    LogPrintf("cstd_test() 1 KING_isupper %d", isupper((int)'a'));
    LogPrintf("cstd_test() KING_isprint %d", isprint('\t'));
    LogPrintf("cstd_test() 1 KING_isprint %d", isprint('4'));
    LogPrintf("cstd_test() KING_ispunct %d", ispunct('t'));
    LogPrintf("cstd_test() 1 KING_ispunct %d", ispunct('/'));
    LogPrintf("cstd_test() KING_isspace %d", isspace(' '));
    LogPrintf("cstd_test() 1 KING_isspace %d", isspace('f'));

    int i;
    int *pn=(int *)calloc(10, sizeof(int));
    if (pn == NULL)
    {
       LogPrintf("cstd_test() calloc pn is fail!"); 
    }
    else 
    {
        for (i  = 0; i < 10; i++)
        {
            LogPrintf("cstd_test() calloc 11 ret %3d", pn[i]);
        }  
    }
    ret = strcasecmp("dfghj", "DFGHJ");
    LogPrintf("cstd_test() strcasecmp ret %d", ret);
    ret = strcasecmp("dfgHj", "DFgHJ");
    LogPrintf("cstd_test() strcasecmp 11 ret %d", ret);
    
    ret = strncasecmp("dfghjkuoyrewe", "DFGHJ", 5);
    LogPrintf("cstd_test() strncasecmp ret %d", ret);
    ret = strncasecmp("dfgHj", "DFgHJvxsaffs", 7);
    LogPrintf("cstd_test() strncasecmp 11 ret %d", ret);
    
    float val;
    char str[20];

    strcpy(str, "98993489");
    val = atof(str);
    LogPrintf("cstd_test() atof str %s, val=%f", str, val);

    strcpy(str, "runoob");
    val = atof(str);
    LogPrintf("cstd_test() atof 11 str %s, val=%f", str, val);

    double k = floor(2.2);
    double h = floor(-2.2);
    
    LogPrintf("cstd_test() floor k=%f", k);
    LogPrintf("cstd_test() floor h=%f", h);

    double x = 2.499999;
    float y = 2.8f;

    LogPrintf("cstd_test() round(%f) is %.0f\n", x, round(x));
    LogPrintf("cstd_test() round(%f) is %.0f\n", -x, round(-x));
    LogPrintf("cstd_test() round(%f) is %.0f\n", y, round(y));
    LogPrintf("cstd_test() round(%f) is %.0f\n", -y, round(-y));


    LogPrintf("---- %s start test sscanf ----", __func__);
    const char *src_str = "The avg height of the 3 students is 1.54m !!";
    int student_num = 0;
    double height = 0.0;

    ret = sscanf(src_str, "The avg height of the %d students is %lfm !!", &student_num, &height);
    if (ret > 0)
    {
        LogPrintf("from \"%s\", we get student_num = %d, avg height = %.2fm", src_str, student_num, height);
    }
    else
    {
        LogPrintf("sscanf fail");
    }
}
