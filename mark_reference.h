#include <stdint.h>
/*
  functions for controling flags
    at the last two bits of 64bit type.
*/
uint64_t get_unmarked(uint64_t val);
uint64_t get_marked(uint64_t val,int i);
uint64_t get_marked_simple(uint64_t val);
int get_flag(uint64_t val);
