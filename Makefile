all: compiler

OBJS = grammar.o \
		token.o  \
		CodeGen.o \
		utils.o \
		main.o	 \
		ObjGen.o \

LLVMCONFIG = llvm-config-3.9
CPPFLAGS = `$(LLVMCONFIG) --cppflags` -std=c++11 -Wdeprecated-register
LDFLAGS = `$(LLVMCONFIG) --ldflags` -lpthread -ldl -lz -lncurses -rdynamic
LIBS = `$(LLVMCONFIG) --libs`

clean:
	$(RM) -rf grammar.cpp grammar.hpp compiler tokens.cpp *.output $(OBJS)

ObjGen.cpp: ObjGen.h

CodeGen.cpp: CodeGen.h

grammar.cpp: grammar.y ASTNodes.h
	bison -d -o $@ $<

grammar.hpp: grammar.cpp

token.cpp: token.l grammar.hpp
	flex -o $@ $<

%.o: %.cpp
	g++ -c $(CPPFLAGS) -o $@ $<

compiler: $(OBJS)
	g++ $(CPPFLAGS) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

test: compiler test.c
	cat test.c | ./compiler
