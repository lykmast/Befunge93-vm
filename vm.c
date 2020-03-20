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

void vm_print_instr(char grid[ROWS][COLS], int pcI, int pcJ){
	if(grid[pcI][pcJ]!=' ')
		fprintf(stderr,"%c",grid[pcI][pcJ]);
}

void vm_print_stack(char *stack, int count){
	for(int ii=0; ii<count; ii++)
		fprintf(stderr,"%d",stack[ii]);
}

void vm_print_state(VM *vm, int pcI, int pcJ, dir_t dir, int sp){
	if(vm->grid[pcI][pcJ]!=' ')
		fprintf(stderr, "<pc:(%d,%d,%d), sp:%d, st:%d, op:%c>\n",
			pcI, pcJ, dir, sp, vm->stack[sp], vm->grid[pcI][pcJ]);
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
void vm_exec(VM *vm, int startI, int startJ, dir_t start_dir, int trace){
	int pcI=startI; int pcJ=startJ; dir_t dir=start_dir;
	int sp=-1;
	char tmp;
	char buf[20];

	static void *label_tab[]={
		&&SPACE_OP,
		&&PLUS_OP,
		&&MINUS_OP,
		&&MUL_OP,
		&&DIN_OP,
		&&MOD_OP,
		&&NOT_OP,
		&&BGT_OP,
		&&EAST_OP,
		&&WEST_OP,
		&&NORTH_OP,
		&&SOUTH_OP,
		&&ANY_OP,
		&&HIF_OP,
		&&VIF_OP,
		&&STR_OP,
		&&DUB_OP,
		&&SWP_OP,
		&&POP_OP,
		&&PRI_OP,
		&&PRC_OP,
		&&SKP_OP,
		&&GET_OP,
		&&PUT_OP,
		&&RDI_OP,
		&&RDC_OP,
		&&HLT_OP
	};
#define NEXT_INSTRUCTION goto *(void *)label_tab[vm->grid[pcI][pcJ]]
NORMAL:
	/*NORMAL MODE*/
	while(1){
		char opcode=vm->grid[pcI][pcJ];

		if(trace){
			vm_print_state(vm, pcI, pcJ, dir, sp);
		}


		switch (opcode) {
			case ' ':
SPACE_OP:
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;
			case '+':
PLUS_OP:
				vm->stack[sp-1] = vm->stack[sp-1] + vm->stack[sp];
				sp--;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '-':
MINUS_OP:
				vm->stack[sp-1] = vm->stack[sp-1] - vm->stack[sp];
				sp--;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '*':
MUL_OP:
				vm->stack[sp-1] = vm->stack[sp-1] * vm->stack[sp];
				sp--;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '/':
DIN_OP:
				vm->stack[sp-1] = vm->stack[sp-1] / vm->stack[sp];
				sp--;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '%':
MOD_OP:
				vm->stack[sp-1] = vm->stack[sp-1] % vm->stack[sp];
				sp--;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '!':
NOT_OP:
				vm->stack[sp] = !(vm->stack[sp]);
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '`':
BGT_OP:
				vm->stack[sp-1] = vm->stack[sp-1] > vm->stack[sp];
				sp--;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '>':
EAST_OP:
				dir = EAST;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '<':
WEST_OP:
				dir = WEST;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '^':
NORTH_OP:
				dir = NORTH;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case 'v':
SOUTH_OP:
				dir = SOUTH;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '?':
ANY_OP:
				dir = (dir_t)(rand()%4);
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '_':
HIF_OP:
				if(vm->stack[sp])
					dir = WEST;
				else
					dir = EAST;
				sp--;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '|':
VIF_OP:
				if(vm->stack[sp])
					dir = NORTH;
				else
					dir = SOUTH;
				sp--;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '"':
STR_OP:
				goto STRING;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case ':':
DUB_OP:
				vm->stack[sp+1] = vm->stack[sp];
				sp++;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '\\':
SWP_OP:
				tmp = vm->stack[sp];
				vm->stack[sp] = vm->stack[sp-1];
				vm->stack[sp-1] = tmp;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '$':
POP_OP:
				sp--;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '.':
PRI_OP:
				printf("%d",vm->stack[sp]);
				sp--;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case ',':
PRC_OP:
				putchar(vm->stack[sp]);
				sp--;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '#':
SKP_OP:
				pc_incr(&pcI, &pcJ, dir);
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case 'g':
GET_OP:
				vm->stack[sp-1] =
					vm->grid[(int)(vm->stack[sp])][(int)(vm->stack[sp-1])];
				sp--;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case 'p':
PUT_OP:
				vm->grid[(int)(vm->stack[sp])][(int)(vm->stack[sp-1])] =
					vm->stack[sp-2];
				sp-=3;
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '&':
RDI_OP:
				fgets(buf, 20, stdin);
				vm->stack[++sp] = atoi(buf);
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '~':
RDC_OP:
				vm->stack[++sp]=getchar();
				pc_incr(&pcI,&pcJ,dir);NEXT_INSTRUCTION;break;

			case '@':
HLT_OP:
				if(trace){
					vm_print_grid(vm);
				}
				return;
			default:
				vm->stack[++sp]=opcode-'0';
		}
		pc_incr(&pcI, &pcJ, dir);
	}

STRING:
	/*STRING MODE*/
	if(trace){
		fprintf(stderr,"STRING MODE ON(%d,%d,%d)\n",pcI,pcJ,dir);
	}
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
	if(trace){
		fprintf(stderr,"STRING MODE OFF(%d,%d,%d)\n",pcI,pcJ,dir);
	}
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
	// vm_print_grid(vm);
	vm_exec(vm,0,0,EAST,1);
	printf("\n");
}
