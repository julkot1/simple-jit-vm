#include "include/main.h"
#include "include/jit_parser.h"
#include <jit/jit.h>
#include <stdio.h>
#include <stdlib.h>

operation pr[] = {
    {BIN_PUSH, 60},
    {BIN_PUSH, 5},
    {BIN_ADD},
    {BIN_EOP}};

int main()
{
    program p = {.main = pr, .labels_size = 1};
    init(&p);
    parse_program(&p);
    end(&p);
    exit(EXIT_SUCCESS);
}