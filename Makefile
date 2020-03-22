CC=gcc

nodebug: vm.c vm.h ## Compile with debug options disabled (default choice)
	$(CC) -o vm -DDEBUG=0 vm.c

debug: vm.c vm.h ## Compile with debug options enabled
	$(CC) -o vm -DDEBUG=1 vm.c

clean: ## Delete executables
	rm -f vm

help:  ## Display this help
	@awk 'BEGIN {FS = ":.*##"; printf "\nUsage:\n  make \033[36m<target>\033[0m\n\nTargets:\n"} /^[a-zA-Z_-]+:.*?##/ { printf "  \033[36m%-10s\033[0m %s\n", $$1, $$2 }' $(MAKEFILE_LIST)
