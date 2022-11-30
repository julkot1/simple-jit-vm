#include "include/main.h"
#include "include/bc_parser.h"
#include "include/jit_parser.h"
#include <jit/jit.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    program p = parse_program_bc("program.bc");
    init(&p);
    parse_program(&p);
    end(&p);
    exit(EXIT_SUCCESS);
}