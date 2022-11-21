#include <jit/jit.h>
#include "main.h"
#ifndef PARSER
#define PARSER

#define CONST_BYTE(v) (jit_value_create_nint_constant(GLOBAL_F, jit_type_ubyte, v))
#define CONST_INT(v) (jit_value_create_nint_constant(GLOBAL_F, jit_type_int, v))

void init(program *__pr);
void parse_program(program *__pr);
void labels_init(program *__pr);
void stack_init(program *__pr);

void end(program *__pr);
void parse(operation op, program *__pr);

void op_add();
void op_div();
void op_sub();
void op_mul();
void op_dup();
void op_swap();
void op_dump();
void op_equal();
void op_not();
void op_or();
void op_and();
void op_not_equal();
void op_grater();
void op_lower();
void op_grater_or_equal();
void op_lower_or_equal();
void op_bitwise_or();
void op_bitwise_and();
void op_bitwise_xor();
void op_bitwise_not();
void op_left_shift();
void op_right_shift();
void op_push(operation op);
void op_label(operation op, jit_label_t *labels);
void op_jmp(operation op, jit_label_t *labels);
void op_jmp_if(operation op, jit_label_t *labels);
void op_jmp_if_not(operation op, jit_label_t *labels);
jit_value_t op_pop();
#endif