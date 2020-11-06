objects = test.o

test : $(objects)
	gcc -o test $(objects)

test.o : test.c
	gcc -c test.c

clean :
	rm test $(objects)
