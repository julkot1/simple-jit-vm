#include "include/jit_native.h"
#include "include/main.h"
#include "string.h"

jit_type_t stack_element;
jit_uint stack_element_size;
jit_type_t stack_element_value;
jit_uint stack_element_value_size;
native_function native_functions[1024];
char *native_names[1024];
native_function create_two_args_function(function_ptr fptr, opcode code)
{
    jit_type_t params[] = {stack_element, stack_element};
    native_function native_f = {.function = fptr, .args = 2, .name = code};
    native_f.signature = jit_type_create_signature(jit_abi_cdecl, stack_element, params, 2, 1);
    return native_f;
}
native_function create_arg_function(function_ptr fptr, opcode code)
{
    native_function native_f = {.function = fptr, .args = 1, .name = code};
    native_f.signature = jit_type_create_signature(jit_abi_cdecl, stack_element, &stack_element, 1, 1);
    return native_f;
}
void types_init()
{
    jit_type_t values[] = {jit_type_int, jit_type_sys_char, jit_type_sys_bool, jit_type_void_ptr};
    stack_element_value = jit_type_create_union(values, 4, 1);
    stack_element_value_size = jit_type_get_size(stack_element_value);
    jit_type_t elements[] = {jit_type_ubyte, stack_element_value};
    stack_element = jit_type_create_struct(elements, 2, 1);
    stack_element_size = jit_type_get_size(stack_element);

    native_names[BIN_ADD] = "native_add";
    native_names[BIN_SUB] = "native_sub";
    native_names[BIN_DIV] = "native_div";
    native_names[BIN_MUL] = "native_mul";
    native_names[BIN_NOT_EQUAL] = "native_not_equal";
    native_names[BIN_OR] = "native_or";
    native_names[BIN_AND] = "native_and";
    native_names[BIN_GRATER] = "native_grater";
    native_names[BIN_LOWER] = "native_lower";
    native_names[BIN_GRATER_OR_EQUAL] = "native_grater_or_equal";
    native_names[BIN_LOWER_OR_EQUAL] = "native_lower";
    native_names[BIN_BITWISE_AND] = "native_bitwise_and";
    native_names[BIN_BITWISE_OR] = "native_bitwise_or";
    native_names[BIN_BITWISE_XOR] = "native_bitwise_xor";
    native_names[BIN_BITWISE_NOT] = "native_bitwise_not";
    native_names[BIN_LEFT_SHIFT] = "native_left_shift";
    native_names[BIN_RIGHT_SHIFT] = "native_right_shift";

    native_functions[BIN_ADD] = create_two_args_function(native_add, BIN_ADD);
    native_functions[BIN_SUB] = create_two_args_function(native_sub, BIN_SUB);
    native_functions[BIN_DIV] = create_two_args_function(native_div, BIN_DIV);
    native_functions[BIN_MUL] = create_two_args_function(native_mul, BIN_MUL);
    native_functions[BIN_EQUAL] = create_two_args_function(native_equal, BIN_EQUAL);
    native_functions[BIN_NOT_EQUAL] = create_two_args_function(native_not_equal, BIN_NOT_EQUAL);
    native_functions[BIN_OR] = create_two_args_function(native_or, BIN_OR);
    native_functions[BIN_AND] = create_two_args_function(native_and, BIN_AND);
    native_functions[BIN_GRATER] = create_two_args_function(native_grater, BIN_GRATER);
    native_functions[BIN_LOWER] = create_two_args_function(native_lower, BIN_LOWER);
    native_functions[BIN_GRATER_OR_EQUAL] = create_two_args_function(native_grater_or_equal, BIN_GRATER_OR_EQUAL);
    native_functions[BIN_LOWER_OR_EQUAL] = create_two_args_function(native_lower, BIN_LOWER_OR_EQUAL);
    native_functions[BIN_BITWISE_AND] = create_two_args_function(native_bitwise_and, BIN_BITWISE_AND);
    native_functions[BIN_BITWISE_OR] = create_two_args_function(native_bitwise_or, BIN_BITWISE_OR);
    native_functions[BIN_BITWISE_XOR] = create_two_args_function(native_bitwise_xor, BIN_BITWISE_XOR);
    native_functions[BIN_LEFT_SHIFT] = create_two_args_function(native_left_shift, BIN_LEFT_SHIFT);
    native_functions[BIN_RIGHT_SHIFT] = create_two_args_function(native_right_shift, BIN_RIGHT_SHIFT);

    // native_functions[BIN_NOT] = create_arg_function(native_not, BIN_NOT);
    // native_functions[BIN_BITWISE_NOT] = create_arg_function(native_bitwise_not, BIN_BITWISE_NOT);
}

