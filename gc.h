#include "vm.h"
#include "mark.h"
#include "DFSops.h"

void DFS(heap_t* heap, cell_t* curr);

void mark_phase(VM* vm);

void sweep_phase(VM* vm);

cell_t* get_next_from_field(cell_t* curr, int i);
