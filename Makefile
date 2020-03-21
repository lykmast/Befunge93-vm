CC=gcc

nodebug: vm.c vm.h
	$(CC) -o vm -DDEBUG=0 vm.c

debug: vm.c vm.h
	$(CC) -o vm -DDEBUG=1 vm.c

clean:
	rm vm
