#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

#ifndef DEFAULT_STACK_SIZE
	#define DEFAULT_STACK_SIZE (2<<19)// 2^20
#endif

#ifndef DEFAULT_HEAP_SIZE
	#define DEFAULT_HEAP_SIZE (2<<23)//2^24
#endif

#ifndef TWO_COMPLEMENT

#if -1==~0
	#define TWO_COMPLEMENT 1
#else
	#define TWO_COMPLEMENT 0
#endif

#endif

#ifndef TYPES
#define TYPES
typedef int32_t index_t;
typedef int64_t value_t;
#define V_FMT "%" PRId64
#define I_FMT "%" PRId32
#if TWO_COMPLEMENT
typedef struct{
	value_t a,b;
} cell_t;
#else
typedef struct cell_t{
	value_t a,b;
	uint8_t mark;
} cell_t;
#endif

typedef struct{
	index_t top;
	value_t elements[DEFAULT_STACK_SIZE];
} stack_t;

typedef struct{
	cell_t* freelist;
	cell_t elements[DEFAULT_HEAP_SIZE];
} heap_t;
#endif
