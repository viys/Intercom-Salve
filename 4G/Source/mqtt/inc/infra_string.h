#ifndef _INFRA_STRING_H_
#define _INFRA_STRING_H_

#include "kingdef.h"

int8 infra_hex2char(uint8 hex);
void infra_hex2str(uint8 *input, uint16 input_len, char *output);
void infra_int2str(uint32 input, char output[10]);
char *infra_strtok(char *str, const char *delim);
int infra_randstr(char *random, int length);

#endif

