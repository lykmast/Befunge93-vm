#include "gc.h"
#if DEBUG
	static index_t freed;
#endif
void mark(cell_t* addr);

void unmark(cell_t* addr);

int is_marked(cell_t* addr);

int get_done(cell_t* addr);

void set_done(cell_t* addr, int i);

value_t* get_field(cell_t* addr, int i){
	if(i==0) return &(addr->a);
	else return &(addr->b);
}

void DFS(heap_t* heap, cell_t* curr){
	cell_t* prev=NULL;
	cell_t* next;
	value_t* field;
	mark(curr);
	set_done(curr, 0);
	while (1) {
		int i = get_done(curr);
		if (i < 2){

			field=get_field(curr,i);

			next = (cell_t*)(*field);
			if(is_in_heap(heap, next) && !is_marked(next)){
				*field = (value_t)prev;
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
			field = get_field(curr,i);
			prev = (cell_t*)(*field);
			*field = (value_t)next;
			set_done(curr,i+1);
		}
	}

}

void mark_phase(VM* vm){
	stack_t* stack = vm->stack;
	heap_t* heap = vm->heap;
	for(index_t i=stack->top; i>=0; i--){
		cell_t* ptr = (cell_t*)stack->elements[i];
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
