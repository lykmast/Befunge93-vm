#include "types.h"
#include "heap_elements.h"
void heap_init(heap_t *heap);
cell_t* heap_add(heap_t* heap, value_t a, value_t b);
int is_in_heap(heap_t* heap, cell_t* addr);
void heap_check(heap_t* heap, cell_t* addr);
