#define DEFAULT_STACK_SIZE 1000
#define ROWS 25
#define COLS 80
// #define DEBUG 1
typedef enum {NORTH,EAST,SOUTH,WEST} dir_t;
typedef struct {
	void *ops[ROWS][COLS];
	char (*grid)[COLS];
	char stack[DEFAULT_STACK_SIZE];
} VM;

VM *vm_create(char grid[ROWS][COLS]);
void vm_free(VM *vm);
void vm_init(VM *vm, char grid[ROWS][COLS]);
void vm_exec(VM *vm, int startI, int startJ, dir_t start_dir);
void vm_print_instr(char grid[ROWS][COLS], int pcI, int pcJ);
void vm_print_stack(char *stack, int count);
void vm_print_grid(VM *vm);
void vm_print_state(VM *vm, int pcI, int pcJ, dir_t dir, int sp);
