#include "include/jit_parser.h"
#include "include/vm.h"
#include <stdlib.h>

jit_context_t context;
jit_function_t GLOBAL_F;
jit_value_t stack_ptr;
jit_type_t dump_signature;
jit_type_t stack_element;
jit_uint stack_element_size;
jit_type_t stack_element_value;
jit_uint stack_element_value_size;

void stack_init(program *__pr)
{
    jit_type_t params[] = {jit_type_int};
    dump_signature = jit_type_create_signature(
        jit_abi_cdecl, jit_type_int, params, 1, 1);

    stack_ptr = jit_value_create(GLOBAL_F, jit_type_void_ptr);
    jit_insn_store(GLOBAL_F, stack_ptr, jit_insn_alloca(GLOBAL_F, CONST_INT(__pr->stack_size * stack_element_size)));
}
void struct_init()
{
    jit_type_t values[] = {jit_type_int, jit_type_sys_char, jit_type_sys_bool, jit_type_void_ptr};
    stack_element_value = jit_type_create_union(values, 4, 1);
    stack_element_value_size = jit_type_get_size(stack_element_value);
    jit_type_t elements[] = {jit_type_ubyte, stack_element_value};
    stack_element = jit_type_create_struct(elements, 2, 1);
    stack_element_size = jit_type_get_size(stack_element);
}
void init(program *__pr)
{

    jit_init();
    context = jit_context_create();
    GLOBAL_F = jit_function_create(context, jit_type_create_signature(jit_abi_cdecl, jit_type_int, NULL, 0, 1));
    struct_init();
    stack_init(__pr);
    labels_init(__pr);
}
void parse_program(program *__pr)
{
    op_node *op_n = __pr->global;
    while ((op_n)->op.code != BIN_EOP)
    {
        parse(op_n->op, __pr);
        op_n = op_n->next;
    }
}
void labels_init(program *__pr)
{
    __pr->labels = malloc(__pr->labels_size);
    for (size_t i = 0; i < __pr->labels_size; i++)
    {
        __pr->labels[i] = jit_label_undefined;
    }
}

void end(program *__pr)
{
    jit_dump_function(stdout, GLOBAL_F, "GLOBAL_F [uncompiled]");
    jit_function_compile(GLOBAL_F);

    jit_dump_function(stdout, GLOBAL_F, "GLOBAL_F [compiled]");
    jit_function_apply(GLOBAL_F, NULL, NULL);
    jit_context_destroy(context);
    free(__pr->labels);
}
void push_vm(jit_value_t val)
{
    jit_insn_store_relative(GLOBAL_F, stack_ptr, 0, val);
    jit_value_t new_ptr = jit_insn_add(GLOBAL_F, stack_ptr, CONST_INT(stack_element_size));
    jit_insn_store(GLOBAL_F, stack_ptr, new_ptr);
}
void push_vm_2(jit_value_t val, jit_value_t type)
{

    jit_insn_store_relative(GLOBAL_F, stack_ptr, 0, type);
    jit_insn_store_relative(GLOBAL_F, stack_ptr, stack_element_value_size, val);
    jit_value_t new_ptr = jit_insn_add(GLOBAL_F, stack_ptr, CONST_INT(stack_element_size));
    jit_insn_store(GLOBAL_F, stack_ptr, new_ptr);
}
void op_push(operation op)
{

    jit_insn_store_relative(GLOBAL_F, stack_ptr, 0, CONST_BYTE(op.payload_type));
    jit_insn_store_relative(GLOBAL_F, stack_ptr, stack_element_value_size, CONST_LONG(op.payload.all));
    jit_value_t new_ptr = jit_insn_add(GLOBAL_F, stack_ptr, CONST_INT(stack_element_size));
    jit_insn_store(GLOBAL_F, stack_ptr, new_ptr);
}

