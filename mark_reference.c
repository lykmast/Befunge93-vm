#include "mark_reference.h"
/*
  functions for controling flags
    at the last two bits of 64bit type.
*/
uint64_t get_unmarked(uint64_t val){
	return (uint64_t)((uint64_t)val & ~(uint64_t)3);
}

uint64_t get_marked(uint64_t val,int i){
	uint64_t tmp = (uint64_t) i & (uint64_t)3;
	return get_unmarked(val) | tmp;
}

uint64_t get_marked_simple(uint64_t val){
	return val | (uint64_t)1;
}

int get_flag(uint64_t val){
	return (uint64_t) val & (uint64_t) 3;
}
