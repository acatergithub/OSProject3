
all : test
test: ../my_vm.h
	gcc multi_test.c -L../ -g3 -pthread -lmy_vm -m32 -o test

clean:
	rm -rf test
