#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#ifndef DEFAULT_STACK_SIZE
	#define DEFAULT_STACK_SIZE (2<<19)// 2^20
#endif

#ifndef DEFAULT_HEAP_SIZE
	#define DEFAULT_HEAP_SIZE (2<<23)//2^24
#endif


typedef int32_t index_t;
typedef int64_t value_t;
#define V_FMT "%" PRId64
#define I_FMT "%" PRId32
#define ROWS 25
#define COLS 80
typedef enum {NORTH,EAST,SOUTH,WEST} dir_t;

typedef struct{
	index_t top;
	value_t elements[DEFAULT_STACK_SIZE];
} stack_t;

typedef struct cell_t{
	value_t a,b;
	uint8_t mark;
} cell_t;

typedef struct{
	cell_t* freelist;
	cell_t elements[DEFAULT_HEAP_SIZE];
} heap_t;


typedef struct {
	void *ops[ROWS][COLS];
	char (*grid)[COLS];
	stack_t* stack;
	heap_t* heap;
} VM;



VM *vm_create(char grid[ROWS][COLS]);
void vm_free(VM *vm);
void vm_init(VM *vm, char grid[ROWS][COLS]);
void vm_exec(VM *vm, int startI, int startJ, dir_t start_dir);
void vm_print_instr(char grid[ROWS][COLS], int pcI, int pcJ);
void vm_print_stack(char *stack);
void vm_print_grid(VM *vm);
void vm_print_state(VM *vm, int pcI, int pcJ, dir_t dir);
void vm_check_for_gc(VM* vm);
void gc(VM* vm);

void stack_push(stack_t *stack, value_t val);
value_t stack_pop(stack_t *stack);
void stack_init(stack_t *stack);
long stack_peek(stack_t *stack);
void stack_print(stack_t *stack);

void heap_init(heap_t *heap);
cell_t* heap_add(heap_t* heap, value_t a, value_t b);
int is_in_heap(heap_t* heap, cell_t* addr);
void heap_check(heap_t* heap, cell_t* addr);
int is_heap_ptr(heap_t* heap, cell_t* ptr);


void mark(cell_t* addr);

void unmark(cell_t* addr);

int is_marked(cell_t* addr);

int get_done(cell_t* addr);

void set_done(cell_t* addr, int i);

value_t* get_field(cell_t* addr, int i);

void DFS(heap_t* heap, cell_t* curr);

void mark_phase(VM* vm);

void sweep_phase(VM* vm);
