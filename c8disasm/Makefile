CC=g++ -g -std=c++0x 
INCDIRS=-I/usr/X11/include -I/opt/local/include
LIBDIRS=-L/usr/X11/lib -L/opt/local/lib/
DIRS=$(INCDIRS) $(LIBDIRS)
LIBS=-lm -pthread -lX11
CFLAGS=$(DIRS) $(LIBS)
FILES=c8disasm.cpp

all: c8disasm

c8disasm: $(FILES)
	$(CC) -o c8disasm $(CFLAGS) $(FILES)
	
clean:
	rm -fr *.o c8disasm c8disasm.dSYM
