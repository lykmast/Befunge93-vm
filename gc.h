#include "vm.h"
void mark(cell_t* addr);

void unmark(cell_t* addr);

int is_marked(cell_t* addr);

int get_done(cell_t* addr);

void set_done(cell_t* addr, int i);

value_t* get_field(cell_t* addr, int i);

void DFS(heap_t* heap, cell_t* curr);

void mark_phase(VM* vm);

void sweep_phase(VM* vm);
