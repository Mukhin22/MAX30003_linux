/**
* @copyright (C) 2018 brezzl. GmbH. All rights reserved.
*
* @file vPrintf.c
* @author Yarik Vlasenko
* @date 20 December 2018
*/

#include "Log_dbg.h"

void hexdump_debug(const uint8_t *p_data, uint16_t length)
{
    uint16_t cntr = 0;

    printf("\t");

    while (length) {
        printf("0x");
        printf("%02x", *p_data);
        p_data++;
        cntr++;
        if (!(cntr % HEXDUMP_COLUMN_NUM)) {
            printf("\r\n"); // enter to new line
            printf("\t");
        } else {
            printf("\t"); // TAB
        }
        length--;
    }
    printf("\r\n");
}

int fputc(int ch, FILE *f)
{
    printf("%c\n", ch);
    return ch;
}
