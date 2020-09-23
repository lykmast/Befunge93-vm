#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include <time.h>

void stack_push(stack_t *stack, value_t val){
	if(stack->top>=(DEFAULT_STACK_SIZE-1)){
		fprintf(stderr,"Stack overflow\n");
		exit(1);
	}
	stack->elements[++(stack->top)]=val;
}

cell_t* heap_add(heap_t *heap, value_t a, value_t b){
	cell_t* addr;
	heap->elements[heap->top].a=a;
	heap->elements[heap->top].b=b;
	addr = &(heap->elements[heap->top]);
	++(heap->top);
	if(heap->top>=(DEFAULT_HEAP_SIZE-1)){
		fprintf(stderr,"Heap overflow\n");
		exit(1);
		//gc(&addr);
	}
	return addr;
}

void heap_check(heap_t* heap, cell_t* addr){
	if (addr < heap->elements ||
	    addr >= heap->elements + DEFAULT_HEAP_SIZE ){
		fprintf(stderr,"Invalid memory access\n");
		exit(1);
	}
}

VM *vm_create(char grid[ROWS][COLS]){
	VM* vm= (VM*) malloc(sizeof(VM));
	vm->stack=
		(stack_t*)malloc(sizeof(stack_t));
	vm->heap=
		(heap_t*)malloc(sizeof(heap_t));
	vm_init(vm, grid);
	return vm;
}

void vm_free(VM *vm){
	free(vm->stack);
	free(vm);
}


void vm_init(VM *vm, char grid[ROWS][COLS]){
	vm->grid=grid;
	stack_init(vm->stack);
}

void preprocess(char const* filename,char grid[ROWS][COLS]){
	FILE* f;
	int ii;
	int jj;
	if((f = fopen(filename, "r")) == NULL){
		perror("Error while opening file");
		exit(1);
	}
	for(ii=0;ii<ROWS;ii++)
		for(jj=0;jj<COLS;jj++)
			grid[ii][jj]=' ';

	char c;
	ii=0;
	jj=0;
	while(fscanf(f,"%c",&c)!=EOF){
		if(c=='\n'){
			/*on newline go to start of next line*/
			ii++;
			jj=0;
			continue;
		}
		else{
			if(jj==COLS){
				fprintf(stderr,"Parsing error at <%d,%d>: Expected newline\n",ii,jj);
				exit(1);
			}
			grid[ii][jj]=c;
		}
		jj++;
	}
}
void vm_print_grid(VM *vm){
	for(int ii=0;ii<ROWS;ii++)
		for(int jj=0;jj<COLS;jj++){
			fprintf(stderr,"%c",vm->grid[ii][jj]);
			if(jj==COLS-1)
				fprintf(stderr,"\n");
		}
}
#define vm_print_ops(vm) \
	for(int ii=0;ii<ROWS;ii++) \
		for(int jj=0;jj<COLS;jj++){ \
			fprintf(stderr,"%p ",vm->ops[ii][jj]); \
			if(jj==COLS-1) \
				fprintf(stderr,"\n"); \
		}
void vm_print_instr(char grid[ROWS][COLS], int pcI, int pcJ){
	if(grid[pcI][pcJ]!=' ')
		fprintf(stderr,"%c",grid[pcI][pcJ]);
}

void stack_print(stack_t *stack, int count){
	for(index_t ii=stack->top; ii>=0; ii++)
		fprintf(stderr,V_FMT,stack->elements[ii]);
}

void vm_print_state(VM *vm, int pcI, int pcJ, dir_t dir){
	value_t top=stack_peek(vm->stack);
	if(vm->grid[pcI][pcJ]!=' ')
		fprintf(stderr, "<pc:(%d,%d,%d), sp:" S_FMT ", st:" V_FMT ", op:%c>\n",
			pcI, pcJ, dir, vm->stack->top, top, vm->grid[pcI][pcJ]);
}


