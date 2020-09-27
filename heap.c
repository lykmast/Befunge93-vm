#include "vm.h"

void heap_init(heap_t *heap){
	heap->freelist = heap->elements;
	for(index_t i=0; i<DEFAULT_HEAP_SIZE-1; i++){
		heap->elements[i].ptr = &(heap->elements[i+1]);
		heap->elements[i].mark = 0;
	}
	heap->elements[DEFAULT_HEAP_SIZE-1].mark = 0;
}

cell_t* heap_add(heap_t *heap, value_t a, value_t b){
	cell_t* addr;
	heap->freelist->a=a;
	heap->freelist->b=b;
	addr = heap->freelist;
	heap->freelist=heap->freelist->ptr;
	return addr;
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

int is_heap_ptr(heap_t* heap, cell_t* ptr){
	return is_in_heap(heap,ptr) && (uintptr_t)ptr %4==0;
}
