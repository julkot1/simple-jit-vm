#pragma once
#include "main.h"
#include <stdio.h>
#include <string.h>

#define TOKEN_SECTION_GLOBAL "::GLOBAL\n"
#define TOKEN_SECTION_DATA "::DATA\n"
#define TOKEN_SECTION_CONST "::CONST_POOL\n"

#define TOKEN_DATA_STACK_SIZE ".stack_size"
#define TOKEN_DATA_LABELS ".labels"
#define TOKEN_DATA_CONST_SIZE ".const_pool"

#define TOKEN_PUSH "push"
#define TOKEN_ADD "add"
#define TOKEN_SUB "sub"
#define TOKEN_DIV "div"
#define TOKEN_MUL "mul"
#define TOKEN_MOD "mod"
#define TOKEN_DUMP "dump"
#define TOKEN_POP "pop"
#define TOKEN_DUP "dup"
#define TOKEN_SWAP "swap"
#define TOKEN_JMP "jmp"
#define TOKEN_JMP_IF "jme"
#define TOKEN_JMP_IF_NOT "jmn"
#define TOKEN_LABEL "lb"
#define TOKEN_EQUAL "eq"
#define TOKEN_NOT "not"
#define TOKEN_NOT_EQUAL "neq"
#define TOKEN_OR "or"
#define TOKEN_AND "and"
#define TOKEN_GRATER "gr"
#define TOKEN_LOWER "lt"
#define TOKEN_GRATER_OR_EQUAL "ge"
#define TOKEN_LOWER_OR_EQUAL "le"
#define TOKEN_BITWISE_AND "band"
#define TOKEN_BITWISE_OR "bor"
#define TOKEN_BITWISE_XOR "xor"
#define TOKEN_BITWISE_NOT "bnot"
#define TOKEN_LEFT_SHIFT "shl"
#define TOKEN_RIGHT_SHIFT "shr"
#define TOKEN_TYPEOF "typeof"
#define TOKEN_SIZEOF "sizeof"
#define TOKEN_EOP "eop"

#define TOKEN_CONST_POOL_MEMBER '$'
#define TOKEN_VAR_POOL_MEMBER '@'

typedef enum
{
    SECTION_GLOBAL,
    SECTION_DATA,
    SECTION_CONST

} file_section;
program parse_program_bc(const char *file_path);
void parse_data(FILE *fd, program *pr);
void parse_section(FILE *fd, file_section section, program *pr);
void parse_global(FILE *fd, program *pr);
void parse_const_pool(FILE *fd, program *pr);
operation parse_operation(char *line, program *pr);
opcode str_to_opcode(const char *str);
char *opcode_str(char *str);
file_section
get_section(const char *str);
void parse_data_value(const char *name, const char *val, program *pr);
int is_payload_operation(opcode op);