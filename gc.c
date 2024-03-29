#include "gc.h"
#if DEBUG
	static index_t freed;
#endif

void DFS(heap_t* heap, cell_t* curr){
	cell_t* prev=NULL;
	cell_t* next;
	mark(curr);
	set_done(curr, 0);
	while (1) {
		int i = get_done(curr);
		if (i < 2){
			next = get_ptr_from_field(curr,i);
			if(is_in_heap(heap, next) && !is_marked(next)){
#if DEBUG
				if(!is_in_heap(heap,prev)){
					fprintf(stderr, "invalid prev address\n");
				}
#endif
				set_field_to_ptr(curr, i, prev);
				prev = curr; curr = next;
				mark(curr);
				set_done(curr,0);
			}
			else set_done(curr, i+1);
		}
		else{
			next = curr; curr = prev;
			if(!curr) return;
			i = get_done(curr);
			prev = get_ptr_from_field(curr,i);
#if DEBUG
			if(!is_in_heap(heap,prev)){
				fprintf(stderr, "invalid prev address\n");
			}
#endif
			set_field_to_ptr(curr,i,next);
			set_done(curr,i+1);
		}
	}

}

void mark_phase(VM* vm){
	stack_t* stack = vm->stack;
	heap_t* heap = vm->heap;
	for(index_t i=stack->top; i>=0; i--){
		cell_t* ptr = get_heap_addr((cell_t*)stack->elements[i]);
		if(is_in_heap(heap, ptr) && !is_marked(ptr)){
			DFS(heap, ptr);
		}
	}
}

void sweep_phase(VM* vm){
#if DEBUG
	freed=0;
#endif
	heap_t* heap = vm->heap;
	heap->freelist=NULL;
	for(index_t i=DEFAULT_HEAP_SIZE-1; i>=0; i--){
		cell_t* curr = &(heap->elements[i]);
		if(is_marked(curr)){
			unmark(curr);
		}
		else{
			curr->a = (value_t)heap->freelist;
			heap->freelist = curr;
#if DEBUG
			freed++;
#endif
		}
	}
}

void gc(VM* vm){
	mark_phase(vm);
	sweep_phase(vm);
	if(!vm->heap->freelist){
		fprintf(stderr,"Heap overflow\n");
		exit(1);
	}
#if DEBUG
	fprintf(stderr, "GC freed " I_FMT " cells!\n", freed);
#endif
}
