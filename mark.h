#include "types.h"
void mark(cell_t* addr);

void unmark(cell_t* addr);

int is_marked(cell_t* addr);

int get_done(cell_t* addr);

void set_done(cell_t* addr, int i);

value_t* get_field(cell_t* addr, int i);
