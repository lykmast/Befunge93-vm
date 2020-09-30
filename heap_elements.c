#include "heap_elements.h"

#if TWO_COMPLEMENT
value_t get_value_from_field(value_t a){
	if(a<0){
		// if negative: shift right by two and set two higher bits
		//   to preserve sign.
		uint64_t res = ((uint64_t)a)>>2;
		return (value_t)(res | (uint64_t)3<<62);
	}
	else{
		uint64_t res = ((uint64_t)a)>>2;
		return (value_t)res;
	}
}

value_t get_field_from_value(value_t a){
	return a<<2;
}

value_t get_a_value(cell_t* addr){
	return get_value_from_field(addr->a);
}

value_t get_b_value(cell_t* addr){
	return get_value_from_field(addr->b);
}

void set_a_field(cell_t* addr, value_t a){
	addr->a = get_field_from_value(a);
}

void set_b_field(cell_t* addr, value_t b){
	addr->b = get_field_from_value(b);
}

cell_t* get_stack_addr(cell_t* addr){
	// heap addresses get right shifted, to be
	//   represented as 62-bit values in stack.
	return (cell_t*)((uint64_t)addr>>2);
}

cell_t* get_heap_addr(cell_t* addr){
	// addresses in stack are right shifted.
	return (cell_t*)((uint64_t)addr<<2);
}

#else

value_t get_value_from_field(value_t a){
	return a;
}
value_t get_field_from_value(value_t a){
	return a;
}

value_t get_a_value(cell_t* addr){
	return addr->a;
}

value_t get_b_value(cell_t* addr){
	return addr->b;
}

void set_a_field(cell_t* addr, value_t a){
	addr->a = a;
}

void set_b_field(cell_t* addr, value_t b){
	addr->b = b;
}

cell_t* get_stack_addr(cell_t* addr){
	return addr;
}

cell_t* get_heap_addr(cell_t* addr){
	return addr;
}
#endif
