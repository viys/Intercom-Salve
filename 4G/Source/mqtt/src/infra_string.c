#include "kingcstd.h"
#include "infra_string.h"

int8 infra_hex2char(uint8 hex)
{
    hex = hex & 0xF;
    return (int8)(hex < 10 ? '0' + hex : hex - 10 + 'a');
}

void infra_hex2str(uint8 *input, uint16 input_len, char *output)
{
    char *zEncode = "0123456789ABCDEF";
    int i = 0, j = 0;

    for (i = 0; i < input_len; i++) {
        output[j++] = zEncode[(input[i] >> 4) & 0xf];
        output[j++] = zEncode[(input[i]) & 0xf];
    }
}

void infra_int2str(uint32 input, char output[10])
{
    uint8 i = 0, j = 0;
    char tmp[10] = {0};

    do {
        tmp[i++] = input%10 + '0';
    }while((input/=10)>0);

    do {
        output[--i] = tmp[j++];
    }while(i > 0);
}

char *infra_strtok(char *str, const char *delim)
{
    int only_delim = 1;
    static char *pos = NULL;
    static char *target = NULL;

    pos = (str == NULL)?(pos):(str);

    if (pos == NULL || delim == NULL ||
        strlen(pos) <= strlen(delim)) {
        return NULL;
    }

    target = pos;
    while (strlen(pos) >= strlen(delim)) {
        if (memcmp(pos,delim,strlen(delim)) != 0) {
            only_delim = 0;
            pos++;
            continue;
        }

        if (strlen(pos) == strlen(delim)) {
            memset(pos,0,strlen(delim));
            if (only_delim) {
                return NULL;
            }
            return target;
        }

        if (target == pos) {
            pos += strlen(delim);
            target = pos;
        }else{
            memset(pos,0,strlen(delim));
            pos += strlen(delim);
            break;
        }
    }

    if (target) {
        return target;
    }

    return NULL;
}
