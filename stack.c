#include "vm.h"
void stack_push(stack_t *stack, value_t val){
	if(stack->top>=(DEFAULT_STACK_SIZE-1)){
		fprintf(stderr,"Stack overflow\n");
		exit(1);
	}
	stack->elements[++(stack->top)]=val;
}
value_t stack_pop(stack_t *stack){
	if(stack->top==-1)
		return 0;
	return stack->elements[stack->top--];
}
void stack_init(stack_t *stack){
	stack->top=-1;
}

void stack_print(stack_t *stack){
	for(index_t ii=stack->top; ii>=0; ii--){
		fprintf(stderr,V_FMT "\n",stack->elements[ii]);
	}
}


long stack_peek(stack_t *stack){
	if(stack->top==-1)
		return 0;
	return stack->elements[stack->top];
}
