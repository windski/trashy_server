CFLAGS = -g -Wall -I../src
CXX = g++

OBJ = test_main.o
.PHONY : clean

all: $(OBJ)
	$(CXX) $(OBJ) -o test_module

$(OBJ): %.o: %.cpp
	$(CXX) -c $(CFLAGS) $< -o $@

clean:
	- @ rm *.o test_module*
