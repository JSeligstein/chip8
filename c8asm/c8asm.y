%union
{
  char *s;
  double d;
  unsigned int i;
  char c;
} 

%{
int yylex();
int yyparse();
%}

%{

#include "c8asm.hpp"
#include "c8asm_instruction.hpp"
#include <stdlib.h>

using namespace std;
extern unsigned int yylineno;

void yyerror(const char *str) {
    fprintf(stderr, "error: %s, near line: %d\n",str, yylineno);
}
 
int yywrap() {
    return 1;
} 

uint8_t hex2dec(char h) {
    switch (h) {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'a': case 'A': return 10;
        case 'b': case 'B': return 11;
        case 'c': case 'C': return 12;
        case 'd': case 'D': return 13;
        case 'e': case 'E': return 14;
        case 'f': case 'F': return 15;
    }
    printf("invalid hex2dec: %c\n", h);
    yyerror("invalid hex2dec");
    exit(1);
}

int main(int argc, char **argv) {
    yyparse();

    if (argc == 2 && strncmp(argv[1], "-t", 2) == 0) {
        c8asm::inst().dumpReadableBytes();
        return 0;
    } else if (argc < 2) {
        printf("usage: %s -t\n", argv[0]);
        printf("usage: %s <outfile>\n", argv[1]);
        return 1;
    } else {
        c8asm::inst().dumpBytes(argv[1]);
    }
} 

%}

/* config */
%error-verbose

/* generic tokens */
%token <s> LABEL COMMA BEGINNING_WHITESPACE WORD NEWLINE COMMENT DEFINE_BYTE EQUALS
%token <i> NUMBER HEX1 HEX2 HEX3 HEX4

/* instructions */
%token <s> INSTR_ADD  INSTR_AND  INSTR_CALL INSTR_CLS  INSTR_DRW
%token <s> INSTR_JP   INSTR_LD   INSTR_OR   INSTR_RET  INSTR_RND
%token <s> INSTR_SE   INSTR_SHL  INSTR_SHR  INSTR_SKNP INSTR_SKP
%token <s> INSTR_SNE  INSTR_SYS  INSTR_SUB  INSTR_SUBN INSTR_XOR

/* registers and values*/
%token <s> RI ST DT BCD KEY FONT RI_INDIRECT
%token <c> VX

/* return types */
%type <i> byte nibble address

%%
commands: /* empty */
        | commands command
        ;

command:  eol
        | BEGINNING_WHITESPACE eol
        | LABEL eol {
            c8asm::inst().markLabel($1);
        } | LABEL instruction eol {
            c8asm::inst().markLabel($1, -2);
        } | WORD EQUALS address {
            // a bit of a hack here - 0x200 gets added to every thing later
            c8asm::inst().markLabelAt($1, $3-0x200);
        } | DEFINE_BYTE db_sequence eol {
        } | BEGINNING_WHITESPACE instruction eol {
        };

eol: NEWLINE | COMMENT NEWLINE;

byte: HEX1
    | HEX2
    | NUMBER {
        if ($1 > 255) {
            printf("Instruction required a byte, but got %d\n", $1);
            yyerror("Invalid byte");
        }
        $$ = $1;
    };

nibble: HEX1
      | NUMBER {
        if ($1 > 15) {
            printf("Instruction required a nibble, but got %d\n", $1);
            yyerror("Invalid nibble");
        }
        $$ = $1;
      };

address: HEX1
       | HEX2
       | HEX3
       | HEX4
       | NUMBER {
           if ($1 > 0xffff) {
               // shouldn't really happen on most systems 
               printf("Instruction required a 4-byte address, but got %x\n", $1);
               yyerror("Invalid address");
           }
           $$ = $1;
       };

