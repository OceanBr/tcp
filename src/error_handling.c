#include <stdio.h>
#include <stdlib.h><stdlib.h>
#include "../include/error_handling.h"

void error_handling(char *buf){
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}