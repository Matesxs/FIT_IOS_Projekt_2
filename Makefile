CLIST = $(wildcard *.c)

all: build

build: $(CLIST)
	gcc -std=gnu99 -Wall -Wextra -Werror -pedantic $(CLIST) -o proj2 -lrt -lpthread -g

.PHONY: run
run: build
	@./proj2 5 3 600 435

.PHONY: cleand
cleand:
	chmod +x clean.sh
	./clean.sh

.PHONY: clean
clean:
	@-rm proj2
	@-rm proj2.out
	@-rm -rf cmake-build-debug/
	@-rm -rf docs/