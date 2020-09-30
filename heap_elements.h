#include "mark.h"
value_t get_value_from_field(value_t a);

value_t get_field_from_value(value_t a);

value_t get_a_value(cell_t* addr);

value_t get_b_value(cell_t* addr);

void set_a_field(cell_t* addr, value_t a);

void set_b_field(cell_t* addr, value_t b);

cell_t* get_stack_addr(cell_t* addr);

cell_t* get_heap_addr(cell_t* addr);