#define to_op_label(opcode, op_label) \
	switch (opcode) { \
		case ' ': \
			*op_label=&&SPACE_OP; \
			break; \
		case '+': \
			*op_label=&&PLUS_OP; \
			break; \
		case '-': \
			*op_label=&&MINUS_OP; \
			break; \
		case '*': \
			*op_label=&&MUL_OP; \
			break; \
		case '/': \
			*op_label=&&DIV_OP; \
			break; \
		case '%': \
			*op_label=&&MOD_OP; \
			break; \
		case '!': \
			*op_label=&&NOT_OP; \
			break; \
		case '`': \
			*op_label=&&BGT_OP; \
			break; \
		case '>': \
			*op_label=&&EAST_OP; \
			break; \
		case '<': \
			*op_label=&&WEST_OP; \
			break; \
		case '^': \
			*op_label=&&NORTH_OP; \
			break; \
		case 'v': \
			*op_label=&&SOUTH_OP; \
			break; \
		case '?': \
			*op_label=&&ANY_OP; \
			break; \
		case '_': \
			*op_label=&&HIF_OP; \
			break; \
		case '|': \
			*op_label=&&VIF_OP; \
			break; \
		case '"': \
			*op_label=&&STR_OP; \
			break; \
		case ':': \
			*op_label=&&DUB_OP; \
			break; \
		case '\\': \
			*op_label=&&SWP_OP; \
			break; \
		case '$': \
			*op_label=&&POP_OP; \
			break; \
		case '.': \
			*op_label=&&PRI_OP; \
			break; \
		case ',': \
			*op_label=&&PRC_OP; \
			break; \
		case '#': \
			*op_label=&&SKP_OP; \
			break; \
		case 'g': \
			*op_label=&&GET_OP; \
			break; \
		case 'p': \
			*op_label=&&PUT_OP; \
			break; \
		case '&': \
			*op_label=&&RDI_OP; \
			break; \
		case '~': \
			*op_label=&&RDC_OP; \
			break; \
		case '@': \
			*op_label=&&HLT_OP; \
			break; \
		case '0' ... '9': \
			*op_label=&&NUM_OP; \
			break; \
		case 'c':\
			*op_label=&&CONS_OP; \
			break; \
		case 'h':\
			*op_label=&&HEAD_OP; \
			break; \
		case 't':\
			*op_label=&&TAIL_OP; \
			break; \
		default: \
			*op_label=&&NOP_OP; \
	}

#define pc_incr(pcI, pcJ, dir) \
	switch (dir) { \
		case EAST: \
			*pcJ=(*pcJ+1)%COLS; \
			break; \
		case NORTH: \
			*pcI=(*pcI-1+ROWS)%ROWS; \
			break; \
		case WEST: \
			*pcJ=(*pcJ-1+COLS)%COLS; \
			break; \
		case SOUTH: \
			*pcI=(*pcI+1)%ROWS; \
			break; \
	}
#if DEBUG
#define NEXT_INSTRUCTION vm_print_state(vm,pcI,pcJ,dir); \
	goto *(vm->ops[pcI][pcJ]);
