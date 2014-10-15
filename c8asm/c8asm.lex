%option yylineno

%{
#include <stdio.h>
#include <string.h>
#include "y.tab.h"

extern uint8_t hex2dec(char h);

%}

%%

;[^\n]*                 return COMMENT;
,                       return COMMA;
\n                      return NEWLINE;
^[ \t]+                 return BEGINNING_WHITESPACE;
=                       return EQUALS;

^[A-Za-z][A-Za-z0-9_]*:  {
    yylval.s = strndup(yytext, strlen(yytext)-1);
    return LABEL;
}

[0-9]+                  {
    yylval.i = atoi(yytext);
    return NUMBER;
}


(v|V)[0-9A-Fa-f]            {
    yylval.c = yytext[1];
    return VX;
}

(?i:add)    return INSTR_ADD;
(?i:and)    return INSTR_AND;
(?i:call)   return INSTR_CALL;
(?i:cls)    return INSTR_CLS;
(?i:drw)    return INSTR_DRW;
(?i:jp)     return INSTR_JP;
(?i:ld)     return INSTR_LD;
(?i:or)     return INSTR_OR;
(?i:ret)    return INSTR_RET;
(?i:rnd)    return INSTR_RND;
(?i:se)     return INSTR_SE;
(?i:shl)    return INSTR_SHL;
(?i:shr)    return INSTR_SHR;
(?i:sknp)   return INSTR_SKNP;
(?i:skp)    return INSTR_SKP;
(?i:sne)    return INSTR_SNE;
(?i:sys)    return INSTR_SYS;
(?i:sub)    return INSTR_SUB;
(?i:subn)   return INSTR_SUBN;
(?i:xor)    return INSTR_XOR;

(?i:"["i"]")  return RI_INDIRECT;
(?i:i)        return RI;
(?i:st)       return ST;
(?i:dt)       return DT;
(?i:b)        return BCD;
(?i:k)        return KEY;
(?i:f)        return FONT;

(?i:\.db)     return DEFINE_BYTE;
  
((0x)|\$)[0-9A-Fa-f] {
    int idx = yytext[0] == '$' ? 1 : 2;
    yylval.i = hex2dec(yytext[idx]);
    return HEX1;
}

((0x)|\$)[0-9A-Fa-f]{2}   {
    int idx = yytext[0] == '$' ? 1 : 2;
    yylval.i = (hex2dec(yytext[idx]) << 4)
               | hex2dec(yytext[idx+1]);
    return HEX2;
}

((0x)|\$)[0-9A-Fa-f]{3}   {
    int idx = yytext[0] == '$' ? 1 : 2;
    yylval.i = (hex2dec(yytext[idx]) << 8)
               | (hex2dec(yytext[idx+1]) << 4)
               | hex2dec(yytext[idx+2]);
    return HEX3;
}

((0x)|\$)[0-9A-Fa-f]{4}   {
    int idx = yytext[0] == '$' ? 1 : 2;
    yylval.i = (hex2dec(yytext[idx]) << 12)
               | (hex2dec(yytext[idx+1]) << 8)
               | (hex2dec(yytext[idx+2]) << 4)
               | hex2dec(yytext[idx+3]);
    return HEX4;
}

[A-Za-z][A-Za-z0-9_]* {
    yylval.s = strdup(yytext);
    return WORD;
}

[ \t\n\r]            /* skip whitespace */

%%
