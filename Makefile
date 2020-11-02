CC=gcc

default: ## Compile with debug options disabled (default choice)
	CFLAGS= -DDEBUG=0
debug: ## Compile with debug options enabled
	CFLAGS= -DDEBUG=1

default: befunge93
debug: befunge93

befunge93: vm.c vm.h
	$(CC) -o befunge93 $(CFLAGS) vm.c

clean: ## Delete executables
	rm -f befunge93

help:  ## Display this help
	@awk 'BEGIN {FS = ":.*##"; printf "\nUsage:\n  make \033[36m<target>\033[0m\n\nTargets:\n"} /^[a-zA-Z_-]+:.*?##/ { printf "  \033[36m%-10s\033[0m %s\n", $$1, $$2 }' $(MAKEFILE_LIST)
