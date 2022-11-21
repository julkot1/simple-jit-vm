#include "include/bc_parser.h"
#include <stdio.h>
#include <stdlib.h>
program parse_program_bc(const char *file_path)
{
    program pr = {};
    FILE *fd = fopen(file_path, "r");
    size_t len = 0;
    char *line = NULL;
    while (getline(&line, &len, fd) != -1)
    {
        parse_section(fd, get_section(line), &pr);
    }
    fclose(fd);

    return pr;
}
file_section get_section(const char *str)
{
    if (strcmp(str, TOKEN_SECTION_GLOBAL) == 0)
        return SECTION_GLOBAL;
    else if (strcmp(str, TOKEN_SECTION_DATA) == 0)
        return SECTION_DATA;
    return SECTION_GLOBAL;
}
void parse_section(FILE *fd, file_section section, program *pr)
{
    if (section == SECTION_DATA)
        parse_data(fd, pr);
    if (section == SECTION_GLOBAL)
        pr->global = parse_global(fd);
}

void parse_data(FILE *fd, program *pr)
{
    size_t len = 0;
    char *line = NULL;
    do
    {
        getline(&line, &len, fd);
        if (*line == '\n')
            break;
        char *name = strtok(line, " ");
        char *val = strtok(NULL, "");
        val[strlen(val) - 1] = '\0';
        parse_data_value(name, val, pr);
    } while (1);
}
void parse_data_value(const char *name, const char *val, program *pr)
{
    if (strcmp(name, TOKEN_DATA_LABELS) == 0)
        pr->labels_size = atoi(val);
    else if (strcmp(name, TOKEN_DATA_STACK_SIZE) == 0)
        pr->stack_size = atoi(val);
}
op_node *parse_global(FILE *fd)
{
    size_t len = 0;
    op_node *root = malloc(sizeof(op_node));
    op_node *tmp = root;
    char *line = NULL;
    do
    {
        getline(&line, &len, fd);
        if (*line == '\n')
            break;
        line[len - 2] = '\0';
        tmp->op = parse_operation(line);
        tmp->next = malloc(sizeof(op_node));
        tmp = tmp->next;
    } while (1);
    return root;
}
operation parse_operation(char *line)
{
    operation op;
    char *op_str = opcode_str(line);
    op.code = str_to_opcode(op_str);
    if (is_payload_operation(op.code))
    {
        op.payload = atoi(strtok(NULL, "\n"));
    }
    return op;
}
char *opcode_str(char *str)
{
    char *op_str = strtok(str, " ");
    size_t len = strlen(op_str);
    if (op_str[len - 1] == '\n')
        op_str[len - 1] = '\0';
    return op_str;
}
int is_payload_operation(opcode op)
{
    return op == BIN_PUSH || op == BIN_LABEL || op == BIN_JMP || op == BIN_JMP_IF || op == BIN_JMP_IF_NOT;
}
opcode str_to_opcode(const char *str)
{
    if (strcmp(str, TOKEN_PUSH) == 0)
        return BIN_PUSH;
    else if (strcmp(str, TOKEN_ADD) == 0)
        return BIN_ADD;
    else if (strcmp(str, TOKEN_SUB) == 0)
        return BIN_SUB;
    else if (strcmp(str, TOKEN_DIV) == 0)
        return BIN_DIV;
    else if (strcmp(str, TOKEN_MUL) == 0)
        return BIN_MUL;
    else if (strcmp(str, TOKEN_DUMP) == 0)
        return BIN_DUMP;
    else if (strcmp(str, TOKEN_POP) == 0)
        return BIN_POP;
    else if (strcmp(str, TOKEN_DUP) == 0)
        return BIN_DUP;
    else if (strcmp(str, TOKEN_SWAP) == 0)
        return BIN_SWAP;
    else if (strcmp(str, TOKEN_JMP) == 0)
        return BIN_JMP;
    else if (strcmp(str, TOKEN_JMP_IF) == 0)
        return BIN_JMP_IF;
    else if (strcmp(str, TOKEN_JMP_IF_NOT) == 0)
        return BIN_JMP_IF_NOT;
    else if (strcmp(str, TOKEN_LABEL) == 0)
        return BIN_LABEL;
    else if (strcmp(str, TOKEN_EQUAL) == 0)
        return BIN_EQUAL;
    else if (strcmp(str, TOKEN_NOT) == 0)
        return BIN_NOT;
    else if (strcmp(str, TOKEN_NOT_EQUAL) == 0)
        return BIN_NOT_EQUAL;
    else if (strcmp(str, TOKEN_OR) == 0)
        return BIN_OR;
    else if (strcmp(str, TOKEN_AND) == 0)
        return BIN_AND;
    else if (strcmp(str, TOKEN_GRATER) == 0)
        return BIN_GRATER;
    else if (strcmp(str, TOKEN_LOWER) == 0)
        return BIN_LOWER;
    else if (strcmp(str, TOKEN_GRATER_OR_EQUAL) == 0)
        return BIN_GRATER_OR_EQUAL;
    else if (strcmp(str, TOKEN_LOWER_OR_EQUAL) == 0)
        return BIN_LOWER_OR_EQUAL;
    else if (strcmp(str, TOKEN_BITWISE_AND) == 0)
        return BIN_BITWISE_AND;
    else if (strcmp(str, TOKEN_BITWISE_OR) == 0)
        return BIN_BITWISE_OR;
    else if (strcmp(str, TOKEN_BITWISE_XOR) == 0)
        return BIN_BITWISE_XOR;
    else if (strcmp(str, TOKEN_BITWISE_NOT) == 0)
        return BIN_BITWISE_NOT;
    else if (strcmp(str, TOKEN_LEFT_SHIFT) == 0)
        return BIN_LEFT_SHIFT;
    else if (strcmp(str, TOKEN_RIGHT_SHIFT) == 0)
        return BIN_RIGHT_SHIFT;
    else if (strcmp(str, TOKEN_EOP) == 0)
        return BIN_EOP;
    return BIN_EOP;
}