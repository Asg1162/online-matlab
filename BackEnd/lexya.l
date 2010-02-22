numeric [-+]?[0-9]*\.?[0-9]+
word	[a-zA-Z_]+[a-zA-Z_0-9]*
illegal [0-9]+[a-zA-Z_]+
space	[ \t\n]+
space1	[ \t\n]
row	{space}?{numeric}({space}{numeric})*{space}?
matrix_item  [ ]*{word}[ ]*
index_range {numeric}?{space}?:({space}?{numeric}{space}?:)?({space}?{numeric})?

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
{illegal}    {
					std::string reason("Illegal variable name ");
				  std::string illegal(yytext);
				  reason += illegal;
				  throw LexException(reason);			
			 }

{word}       { 
                yylval.varName = strdup(yytext);
                return VARIABLE;
             }

[-()<>=+*/;,{}.\'] {
                return *yytext;
             }

{numeric}    {
                yylval.nuValue = atof(yytext);
                return NUMBER;
             }
{index_range}  {
                yylval.index_range = strdup(yytext);
                return INDEX_RANGE;
 
             }
{space}    {  
  ;       /* ignore whitespace */
}
\".*\"     {
        yylval.string = strdup(yytext);
        return STRING;
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

