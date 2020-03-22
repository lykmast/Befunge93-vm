#ifndef DEFAULT_STACK_SIZE
	#define DEFAULT_STACK_SIZE ((2<<20)/8) // 2MB
#endif

#define ROWS 25
#define COLS 80
typedef enum {NORTH,EAST,SOUTH,WEST} dir_t;
typedef struct{
	long top;
	long elements[DEFAULT_STACK_SIZE];
} stack_t;
typedef struct {

	void *ops[ROWS][COLS];
	char (*grid)[COLS];
	stack_t* stack;
} VM;



VM *vm_create(char grid[ROWS][COLS]);
void vm_free(VM *vm);
void vm_init(VM *vm, char grid[ROWS][COLS]);
void vm_exec(VM *vm, int startI, int startJ, dir_t start_dir);
void vm_print_instr(char grid[ROWS][COLS], int pcI, int pcJ);
void vm_print_stack(char *stack, int count);
void vm_print_grid(VM *vm);
void vm_print_state(VM *vm, int pcI, int pcJ, dir_t dir);

void stack_push(stack_t *stack, long val);
long stack_pop(stack_t *stack){
	if(stack->top==-1)
		return 0;
	return stack->elements[stack->top--];
}
void stack_init(stack_t *stack){
	stack->top=-1;
}

long stack_peek(stack_t *stack){
	if(stack->top==-1)
		return 0;
	return stack->elements[stack->top];
}