inline struct stack_element native_add(struct stack_element a, struct stack_element b)
{
    return (struct stack_element){NUMBER, .val.number = (a.val.number + b.val.number)};
}
inline struct stack_element native_sub(struct stack_element a, struct stack_element b)
{
    return (struct stack_element){NUMBER, a.val.number - b.val.number};
}
inline struct stack_element native_div(struct stack_element a, struct stack_element b)
{
    return (struct stack_element){NUMBER, a.val.number / b.val.number};
}
inline struct stack_element native_mul(struct stack_element a, struct stack_element b)
{
    return (struct stack_element){NUMBER, a.val.number * b.val.number};
}
inline struct stack_element native_mod(struct stack_element a, struct stack_element b)
{
    return (struct stack_element){NUMBER, a.val.number % b.val.number};
}
inline struct stack_element native_equal(struct stack_element a, struct stack_element b)
{
    return (struct stack_element){NUMBER, a.val.number == b.val.number};
}
inline struct stack_element native_or(struct stack_element a, struct stack_element b)
{
    return (struct stack_element){NUMBER, a.val.number || b.val.number};
}
inline struct stack_element native_and(struct stack_element a, struct stack_element b)
{
    return (struct stack_element){NUMBER, a.val.number && b.val.number};
}
inline struct stack_element native_not_equal(struct stack_element a, struct stack_element b)
{
    return (struct stack_element){NUMBER, a.val.number != b.val.number};
}
inline struct stack_element native_grater(struct stack_element a, struct stack_element b)
{
    return (struct stack_element){NUMBER, a.val.number > b.val.number};
}
inline struct stack_element native_lower(struct stack_element a, struct stack_element b)
{
    return (struct stack_element){NUMBER, a.val.number < b.val.number};
}
inline struct stack_element native_grater_or_equal(struct stack_element a, struct stack_element b)
{
    return (struct stack_element){NUMBER, a.val.number >= b.val.number};
}
inline struct stack_element native_lower_or_equal(struct stack_element a, struct stack_element b)
{
    return (struct stack_element){NUMBER, a.val.number <= b.val.number};
}
inline struct stack_element native_bitwise_or(struct stack_element a, struct stack_element b)
{
    return (struct stack_element){NUMBER, a.val.number | b.val.number};
}
inline struct stack_element native_bitwise_and(struct stack_element a, struct stack_element b)
{
    return (struct stack_element){NUMBER, a.val.number & b.val.number};
}
inline struct stack_element native_bitwise_xor(struct stack_element a, struct stack_element b)
{
    return (struct stack_element){NUMBER, a.val.number ^ b.val.number};
}
inline struct stack_element native_left_shift(struct stack_element a, struct stack_element b)
{
    return (struct stack_element){NUMBER, a.val.number << b.val.number};
}
inline struct stack_element native_right_shift(struct stack_element a, struct stack_element b)
{
    return (struct stack_element){NUMBER, a.val.number >> b.val.number};
}

inline struct stack_element native_not(struct stack_element a)
{
    return (struct stack_element){NUMBER, !a.val.number};
}
inline struct stack_element native_bitwise_not(struct stack_element a)
{
    return (struct stack_element){NUMBER, ~a.val.number};
}