#else
#define NEXT_INSTRUCTION goto *(void *)(vm->ops[pcI][pcJ]);
#endif
void vm_exec(VM *vm, int startI, int startJ, dir_t start_dir){
	int pcI=startI; int pcJ=startJ; dir_t dir=start_dir;
	value_t val1,val2,val3;
	cell_t* addr;

	/*==========Create vm->ops grid=================*/
	for(int ii=0;ii<ROWS;ii++)
		for(int jj=0;jj<COLS;jj++)
			to_op_label(vm->grid[ii][jj],&(vm->ops[ii][jj]));

NORMAL:
	/*NORMAL MODE*/
	NEXT_INSTRUCTION;

SPACE_OP:
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

PLUS_OP:
	val1=stack_pop(vm->stack); val2=stack_pop(vm->stack);
	stack_push(vm->stack,val2+val1);
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

MINUS_OP:
	val1=stack_pop(vm->stack); val2=stack_pop(vm->stack);
	stack_push(vm->stack,val2-val1);
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

MUL_OP:
	val1=stack_pop(vm->stack); val2=stack_pop(vm->stack);
	stack_push(vm->stack,val2*val1);
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

DIV_OP:
	val1=stack_pop(vm->stack); val2=stack_pop(vm->stack);
	stack_push(vm->stack,val2/val1);
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

MOD_OP:
	val1=stack_pop(vm->stack); val2=stack_pop(vm->stack);
	stack_push(vm->stack,val2%val1);
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

NOT_OP:
	val1=stack_pop(vm->stack);
	stack_push(vm->stack,(value_t)!(val1));
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

BGT_OP:
	val1=stack_pop(vm->stack); val2=stack_pop(vm->stack);
	stack_push(vm->stack,(value_t) (val2>val1));
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

EAST_OP:
	dir = EAST;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

WEST_OP:
	dir = WEST;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

NORTH_OP:
	dir = NORTH;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

SOUTH_OP:
	dir = SOUTH;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

ANY_OP:
	dir = (dir_t)(rand()%4);
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

HIF_OP:
	if(stack_pop(vm->stack))
		dir = WEST;
	else
		dir = EAST;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

VIF_OP:
	if(stack_pop(vm->stack))
		dir = NORTH;
	else
		dir = SOUTH;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

STR_OP:
	goto STRING;

DUB_OP:
	stack_push(vm->stack,stack_peek(vm->stack));
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

SWP_OP:
	val1 = stack_pop(vm->stack);
	val2 = stack_pop(vm->stack);
	stack_push(vm->stack,val1);
	stack_push(vm->stack,val2);
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

POP_OP:
	stack_pop(vm->stack);
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

PRI_OP:
	printf(V_FMT " ",stack_pop(vm->stack));
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

PRC_OP:
	printf("%c",(char)(stack_pop(vm->stack)));
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

SKP_OP:
	pc_incr(&pcI, &pcJ, dir);
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

GET_OP:
	val1=stack_pop(vm->stack);
	val2=stack_pop(vm->stack);
	if(val1<ROWS && val2<COLS)
		stack_push(vm->stack,(value_t) vm->grid[val1][val2]);
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

PUT_OP:
	val1=stack_pop(vm->stack);
	val2=stack_pop(vm->stack);
	val3=stack_pop(vm->stack);
	if(val1<ROWS && val2<COLS){
		to_op_label(val3,
			&(vm->ops[val1][val2]));
		vm->grid[val1][val2]=val3;
	}
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

RDI_OP:
	scanf(V_FMT,&val1);
	stack_push(vm->stack,val1);
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

RDC_OP:
	stack_push(vm->stack, (value_t)getchar());
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

NUM_OP:
	stack_push(vm->stack, (value_t)(vm->grid[pcI][pcJ]-'0'));
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

CONS_OP:
	val1=stack_pop(vm->stack);
	val2=stack_pop(vm->stack);
	addr=heap_add(vm->heap,val2,val1);
	stack_push(vm->stack, (value_t)addr);
	NEXT_INSTRUCTION;

HEAD_OP:
	addr=(cell_t*)stack_pop(vm->stack);
	heap_check(vm->heap,addr);
	stack_push(vm->stack, addr->a);
	NEXT_INSTRUCTION;

TAIL_OP:
	addr=(cell_t*)stack_pop(vm->stack);
	heap_check(vm->heap,addr);
	stack_push(vm->stack, addr->b);
	NEXT_INSTRUCTION;

NOP_OP:
	fprintf(stderr,"Invalid command '%c' at <%d,%d>\n",vm->grid[pcI][pcJ],pcI,pcJ);
	exit(1);

HLT_OP:
	return;

STRING:
	/*STRING MODE*/
#if DEBUG
	fprintf(stderr,"STRING MODE ON(%d,%d,%d)\n",pcI,pcJ,dir);
#endif
	pc_incr(&pcI, &pcJ, dir);
	while(1){
		char opcode=vm->grid[pcI][pcJ];

		switch(opcode){
			case '"':
				goto ENDSTRING;
			default:
				stack_push(vm->stack,(value_t) opcode);
		}
		pc_incr(&pcI, &pcJ, dir);
	}

ENDSTRING:
	/*END STRING MODE*/
#if DEBUG
	fprintf(stderr,"STRING MODE OFF(%d,%d,%d)\n",pcI,pcJ,dir);
#endif
	pc_incr(&pcI, &pcJ, dir);
	goto NORMAL;
}

int main(int argc, char const *argv[]) {

	/*--------------PREPROCESSING--------------*/
	if (argc!=2){
		fprintf(stderr,"Wrong number of arguments\n");
		exit(1);
	}
	srand(time(NULL));
	char grid[ROWS][COLS];
	preprocess(argv[1], grid);
	VM* vm = vm_create(grid);
	vm_exec(vm,0,0,EAST);
}
