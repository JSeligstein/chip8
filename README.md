chip8
=====

This was written by Joel Seligstein in about a day on 10/14/14 to
experiment with writing an emulator/assembler/disassembler.

The code isn't yet fully complete and there may be a problem with
certain opcodes still.  The code isn't really commented at all
since this was originally meant to just be an experiment.

Thanks to portaudio and cimg for easy to use libraries.  Thanks to
Cowgod's technical refrence as well:
http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

directories
===========

c8: the emulator
----------------
This is the main emulator and takes a path to a rom to execute.  If you edit
some of the #defines, you can have it output all the opcodes and memory along
an execution

c8asm: the assembler
--------------------
This is the main assembler and will understand most opcodes.  They generally
match the standard chip-8 instructions here:
http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

Here are some general rules:
* labels must be at the beginning of a line and end with a colon
* instructions must not be at the beginning of a line
* vx variables must be in the format: v[0-f]
* hex numbers can be indicated with $ or 0x
* you can define explicit bytes with .ds <byte1>, <byte2> in even number of bytes
* .ds must be at the start of a line
* you can store an address by NAME=address at the beggining of a line and use it like a label

Assembly should be passed in and it takes an output file as a command line
arg.  If that arg is "-t" then it will output human readable bytes instead.

Known bugs/issues:
* allows multiple labels with the same name
* no expression support
* no binary support

c8disasm: the disassembler
--------------------------
Given a compiled binary, this will output a reasonable readable code output

Known bugs/issues:
* if a label is referred to but isn't an even byte number, it won't get 
        displayed.  (this generally is a bad idea here anyway)
* it is quite hard/impossible to decipher a .ds instruction, so some
        data will look like gibberish if defined using .ds