instruction:
        INSTR_ADD VX COMMA byte {
        c8asm::inst().addInstruction(0x7, hex2dec($2), $4);
    } | INSTR_ADD VX COMMA VX {
        c8asm::inst().addInstruction(0x8, hex2dec($2), hex2dec($4), 4);
    } | INSTR_ADD RI COMMA VX {
        c8asm::inst().addInstruction(0xF, hex2dec($4), 0x1E);
    } | INSTR_AND VX COMMA VX {
        c8asm::inst().addInstruction(0x8, hex2dec($2), hex2dec($4), 0x2);
    } | INSTR_CALL address {
        uint16_t n = (uint16_t)$2;
        c8asm::inst().addInstruction(0x2, (n & 0xf00) >> 8, n & 0xff);
    } | INSTR_CALL WORD {
        c8asm::inst().addInstructionWithLabel(0x2, $2);
    } | INSTR_CLS {
        c8asm::inst().addInstruction(0, 0xe0);
    } | INSTR_DRW VX COMMA VX COMMA nibble {
        c8asm::inst().addInstruction(0xD, hex2dec($2), hex2dec($4), (uint8_t)$6);
    } | INSTR_JP address {
        uint16_t n = (uint16_t)$2;
        c8asm::inst().addInstruction(0x1, (n & 0xf00) >> 8, n & 0xff);
    } | INSTR_JP WORD {
        c8asm::inst().addInstructionWithLabel(0x1, $2);
    } | INSTR_JP VX COMMA address {
        if ($2 != '0') {
            printf("JP VX, addr must be v0, v%x supplied\n", $2);
            yyerror("Invalid vx for JP VX, addr");
        }
        uint16_t n = (uint16_t)$4;
        c8asm::inst().addInstruction(0xB, (n & 0xf00) >> 8, n & 0xff);
    } | INSTR_JP VX COMMA WORD {
        if ($2 != '0') {
            printf("JP VX, addr must be v0, v%x supplied\n", $2);
            yyerror("Invalid vx for JP VX, addr");
        }
        c8asm::inst().addInstructionWithLabel(0xB, $4);
    } | INSTR_LD VX COMMA byte {
        c8asm::inst().addInstruction(0x6, hex2dec($2), $4);
    } | INSTR_LD VX COMMA VX {
        c8asm::inst().addInstruction(0x8, hex2dec($2), hex2dec($4), 0);
    } | INSTR_LD RI COMMA address {
        uint16_t n = (uint16_t)$4;
        c8asm::inst().addInstruction(0xA, (n & 0xf00) >> 8, n & 0xff);
    } | INSTR_LD RI COMMA WORD {
        c8asm::inst().addInstructionWithLabel(0xA, $4);
    } | INSTR_LD VX COMMA DT {
        c8asm::inst().addInstruction(0xF, hex2dec($2), 0x07);
    } | INSTR_LD VX COMMA KEY {
        c8asm::inst().addInstruction(0xF, hex2dec($2), 0x0A);
    } | INSTR_LD DT COMMA VX {
        c8asm::inst().addInstruction(0xF, hex2dec($4), 0x15);
    } | INSTR_LD ST COMMA VX {
        c8asm::inst().addInstruction(0xF, hex2dec($4), 0x18);
    } | INSTR_LD FONT COMMA VX {
        c8asm::inst().addInstruction(0xF, hex2dec($4), 0x29);
    } | INSTR_LD BCD COMMA VX {
        c8asm::inst().addInstruction(0xF, hex2dec($4), 0x33);
    } | INSTR_LD RI_INDIRECT COMMA VX {
        c8asm::inst().addInstruction(0xF, hex2dec($4), 0x55);
    } | INSTR_LD VX COMMA RI_INDIRECT {
        c8asm::inst().addInstruction(0xF, hex2dec($2), 0x65);
    } | INSTR_OR VX COMMA VX {
        c8asm::inst().addInstruction(8, hex2dec($2), hex2dec($4), 0x1);
    } | INSTR_RET {
        c8asm::inst().addInstruction(0, 0xEE);
    } | INSTR_RND VX COMMA byte {
        c8asm::inst().addInstruction(0xC, hex2dec($2), $4);
    } | INSTR_SE VX COMMA byte {
        c8asm::inst().addInstruction(0x3, hex2dec($2), $4);
    } | INSTR_SE VX COMMA VX {
        c8asm::inst().addInstruction(0x5, hex2dec($2), hex2dec($4), 0);
    } | INSTR_SHL VX {
        c8asm::inst().addInstruction(0x8, hex2dec($2), 0x0E);
    } | INSTR_SHR VX {
        c8asm::inst().addInstruction(0x8, hex2dec($2), 0x06);
    } | INSTR_SKNP VX {
        c8asm::inst().addInstruction(0xE, hex2dec($2), 0xA1);
    } | INSTR_SKP VX {
        c8asm::inst().addInstruction(0xE, hex2dec($2), 0x9E);
    } | INSTR_SNE VX COMMA byte {
        c8asm::inst().addInstruction(0x4, hex2dec($2), $4);
    } | INSTR_SNE VX COMMA VX {
        c8asm::inst().addInstruction(0x9, hex2dec($2), hex2dec($4), 0);
    } | INSTR_SYS address {
        uint16_t n = (uint16_t)$2;
        c8asm::inst().addInstruction(0x0, (n & 0xf00) >> 8, n & 0xff);
    } | INSTR_SYS WORD {
        c8asm::inst().addInstructionWithLabel(0x0, $2);
    } | INSTR_SUB VX COMMA VX {
        c8asm::inst().addInstruction(0x8, hex2dec($2), hex2dec($4), 0x5);
    } | INSTR_SUBN VX COMMA VX {
        c8asm::inst().addInstruction(0x8, hex2dec($2), hex2dec($4), 0x7);
    } | INSTR_XOR VX COMMA VX {
        c8asm::inst().addInstruction(0x8, hex2dec($2), hex2dec($4), 0x3);
    } | WORD {
        printf("Invalid instruction: %s\n", $1);
    };

db_sequence: 
      byte {
        c8asm::inst().addInstruction($1);
    } | db_sequence COMMA byte {
        c8asm::inst().addInstruction($3);
    };

%%


