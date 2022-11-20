#include "include/main.h"
#include "include/jit_parser.h"
#include <jit/jit.h>
#include <stdio.h>
#include <stdlib.h>

operation program[] = {
    {BIN_PUSH, 60},
    {BIN_PUSH, 5},
    {BIN_ADD},
    {BIN_DUP},
    {BIN_DUMP},
    {BIN_PUSH, 1},
    {BIN_ADD},
    {BIN_DUMP},
    {BIN_PUSH, 10},
    {BIN_DUMP},
    {BIN_EOP}};
const int size = 11;

void save()
{
    FILE *fd = fopen("program.bin", "wb");
    fwrite(&size, sizeof(int), 1, fd);
    fwrite(program, sizeof(operation), size, fd);
    fclose(fd);
}
int main()
{
    save();
    init();
    operation *op = &program[0];
    while ((op)->code != BIN_EOP)
    {
        parse(*op);
        op++;
    }

    // jit_dump_function(stdout, GLOBAL_F, "GLOBAL_F [uncompiled]");
    jit_function_compile(GLOBAL_F);
    // jit_dump_function(stdout, GLOBAL_F, "GLOBAL_F [compiled]");
    jit_function_apply(GLOBAL_F, NULL, NULL);
    jit_context_destroy(context);
    exit(EXIT_SUCCESS);
}