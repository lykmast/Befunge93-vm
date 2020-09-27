#include "gc.h"
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

uint8_t get_clear_done(cell_t* addr){
	return addr->mark & (uint8_t)4;
}

void set_done(cell_t* addr, int i){
	uint8_t tmp = (uint8_t) i & (uint8_t)3;
	addr->mark= get_clear_done(addr) | tmp;
}
