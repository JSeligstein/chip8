

all: c8 c8disasm c8asm

c8: force_look
	$(MAKE) -C c8 all

c8disasm: force_look
	$(MAKE) -C c8disasm all

c8asm: force_look
	$(MAKE) -C c8asm all

clean:
	$(MAKE) -C c8 clean
	$(MAKE) -C c8disasm clean
	$(MAKE) -C c8asm clean

force_look :
	true