jit_value_t op_pop()
{
    jit_value_t val = jit_insn_load_relative(GLOBAL_F, stack_ptr, -stack_element_size, stack_element);
    jit_value_t new_ptr = jit_insn_sub(GLOBAL_F, stack_ptr, CONST_INT(stack_element_size));
    jit_insn_store(GLOBAL_F, stack_ptr, new_ptr);
    return val;
}
void op_bitwise_or()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, stack_element);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);

    jit_value_t b = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t b_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b), stack_element_size, stack_element);
    jit_value_t b_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b_union), 0, jit_type_int);
    jit_value_t res = jit_insn_or(GLOBAL_F, b_val, a_val);
    push_vm_2(res, CONST_BYTE(0));
}
void op_bitwise_and()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, stack_element);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);

    jit_value_t b = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t b_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b), stack_element_size, stack_element);
    jit_value_t b_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b_union), 0, jit_type_int);
    jit_value_t res = jit_insn_and(GLOBAL_F, b_val, a_val);
    push_vm_2(res, CONST_BYTE(0));
}
void op_bitwise_xor()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, stack_element);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);

    jit_value_t b = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t b_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b), stack_element_size, stack_element);
    jit_value_t b_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b_union), 0, jit_type_int);
    jit_value_t res = jit_insn_xor(GLOBAL_F, b_val, a_val);
    push_vm_2(res, CONST_BYTE(0));
}
void op_bitwise_not()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, jit_type_long);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);
    jit_value_t res = jit_insn_not(GLOBAL_F, a_val);
    push_vm_2(res, CONST_BYTE(0));
}
void op_left_shift()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, stack_element);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);

    jit_value_t b = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t b_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b), stack_element_size, stack_element);
    jit_value_t b_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b_union), 0, jit_type_int);
    jit_value_t res = jit_insn_shl(GLOBAL_F, b_val, a_val);
    push_vm_2(res, CONST_BYTE(0));
}
void op_right_shift()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, stack_element);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);

    jit_value_t b = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t b_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b), stack_element_size, stack_element);
    jit_value_t b_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b_union), 0, jit_type_int);
    jit_value_t res = jit_insn_shr(GLOBAL_F, b_val, a_val);
    push_vm_2(res, CONST_BYTE(0));
}
void op_equal()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, stack_element);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);

    jit_value_t b = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t b_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b), stack_element_size, stack_element);
    jit_value_t b_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b_union), 0, jit_type_int);
    jit_value_t res = jit_insn_eq(GLOBAL_F, b_val, a_val);
    push_vm_2(res, CONST_BYTE(0));
}
void op_not()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, stack_element);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);

    jit_value_t res = jit_insn_neg(GLOBAL_F, a_val);
    push_vm_2(res, CONST_BYTE(0));
}
void op_or()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, stack_element);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);

    jit_value_t b = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t b_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b), stack_element_size, stack_element);
    jit_value_t b_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b_union), 0, jit_type_int);
    jit_value_t res = jit_insn_or(GLOBAL_F, b_val, a_val);
    push_vm_2(res, CONST_BYTE(0));
}
void op_and()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, stack_element);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);

    jit_value_t b = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t b_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b), stack_element_size, stack_element);
    jit_value_t b_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b_union), 0, jit_type_int);
    jit_value_t res = jit_insn_and(GLOBAL_F, b_val, a_val);
    push_vm_2(res, CONST_BYTE(0));
}
void op_not_equal()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, stack_element);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);

    jit_value_t b = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t b_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b), stack_element_size, stack_element);
    jit_value_t b_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b_union), 0, jit_type_int);
    jit_value_t res = jit_insn_ne(GLOBAL_F, b_val, a_val);
    push_vm_2(res, CONST_BYTE(0));
}
void op_grater()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, stack_element);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);

    jit_value_t b = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t b_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b), stack_element_size, stack_element);
    jit_value_t b_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b_union), 0, jit_type_int);
    jit_value_t res = jit_insn_gt(GLOBAL_F, b_val, a_val);
    push_vm_2(res, CONST_BYTE(0));
}
void op_lower()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, stack_element);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);

    jit_value_t b = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t b_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b), stack_element_size, stack_element);
    jit_value_t b_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b_union), 0, jit_type_int);
    jit_value_t res = jit_insn_lt(GLOBAL_F, b_val, a_val);
    push_vm_2(res, CONST_BYTE(0));
}
void op_grater_or_equal()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, stack_element);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);

    jit_value_t b = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t b_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b), stack_element_size, stack_element);
    jit_value_t b_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b_union), 0, jit_type_int);
    jit_value_t res = jit_insn_ge(GLOBAL_F, b_val, a_val);
    push_vm_2(res, CONST_BYTE(0));
}
void op_lower_or_equal()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, stack_element);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);

    jit_value_t b = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t b_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b), stack_element_size, stack_element);
    jit_value_t b_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b_union), 0, jit_type_int);
    jit_value_t res = jit_insn_le(GLOBAL_F, b_val, a_val);
    push_vm_2(res, CONST_BYTE(0));
}
void op_add()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, stack_element);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);

    jit_value_t b = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t b_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b), stack_element_size, stack_element);
    jit_value_t b_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b_union), 0, jit_type_int);

    jit_value_t res_val = jit_insn_add(GLOBAL_F, b_val, a_val);

    push_vm_2(res_val, CONST_BYTE(0));
}
void op_sub()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, stack_element);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);

    jit_value_t b = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t b_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b), stack_element_size, stack_element);
    jit_value_t b_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b_union), 0, jit_type_int);

    jit_value_t res = jit_insn_sub(GLOBAL_F, b_val, a_val);
    push_vm_2(res, CONST_BYTE(0));
}
void op_div()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, stack_element);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);

    jit_value_t b = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t b_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b), stack_element_size, stack_element);
    jit_value_t b_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b_union), 0, jit_type_int);
    jit_value_t res = jit_insn_div(GLOBAL_F, b_val, a_val);
    push_vm_2(res, CONST_BYTE(0));
}
void op_mul()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, stack_element);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_int);

    jit_value_t b = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t b_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b), stack_element_size, stack_element);
    jit_value_t b_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, b_union), 0, jit_type_int);
    jit_value_t res = jit_insn_mul(GLOBAL_F, b_val, a_val);
    push_vm_2(res, CONST_BYTE(0));
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
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_union = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), stack_element_size, jit_type_long);
    jit_value_t a_val = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a_union), 0, jit_type_ubyte);
    /*jit_insn_call_native(GLOBAL_F, "putchar", putchar, dump_signature, &a_val, 1, JIT_CALL_NOTHROW);*/
    jit_type_t type_cstring = jit_type_create_pointer(jit_type_sys_char, 2);
    jit_value_t hostmemptr = jit_value_create_long_constant(
        GLOBAL_F, type_cstring, (long)"%hhu\n");
    jit_type_t pa[] = {type_cstring, jit_type_ubyte};
    jit_type_t puts_signature = jit_type_create_signature(
        jit_abi_cdecl, jit_type_ubyte, pa, 2, 1);
    jit_value_t args[] = {hostmemptr, a_val};
    jit_insn_call_native(
        GLOBAL_F, "printf", printf, puts_signature, args, 2, JIT_CALL_NOTHROW);
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
void op_typeof()
{
    jit_value_t a = jit_insn_address_of(GLOBAL_F, op_pop());
    jit_value_t a_type = jit_insn_load_relative(GLOBAL_F, jit_insn_address_of(GLOBAL_F, a), 0, jit_type_ubyte);
    push_vm_2(a_type, CONST_BYTE(TYPE));
}
void parse(operation op, program *__pr)
{
    switch (op.code)
    {
    case BIN_TYPEOF:
        op_typeof();
        break;
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
        op_printf();
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
    case BIN_EQUAL:
        op_equal();
        break;
    case BIN_NOT:
        op_not();
        break;
    case BIN_NOT_EQUAL:
        op_not_equal();
        break;
    case BIN_OR:
        op_or();
        break;
    case BIN_AND:
        op_and();
        break;
    case BIN_GRATER:
        op_grater();
        break;
    case BIN_LOWER:
        op_lower();
        break;
    case BIN_GRATER_OR_EQUAL:
        op_grater_or_equal();
        break;
    case BIN_LOWER_OR_EQUAL:
        op_lower_or_equal();
        break;
    case BIN_BITWISE_AND:
        op_bitwise_and();
        break;
    case BIN_BITWISE_OR:
        op_bitwise_or();
        break;
    case BIN_BITWISE_XOR:
        op_bitwise_xor();
        break;
    case BIN_BITWISE_NOT:
        op_bitwise_not();
        break;
    case BIN_LEFT_SHIFT:
        op_left_shift();
        break;
    case BIN_RIGHT_SHIFT:
        op_right_shift();
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
        break;
    }
}
