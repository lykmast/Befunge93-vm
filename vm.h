#define DEFAULT_STACK_SIZE 1000
#define ROWS 25
#define COLS 80
#define LEFT 1
#define RIGHT -1
#define UP -COLS
#define DOWN +COLS
typedef enum {NORTH,EAST,SOUTH,WEST} dir_t;
typedef struct {
	char grid[ROWS][COLS];
	int pcI;
	int pcJ;
	char stack[DEFAULT_STACK_SIZE];
	int sp;
	dir_t dir;
} VM;

VM *vm_create(char** grid);
VM *vm_free(VM *vm);
VM *vm_init(VM* vm, char **grid);
void vm_exec(VM *vm, int startI, int startJ, dir_t dir, bool trace);
void vm_print_instr(int *code, int pcI, int pcJ);
void vm_print_stack(int *stack, int count);
