all:
	gcc -g -Wall -o exe -I. main.c vhash.c vmem.c vutil.c -pthread
clean:
	rm -rf exe

