name = EXP_2_1
objects = $(name).o

test : $(objects)
	g++ -g -o $(name) $(objects) -lpthread

test.o : $(name).cpp sem.h
	g++ -g -c $(name).cpp

clean :
	rm $(name) $(objects)
