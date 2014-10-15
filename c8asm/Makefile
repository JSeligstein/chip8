CC=g++ -g 
INCDIRS=-I/opt/local/include
LIBDIRS=-L/opt/local/lib/
LIBS=-lfl -lm
CFLAGS=$(INCDIRS) $(LIBDIRS) $(LIBS)

ASMFILES=c8asm.lex c8asm.y

EXECUTABLE=c8asm
SOURCES=c8asm_instruction.cpp c8asm.cpp
OBJECTS=$(SOURCES:.cpp=.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(ASMFILES)
	flex c8asm.lex
	yacc -d c8asm.y
	$(CC) -o $(EXECUTABLE) $(CFLAGS) $(OBJECTS) lex.yy.c y.tab.c

.cpp.o:
	$(CC) $(INCDIRS) -c $<

clean:
	rm -fr *.o c8asm c8asm.dSYM lex.yy.c y.tab.c y.tab.h
