#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "types.h"
#include "stack.h"
#include "heap.h"

#ifndef ROWS
#define ROWS 25
#define COLS 80
#endif
typedef enum {NORTH,EAST,SOUTH,WEST} dir_t;

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
