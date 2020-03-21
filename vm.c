#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include <time.h>

VM *vm_create(char grid[ROWS][COLS]){
	VM* vm= (VM*) malloc(sizeof(VM));
	vm_init(vm, grid);
	return vm;
}

void vm_free(VM *vm){
	free(vm);
}


void vm_init(VM *vm, char grid[ROWS][COLS]){
	vm->grid=grid;
}

void preprocess(char const* filename,char grid[ROWS][COLS]){
	FILE* f;
	int ii;
	int jj;
	if((f = fopen(filename, "r")) == NULL)
		exit(1);
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
				perror("Expected newline\n");
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

void vm_print_stack(char *stack, int count){
	for(int ii=0; ii<count; ii++)
		fprintf(stderr,"%d",stack[ii]);
}

void vm_print_state(VM *vm, int pcI, int pcJ, dir_t dir, int sp){
	int top;
	if(sp==-1) top=vm->stack[sp]; else top=0;
	if(vm->grid[pcI][pcJ]!=' ')
		fprintf(stderr, "<pc:(%d,%d,%d), sp:%d, st:%d, op:%c>\n",
			pcI, pcJ, dir, sp, top, vm->grid[pcI][pcJ]);
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
		case '0': \
			*op_label=&&NUM0_OP; \
			break; \
		case '1': \
			*op_label=&&NUM1_OP; \
			break; \
		case '2': \
			*op_label=&&NUM2_OP; \
			break; \
		case '3': \
			*op_label=&&NUM3_OP; \
			break; \
		case '4': \
			*op_label=&&NUM4_OP; \
			break; \
		case '5': \
			*op_label=&&NUM5_OP; \
			break; \
		case '6': \
			*op_label=&&NUM6_OP; \
			break; \
		case '7': \
			*op_label=&&NUM7_OP; \
			break; \
		case '8': \
			*op_label=&&NUM8_OP; \
			break; \
		case '9': \
			*op_label=&&NUM9_OP; \
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
#ifdef DEBUG
#define NEXT_INSTRUCTION vm_print_state(vm,pcI,pcJ,dir,sp); \
	goto *(vm->ops[pcI][pcJ]);
#else
#define NEXT_INSTRUCTION goto *(void *)(vm->ops[pcI][pcJ]);
#endif
void vm_exec(VM *vm, int startI, int startJ, dir_t start_dir){
	int pcI=startI; int pcJ=startJ; dir_t dir=start_dir;
	int sp=-1;
	char tmp;
	char buf[20];

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
	vm->stack[sp-1] = vm->stack[sp-1] + vm->stack[sp];
	sp--;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

MINUS_OP:
	vm->stack[sp-1] = vm->stack[sp-1] - vm->stack[sp];
	sp--;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

MUL_OP:
	vm->stack[sp-1] = vm->stack[sp-1] * vm->stack[sp];
	sp--;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

DIV_OP:
	vm->stack[sp-1] = vm->stack[sp-1] / vm->stack[sp];
	sp--;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

MOD_OP:
	vm->stack[sp-1] = vm->stack[sp-1] % vm->stack[sp];
	sp--;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

NOT_OP:
	vm->stack[sp] = !(vm->stack[sp]);
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

BGT_OP:
	vm->stack[sp-1] = vm->stack[sp-1] > vm->stack[sp];
	sp--;
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
	if(vm->stack[sp])
		dir = WEST;
	else
		dir = EAST;
	sp--;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

VIF_OP:
	if(vm->stack[sp])
		dir = NORTH;
	else
		dir = SOUTH;
	sp--;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

STR_OP:
	goto STRING;

DUB_OP:
	vm->stack[sp+1] = vm->stack[sp];
	sp++;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

SWP_OP:
	tmp = vm->stack[sp];
	vm->stack[sp] = vm->stack[sp-1];
	vm->stack[sp-1] = tmp;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

POP_OP:
	sp--;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

PRI_OP:
	printf("%d",vm->stack[sp]);
	sp--;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

PRC_OP:
	putchar(vm->stack[sp]);
	sp--;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

SKP_OP:
	pc_incr(&pcI, &pcJ, dir);
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

GET_OP:
	vm->stack[sp-1] =
		vm->grid[(int)(vm->stack[sp])][(int)(vm->stack[sp-1])];
	sp--;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

PUT_OP:
	vm->grid[(int)(vm->stack[sp])][(int)(vm->stack[sp-1])] =
		vm->stack[sp-2];
	to_op_label(vm->stack[sp-2],
		&(vm->ops[(int)(vm->stack[sp])][(int)(vm->stack[sp-1])]))

	sp-=3;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

RDI_OP:
	fgets(buf, 20, stdin);
	vm->stack[++sp] = atoi(buf);
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

RDC_OP:
	vm->stack[++sp]=getchar();
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

NUM0_OP:
	vm->stack[++sp]=0;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;
NUM1_OP:
	vm->stack[++sp]=1;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;
NUM2_OP:
	vm->stack[++sp]=2;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;
NUM3_OP:
	vm->stack[++sp]=3;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;
NUM4_OP:
	vm->stack[++sp]=4;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;
NUM5_OP:
	vm->stack[++sp]=5;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;
NUM6_OP:
	vm->stack[++sp]=6;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;
NUM7_OP:
	vm->stack[++sp]=7;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;
NUM8_OP:
	vm->stack[++sp]=8;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;
NUM9_OP:
	vm->stack[++sp]=9;
	pc_incr(&pcI,&pcJ,dir);
	NEXT_INSTRUCTION;

NOP_OP:
	fprintf(stderr,"NOP ERROR!\n");
	exit(1);

HLT_OP:
	return;

STRING:
	/*STRING MODE*/
#ifdef DEBUG
	fprintf(stderr,"STRING MODE ON(%d,%d,%d)\n",pcI,pcJ,dir);
#endif
	pc_incr(&pcI, &pcJ, dir);
	while(1){
		char opcode=vm->grid[pcI][pcJ];

		switch(opcode){
			case '"':
				goto ENDSTRING;
			default:
				vm->stack[++sp] = opcode;
		}
		pc_incr(&pcI, &pcJ, dir);
	}

ENDSTRING:
	/*END STRING MODE*/
#ifdef DEBUG
		fprintf(stderr,"STRING MODE OFF(%d,%d,%d)\n",pcI,pcJ,dir);
#endif
	pc_incr(&pcI, &pcJ, dir);
	goto NORMAL;
}

int main(int argc, char const *argv[]) {

	/*--------------PREPROCESSING--------------*/
	if (argc!=2){
		perror("Wrong number of arguments\n");
	}
	srand(time(NULL));
	char grid[ROWS][COLS];
	preprocess(argv[1], grid);
	VM* vm = vm_create(grid);
	vm_exec(vm,0,0,EAST);
	printf("\n");
}
