all: compiler

OBJS = grammar.o \
		token.o  \
		main.o	 \

LLVMCONFIG = llvm-config
CPPFLAGS = `$(LLVMCONFIG) --cppflags` -std=c++11
LDFLAGS = `$(LLVMCONFIG) --ldflags` -lpthread -ldl -lz -lncurses -rdynamic
LIBS = `$(LLVMCONFIG) --libs`

grammar.cpp: grammar.y ASTNodes.h
	bison -d -o $@ $<

grammar.hpp: grammar.cpp

token.cpp: token.l grammar.hpp
	flex -o $@ $<

%.o: %.cpp
	g++ -c $(CPPFLAGS) -o $@ $<

compiler: $(OBJS)
	g++ -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

test: compiler test.input
	cat test.input | ./compiler
