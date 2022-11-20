#ifndef MAIN
#define MAIN
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
    BIN_EOP
} opcode;
typedef enum
{
    NUMBER,
    STRING,
    BOOL,
    CHAR,
    PTR,
} type;
typedef struct
{
    opcode code;
    int payload;
} operation;

int main();
#endif