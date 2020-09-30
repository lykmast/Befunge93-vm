#include "mark.h"

#if TWO_COMPLEMENT
/*
cell{value_t a,b;};
cell->a is also next if cell is in freelist.
cell->a[63] is marked bit
cell->b[62:64] are done bits
values use the 62 higher bits (a[0:62],b[0:62]) to
represent signed integers.
*/
#include "mark_reference.h"
void mark(cell_t* addr){
	addr->a = (value_t)get_marked_simple((uint64_t)addr->a);
}

void unmark(cell_t* addr){
	addr->a= (value_t)get_unmarked((uint64_t)addr->a);
}

int is_marked(cell_t* addr){
	return (value_t)get_flag((uint64_t)addr->a);
}

int get_done(cell_t* addr){
	return (value_t)get_flag((uint64_t)addr->b);
}

void set_done(cell_t* addr, int i){
	addr->b= (value_t)get_marked((uint64_t)addr->b,i);
}
#else
/*
cell{value_t a,b; int mark;};
cell->a is also next if cell is in freelist.
cell->mark[1] is marked bit
cell->mark[2:4] are done bits
values use the whole 64 bits (a[0:64],b[0:64]) to
represent signed integers.
*/
void mark(cell_t* addr){
	addr->mark = addr->mark | (uint8_t)4;
}

void unmark(cell_t* addr){
	addr->mark=0;
}

int is_marked(cell_t* addr){
	return addr->mark & (uint8_t)4;
}

int get_done(cell_t* addr){
	return addr->mark & (uint8_t)3;
}

void set_done(cell_t* addr, int i){
	uint8_t tmp = (uint8_t) i & (uint8_t)3;
	addr->mark = addr->mark & ~(uint8_t)3;
	addr->mark= addr->mark | tmp;
}
#endif
