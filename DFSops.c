#include "DFSops.h"
#if TWO_COMPLEMENT
// marked reference.
cell_t* get_ptr_from_field(cell_t* curr, int i){
	return (cell_t*)get_unmarked((uint64_t)*get_field(curr,i));
}

void set_field_to_ptr(cell_t* curr, int i, cell_t* ptr){
	// set i-th field of curr to prev
	//   but also preserve flag (last two bits).
	value_t* field = get_field(curr,i);
	int field_flag = get_flag((uint64_t)*field);
	*field = get_marked((value_t)ptr,field_flag);
}

#else
// mark field.
cell_t* get_ptr_from_field(cell_t* curr, int i){
	return (cell_t*)*get_field(curr,i);
}

void set_field_to_ptr(cell_t* curr, int i, cell_t* ptr){
	*get_field(curr,i)=(value_t)ptr;
}
#endif
// common.
value_t* get_field(cell_t* addr, int i){
	if(i==0) return &(addr->a);
	else return &(addr->b);
}
