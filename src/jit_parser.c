#include "include/jit_parser.h"
#include "include/vm.h"
#include <stdlib.h>
#include "include/jit_native.h"
jit_context_t context;
jit_function_t GLOBAL_F;
jit_value_t stack_ptr;
jit_type_t dump_signature;

void stack_init(program *__pr)
{
    jit_type_t params[] = {jit_type_int};
    dump_signature = jit_type_create_signature(
        jit_abi_cdecl, jit_type_int, params, 1, 1);

    stack_ptr = jit_value_create(GLOBAL_F, jit_type_void_ptr);
    jit_insn_store(GLOBAL_F, stack_ptr, jit_insn_alloca(GLOBAL_F, CONST_INT(__pr->meta.stack_size * stack_element_size)));
}

void init(program *__pr)
{
    jit_init();
    context = jit_context_create();
    GLOBAL_F = jit_function_create(context, jit_type_create_signature(jit_abi_cdecl, jit_type_int, NULL, 0, 1));
    types_init();
    stack_init(__pr);
    labels_init(__pr);
}
void parse_program(program *__pr)
{
    op_node *op_n = __pr->global;
    for (size_t i = 0; i < __pr->meta.operations_size; i++)
    {
        parse((__pr->global)[i], __pr);
    }
}
void labels_init(program *__pr)
{
    __pr->labels = malloc(__pr->meta.labels_size * sizeof(jit_label_t));
    for (size_t i = 0; i < __pr->meta.labels_size; i++)
    {
        __pr->labels[i] = jit_label_undefined;
    }
}

void end(program *__pr)
{
    push_vm(CONST_INT(0));
    // jit_dump_function(stdout, GLOBAL_F, "GLOBAL_F [uncompiled]");
    jit_function_compile(GLOBAL_F);

    // jit_dump_function(stdout, GLOBAL_F, "GLOBAL_F [compiled]");
    jit_function_apply(GLOBAL_F, NULL, NULL);
    jit_context_destroy(context);
    free(__pr->labels);
}
void printt(struct stack_element a)
{
    if (a.t == NUMBER)
        printf("<%d> %d\n", a.t, a.val.number);
    else if (a.t == PTR)
        printf("<%d>:<%d> %s \n", a.t, ((pool_element *)a.val.ptr)->type, (char *)((pool_element *)a.val.ptr)->val);
    return;
}
void op_printff(jit_value_t aa)
{

    jit_type_t puts_signature = jit_type_create_signature(
        jit_abi_cdecl, jit_type_void, &stack_element, 1, 1);
    jit_insn_call_native(
        GLOBAL_F, "printt", printt, puts_signature, &aa, 1, JIT_CALL_NOTHROW);
}

void push_vm(jit_value_t val)
{

    jit_insn_store_relative(GLOBAL_F, stack_ptr, 0, val);
    jit_value_t new_ptr = jit_insn_add(GLOBAL_F, stack_ptr, CONST_INT(stack_element_size));
    jit_insn_store(GLOBAL_F, stack_ptr, new_ptr);
}
void push_vm_with_type(jit_value_t val, jit_value_t type)
{
    jit_insn_store_relative(GLOBAL_F, stack_ptr, 0, type);
    jit_insn_store_relative(GLOBAL_F, stack_ptr, stack_element_value_size, val);
    jit_value_t new_ptr = jit_insn_add(GLOBAL_F, stack_ptr, CONST_INT(stack_element_size));
    jit_insn_store(GLOBAL_F, stack_ptr, new_ptr);
}

void op_push(operation op)
{
    jit_insn_store_relative(GLOBAL_F, stack_ptr, 0, CONST_INT(op.payload_type));
    jit_insn_store_relative(GLOBAL_F, stack_ptr, stack_element_value_size, CONST_LONG(op.payload.all));
    jit_value_t new_ptr = jit_insn_add(GLOBAL_F, stack_ptr, CONST_UBYTE(stack_element_size));
    jit_insn_store(GLOBAL_F, stack_ptr, new_ptr);
}

jit_value_t op_pop()
{
    jit_value_t val = jit_insn_load_relative(GLOBAL_F, stack_ptr, -stack_element_size, stack_element);
    jit_value_t new_ptr = jit_insn_sub(GLOBAL_F, stack_ptr, CONST_INT(stack_element_size));
    jit_insn_store(GLOBAL_F, stack_ptr, new_ptr);
    return val;
}
void op_swap()
{
    jit_value_t a = op_pop();
    jit_value_t b = op_pop();
    push_vm(a);
    push_vm(b);
}
void op_dup()
{
    jit_value_t a = op_pop();
    push_vm(a);
    push_vm(a);
}
void op_dump()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, jit_type_long);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);
    jit_insn_call_native(GLOBAL_F, "putchar", putchar, dump_signature, &a_val, 1, JIT_CALL_NOTHROW);
}
void op_printf()
{

    jit_value_t a = op_pop();
    op_printff(a);
}
void op_label(operation op, jit_label_t *labels)
{
    jit_insn_label(GLOBAL_F, &(labels[op.payload.number]));
}
void op_jmp(operation op, jit_label_t *labels)
{

    jit_insn_branch(GLOBAL_F, &(labels[op.payload.number]));
}
void op_jmp_if(operation op, jit_label_t *labels)
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, jit_type_long);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);
    jit_value_t eq = jit_insn_eq(GLOBAL_F, a_val, CONST_INT(0));
    jit_insn_branch_if(GLOBAL_F, eq, &(labels[op.payload.number]));
}
void op_jmp_if_not(operation op, jit_label_t *labels)
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, jit_type_long);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);
    jit_value_t eq = jit_insn_eq(GLOBAL_F, a_val, CONST_INT(0));
    jit_insn_branch_if_not(GLOBAL_F, eq, &(labels[op.payload.number]));
}

void op_native(native_function native_f)
{
    if (native_f.args == 1)
        op_native_1(native_f);
    else if (native_f.args == 2)
        op_native_2(native_f);
}
void op_native_1(native_function native_f)
{
    jit_value_t a = op_pop();
    jit_value_t res = jit_insn_call_native(
        GLOBAL_F, native_names[native_f.name], native_f.function, native_f.signature, &a, native_f.args, JIT_CALL_NOTHROW);
    push_vm(res);
}
void op_native_2(native_function native_f)
{
    jit_value_t a = op_pop();
    jit_value_t b = op_pop();
    jit_value_t args[] = {b, a};
    jit_value_t res = jit_insn_call_native(
        GLOBAL_F, native_names[native_f.name], native_f.function, native_f.signature, args, native_f.args, JIT_CALL_NOTHROW);
    push_vm(res);
}
void parse(operation op, program *__pr)
{
    switch (op.code)
    {

    case BIN_PUSH:
        op_push(op);
        break;
    case BIN_POP:
        op_pop();
        break;
    case BIN_DUP:
        op_dup();
        break;
    case BIN_SWAP:
        op_swap();
        break;
    case BIN_DUMP:
        op_printf();
        break;
    case BIN_LABEL:
        op_label(op, __pr->labels);
        break;
    case BIN_JMP:
        op_jmp(op, __pr->labels);
        break;
    case BIN_JMP_IF:
        op_jmp_if(op, __pr->labels);
        break;
    case BIN_JMP_IF_NOT:
        op_jmp_if_not(op, __pr->labels);
        break;
    default:
        op_native(native_functions[op.code]);
        break;
    }
}
