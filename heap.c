#include "stdio.h"
#include "heap.h"
#include "heap_elements.h"
void heap_init(heap_t *heap){
	heap->freelist = heap->elements;
	for(index_t i=0; i<DEFAULT_HEAP_SIZE-1; i++){
		heap->elements[i].a = (value_t)&(heap->elements[i+1]);
#if ! TWO_COMPLEMENT
		heap->elements[i].mark = 0;
#endif
	}
#if ! TWO_COMPLEMENT
	heap->elements[DEFAULT_HEAP_SIZE-1].mark = 0;
#endif
	heap->elements[DEFAULT_HEAP_SIZE-1].a = (value_t) NULL;
}

cell_t* heap_add(heap_t *heap, value_t a, value_t b){
	cell_t* addr, *next;
	addr = heap->freelist;
	next = (cell_t*)addr->a;
	set_a_field(heap->freelist,a);
	set_b_field(heap->freelist,b);
	heap->freelist=next;
	return get_stack_addr(addr);
}

int is_in_heap(heap_t* heap, cell_t* addr){
	return addr >= heap->elements &&
	       addr < heap->elements + DEFAULT_HEAP_SIZE;
}

void heap_check(heap_t* heap, cell_t* addr){
	if (!is_in_heap(heap,addr)){
		fprintf(stderr,"Invalid memory access\n");
		exit(1);
	}
}
