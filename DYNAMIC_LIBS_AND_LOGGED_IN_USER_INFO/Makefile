main: main.o lib.so.0.1
	gcc -g -Wall -o main main.o -ldl

main.o: main.c
	gcc -Wall -g -c main.c

lib.so.0.1: lib.o
	gcc -shared -fPIC -o lib.so.0.1 lib.o

lib.o: lib.c
	gcc -Wall -c -fPIC lib.c

clean: 
	rm *.o main lib.so.0.1