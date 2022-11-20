#include <jit/jit.h>
#include "main.h"
#ifndef PARSER
#define PARSER
jit_context_t context;
jit_function_t GLOBAL_F;
jit_type_t stack_ptr_t;
jit_value_t stack_ptr;
jit_type_t dump_signature;
#define CONST_BYTE(v) (jit_value_create_nint_constant(GLOBAL_F, jit_type_ubyte, v))
#define CONST_INT(v) (jit_value_create_nint_constant(GLOBAL_F, jit_type_int, v))

void init();
void stack_init();
void end();
void parse(operation op);

void op_add();
void op_div();
void op_sub();
void op_mul();
void op_dup();
void op_swap();
void op_dump();
void op_push(operation op);
jit_value_t op_pop();
#endif