CC=g++ -g -std=c++0x 
INCDIRS=-I/usr/X11/include -I/opt/local/include -Iportaudio/include
LIBDIRS=-L/usr/X11/lib -L/opt/local/lib/ -Lportaudio/lib/
DIRS=$(INCDIRS) $(LIBDIRS)
LIBS=-lm -lX11 -lportaudio
CFLAGS=$(DIRS) $(LIBS)

EXECUTABLE=c8
SOURCES=chip8_display.cpp chip8.cpp c8.cpp
OBJECTS=$(SOURCES:.cpp=.o)

all: $(EXECUTABLE) $(SOURCES)

$(EXECUTABLE): portaudio $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(OBJECTS)

.cpp.o:
	$(CC) $(INCDIRS) -c $<

portaudio: portaudio/lib/libportaudio.la

portaudio/lib/libportaudio.la:
	cd portaudio/; ./configure
	make -C portaudio

clean:
	rm -fr *.o c8 c8.dSYM
	make -C portaudio clean

force_check:
	true

