#include <jit/jit.h>
#include "main.h"
#include "jit_native.h"
#ifndef PARSER
#define PARSER

#define CONST_UBYTE(v) (jit_value_create_nint_constant(GLOBAL_F, jit_type_nint, v))
#define CONST_BYTE(v) (jit_value_create_nint_constant(GLOBAL_F, jit_type_ubyte, v))
#define CONST_INT(v) (jit_value_create_nint_constant(GLOBAL_F, jit_type_nint, v))
#define CONST_LONG(v) (jit_value_create_long_constant(GLOBAL_F, jit_type_long, v))

void init(program *__pr);
void parse_program(program *__pr);
void labels_init(program *__pr);
void stack_init(program *__pr);

void end(program *__pr);
void parse(operation op, program *__pr);
void op_native(native_function native_f);
void op_native_1(native_function native_f);
void op_native_2(native_function native_f);
void op_dup();
void op_swap();
void op_dump();
void op_push(operation op);
void op_label(operation op, jit_label_t *labels);
void op_jmp(operation op, jit_label_t *labels);
void op_jmp_if(operation op, jit_label_t *labels);
void op_jmp_if_not(operation op, jit_label_t *labels);
void push_vm_with_type(jit_value_t val, jit_value_t type);
jit_value_t op_pop();
#endif