#include "types.h"
#if TWO_COMPLEMENT
#include "mark_reference.h"
#endif
cell_t* get_ptr_from_field(cell_t* curr, int i);
value_t* get_field(cell_t* addr, int i);
void set_field_to_ptr(cell_t* curr, int i, cell_t* prev);
