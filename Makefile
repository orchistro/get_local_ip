.PHONY: all clean

all: foo

SRCS := $(shell find . -name '*.cpp')
OBJS := $(SRCS:.cpp=.o)

foo: $(OBJS)
	gcc -o $@ $^ -lstdc++
%.o: %.cpp
	gcc -std=c++17 -c -g -O3 -Wall -o $@ $<

clean:
	rm -rf foo *.o
