#include "types.h"
void stack_push(stack_t *stack, value_t val);
value_t stack_pop(stack_t *stack);
void stack_init(stack_t *stack);
long stack_peek(stack_t *stack);
void stack_print(stack_t *stack);
