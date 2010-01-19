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
#include "include/LexException.h"
#include <string>
  //void yyerror(char *);
 using namespace std;
 using namespace ONLINE_MATLAB;
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


{space}    {  
  ;       /* ignore whitespace */
}

.               { 
  // printf("unknown character %s.\n", yytext);
  // do not call yyerror, just throw an exception
  std::string reason("Unknown character ");
  std::string illegal(yytext);
  reason += illegal;
  throw LexException(reason);
   }  /* otherwise spew warnings */

%%

int yywrap(void) {
    return 1;
}

