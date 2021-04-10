CLIST = $(wildcard *.c)

all: build run

build: $(CLIST)
	gcc -std=gnu99 -Wall -Wextra -Werror -pedantic $(CLIST) -o proj2 -lrt -lpthread

.PHONY: run
run:
	@./proj2 4 3 300 755

clean:
	@-rm proj2
	@-rm proj2.out
	@-rm -rf cmake-build-debug/
	@-rm -rf docs/