numeric [-+]?[0-9]*\.?[0-9]+
word	[a-zA-Z_]+
space	[ \t\n]+
space1	[ \t\n]
row	{space}?{numeric}({space}{numeric})*{space}?
matrix_item  [ ]*{word}[ ]*

%{
#include <stdlib.h>
#include "parser.h"
#include "yacc.tab.h"
void yyerror(char *);
%}

%%


\[{row}(;{row})*\]  {
                yylval.matrix = strdup(yytext);
                return MATRIX;
             }


\[{matrix_item}([ ,]{matrix_item})*\]  {
printf("return matrix-list.\n");  /* %\[{matrix_item}([,|\s]{matrix_item})\s*\]   {*/
                yylval.matrix = strdup(yytext);
                return MATRIX_LIST;
        }


{word}       { 
                yylval.varName = strdup(yytext);
                return VARIABLE;
             }


{numeric}    {
                yylval.nuValue = atof(yytext);
                return NUMBER;
             }


[-()<>=+*/;,{}.] {
                return *yytext;
             }


{space}      printf("ignore space.\n");  ;       /* ignore whitespace */

.               { 
printf("unknown character %s.\n", yytext);
            yyerror((char *)"Unknown character "); }  /* otherwise spew warnings */

%%

int yywrap(void) {
    return 1;
}

