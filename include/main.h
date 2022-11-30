#ifndef MAIN
#define MAIN

#include <jit/jit.h>

typedef enum
{
    BIN_PUSH,
    BIN_ADD,
    BIN_SUB,
    BIN_DIV,
    BIN_MUL,
    BIN_DUMP,
    BIN_POP,
    BIN_DUP,
    BIN_SWAP,
    BIN_JMP,
    BIN_JMP_IF,
    BIN_JMP_IF_NOT,
    BIN_LABEL,
    BIN_EQUAL,
    BIN_NOT,
    BIN_NOT_EQUAL,
    BIN_OR,
    BIN_AND,
    BIN_GRATER,
    BIN_LOWER,
    BIN_GRATER_OR_EQUAL,
    BIN_LOWER_OR_EQUAL,
    BIN_BITWISE_AND,
    BIN_BITWISE_OR,
    BIN_BITWISE_XOR,
    BIN_BITWISE_NOT,
    BIN_LEFT_SHIFT,
    BIN_RIGHT_SHIFT,
    BIN_TYPEOF,
    BIN_EOP
} opcode;
typedef enum
{
    NUMBER,
    STRING,
    BOOL,
    CHAR,
    PTR,
    TYPE
} type;
typedef union
{
    int number;
    char byte;
    char *str;
    void *ptr;
    unsigned long all;
} payload_value;

typedef struct
{
    opcode code;
    type payload_type;
    payload_value payload;
} operation;
typedef struct op_node op_node;
struct op_node
{
    operation op;
    op_node *next;
};

typedef struct
{
    op_node *global;
    int labels_size;
    int stack_size;
    jit_label_t *labels;
} program;

int main();
#endif