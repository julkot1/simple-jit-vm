#include "include/jit_parser.h"
#include "include/vm.h"
void stack_init()
{
    jit_type_t type_cstring = jit_type_create_pointer(jit_type_sys_char, 2);
    jit_type_t params[] = {jit_type_int};
    dump_signature = jit_type_create_signature(
        jit_abi_cdecl, jit_type_int, params, 1, 1);

    stack_ptr_t = jit_type_create_pointer(jit_type_sys_int, 4);
    stack_ptr = jit_value_create(GLOBAL_F, jit_type_void_ptr);
    jit_insn_store(GLOBAL_F, stack_ptr, jit_insn_alloca(GLOBAL_F, CONST_INT(STACK_SIZE)));
}
void init()
{
    jit_init();
    context = jit_context_create();
    GLOBAL_F = jit_function_create(context, jit_type_create_signature(jit_abi_cdecl, jit_type_int, NULL, 0, 1));
    stack_init();
}
void end()
{
    jit_context_destroy(context);
}
void push_vm(jit_value_t val)
{
    jit_insn_store_relative(GLOBAL_F, stack_ptr, 0, val);
    jit_value_t new_ptr = jit_insn_add(GLOBAL_F, stack_ptr, CONST_INT(ELEMENT_SIZE));
    jit_insn_store(GLOBAL_F, stack_ptr, new_ptr);
}
void op_push(operation op)
{
    push_vm(CONST_INT(op.payload));
}

jit_value_t op_pop()
{
    jit_value_t val = jit_insn_load_relative(GLOBAL_F, stack_ptr, -ELEMENT_SIZE, jit_type_int);
    jit_value_t new_ptr = jit_insn_sub(GLOBAL_F, stack_ptr, CONST_INT(ELEMENT_SIZE));
    jit_insn_store(GLOBAL_F, stack_ptr, new_ptr);
    return val;
}

void op_add()
{
    jit_value_t a = op_pop();
    jit_value_t b = op_pop();
    jit_value_t res = jit_insn_add(GLOBAL_F, a, b);
    push_vm(res);
}
void op_sub()
{
    jit_value_t a = op_pop();
    jit_value_t b = op_pop();
    jit_value_t res = jit_insn_sub(GLOBAL_F, a, b);
    push_vm(res);
}
void op_div()
{
    jit_value_t a = op_pop();
    jit_value_t b = op_pop();
    jit_value_t res = jit_insn_div(GLOBAL_F, a, b);
    push_vm(res);
}
void op_mul()
{
    jit_value_t a = op_pop();
    jit_value_t b = op_pop();
    jit_value_t res = jit_insn_mul(GLOBAL_F, a, b);
    push_vm(res);
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
    jit_value_t a = op_pop();
    jit_insn_call_native(GLOBAL_F, "putchar", putchar, dump_signature, &a, 1, JIT_CALL_NOTHROW);
}
void parse(operation op)
{
    switch (op.code)
    {
    case BIN_PUSH:
        op_push(op);
        break;
    case BIN_ADD:
        op_add();
        break;
    case BIN_POP:
        op_pop();
        break;
    case BIN_DUMP:
        op_dump();
        break;
    case BIN_DIV:
        op_div();
        break;
    case BIN_MUL:
        op_mul();
        break;
    case BIN_SUB:
        op_sub();
        break;
    case BIN_SWAP:
        op_swap();
        break;
    case BIN_DUP:
        op_dup();
        break;
    default:
        break;
    }
}
