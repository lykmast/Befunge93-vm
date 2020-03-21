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
				fprintf(stderr,"Expected newline at <%d,%d>\n",ii,jj);
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
void vm_exec(VM *vm, int startI, int startJ, dir_t start_dir){
	int pcI=startI; int pcJ=startJ; dir_t dir=start_dir;
	int sp=-1;
	char tmp;
	char buf[20];
NORMAL:
	/*NORMAL MODE*/
	while(1){
		char opcode=vm->grid[pcI][pcJ];

#if DEBUG
		vm_print_state(vm, pcI, pcJ, dir, sp);
#endif


		switch (opcode) {
			case ' ':
				break;
			case '+':
				vm->stack[sp-1] = vm->stack[sp-1] + vm->stack[sp];
				sp--;
				break;

			case '-':
				vm->stack[sp-1] = vm->stack[sp-1] - vm->stack[sp];
				sp--;
				break;

			case '*':
				vm->stack[sp-1] = vm->stack[sp-1] * vm->stack[sp];
				sp--;
				break;

			case '/':
				vm->stack[sp-1] = vm->stack[sp-1] / vm->stack[sp];
				sp--;
				break;

			case '%':
				vm->stack[sp-1] = vm->stack[sp-1] % vm->stack[sp];
				sp--;
				break;

			case '!':
				vm->stack[sp] = !(vm->stack[sp]);
				break;

			case '`':
				vm->stack[sp-1] = vm->stack[sp-1] > vm->stack[sp];
				sp--;
				break;

			case '>':
				dir = EAST;
				break;

			case '<':
				dir = WEST;
				break;

			case '^':
				dir = NORTH;
				break;

			case 'v':
				dir = SOUTH;
				break;

			case '?':
				dir = (dir_t)(rand()%4);
				break;

			case '_':
				if(vm->stack[sp])
					dir = WEST;
				else
					dir = EAST;
				sp--;
				break;

			case '|':
				if(vm->stack[sp])
					dir = NORTH;
				else
					dir = SOUTH;
				sp--;
				break;

			case '"':
				goto STRING;
				break;

			case ':':
				vm->stack[sp+1] = vm->stack[sp];
				sp++;
				break;

			case '\\':
				tmp = vm->stack[sp];
				vm->stack[sp] = vm->stack[sp-1];
				vm->stack[sp-1] = tmp;
				break;

			case '$':
				sp--;
				break;

			case '.':
				printf("%d",vm->stack[sp]);
				sp--;
				break;

			case ',':
				putchar(vm->stack[sp]);
				sp--;
				break;

			case '#':
				pc_incr(&pcI, &pcJ, dir);
				break;

			case 'g':
				vm->stack[sp-1] =
					vm->grid[(int)(vm->stack[sp])][(int)(vm->stack[sp-1])];
				sp--;
				break;

			case 'p':
				vm->grid[(int)(vm->stack[sp])][(int)(vm->stack[sp-1])] =
					vm->stack[sp-2];
				sp-=3;
				break;

			case '&':
				fgets(buf, 20, stdin);
				vm->stack[++sp] = atoi(buf);
				break;

			case '~':
				vm->stack[++sp]=getchar();
				break;

			case '@':
				return;
			default:
				vm->stack[++sp]=opcode-'0';
		}
		pc_incr(&pcI, &pcJ, dir);
	}

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
				vm->stack[++sp] = opcode;
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
		perror("Wrong number of arguments\n");
	}
	srand(time(NULL));
	char grid[ROWS][COLS];
	preprocess(argv[1], grid);
	VM* vm = vm_create(grid);
	vm_exec(vm,0,0,EAST);
	printf("\n");
}
