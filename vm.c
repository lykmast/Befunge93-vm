#include <stdio.h>
#include <stdlib.h>
#include "vm.h"

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
			vm_print_instr(vm->grid, ii, jj);
			if(jj==0 && ii!=0)
				printf("\n");
		}
}

void vm_exec(VM *vm, int startI, int startJ, dir_t dir, bool trace){
	vm->pcI=startI; vm->pcJ=startJ; vm->dir=EAST;
}



void vm_print_instr(char grid[ROWS][COLS], int pcI, int pcJ){
	printf("%c",grid[pcI][pcJ]);
}
void vm_print_stack(int *stack, int count);

int main(int argc, char const *argv[]) {
	/*--------------PREPROCESSING--------------*/
	if (argc!=2){
		perror("Wrong number of arguments\n");
	}
	char grid[ROWS][COLS];
	preprocess(argv[1], grid);
	vm_create(grid);

}
