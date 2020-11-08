name = EXP_4
objects = $(name).o

test : $(objects)
	g++ -o $(name) $(objects)

test.o : $(name).cpp sem.h
	g++ -c $(name).cpp

clean :
	rm $(name) $(objects)
