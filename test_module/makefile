.PHONY : clean

all: test_main.o
	g++ test_main.o -o test_module

test_main.o:
	g++ -c test_main.cpp -o test_main.o

clean:
	rm *.o test_module*
