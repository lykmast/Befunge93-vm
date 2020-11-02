CC=gcc
CFLAGS=-Wall

SOURCES= vm.c \
         stack.c \
         heap.c \
         gc.c \
         mark.c \
         mark_reference.c \
         heap_elements.c \
         DFSops.c

OBJECTS=$(SOURCES:.c=.o)


nodebug: CFLAGS+= -O2 -DDEBUG=0## Compile with debug options disabled (default choice)

nodebug: out=befunge93+

debug: CFLAGS+= -Og -g -DDEBUG=1## Compile with debug options enabled

debug: out=vm

nodebug debug: befunge93+

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

mark.h: types.h

heap_elements.h: mark.h

DFSOps.h: types.h mark_reference.h

stack.h: types.h

heap.h: types.h heap_elements.h

vm.h: types.h stack.h heap.h

gc.h: vm.h mark.h

befunge93+: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(out) $(OBJECTS)

clean: ## Delete executables
	rm -f vm befunge93+ *.o

help:  ## Display this help
	@awk 'BEGIN {FS = ":.*##"; printf "\nUsage:\n  make \033[36m<target>\033[0m\n\nTargets:\n"} /^[a-zA-Z_-]+:.*?##/ { printf "  \033[36m%-10s\033[0m %s\n", $$1, $$2 }' $(MAKEFILE_LIST)
