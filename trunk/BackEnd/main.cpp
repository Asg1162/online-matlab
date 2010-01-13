extern int yy_flex_debug;

#include "parser.h"
#include "yacc.tab.h"
#include <iostream>
#include <string>
#include "include/Matlab.h"
#include "include/Matrix.h"
extern int yyparse();
extern FILE *yyin;

struct yy_buffer_state;
yy_buffer_state *yy_scan_string( const char *yy_str );

using namespace ONLINE_MATLAB;
Matlab *gMatlab;
std::string gCurUser;

void yy_delete_buffer (yy_buffer_state *b );

int main()
{
  // todo make gpuId from arguments
  gMatlab  = new Matlab(0);

  gCurUser = "xwu";
  gMatlab->newUser(gCurUser);

  int n = 1;
  while(n--)
    {
      //yy_flex_debug = 1;
      std::string input = "a = [1 2 ;  2 1];";

      //  yyin = stdin; //std::cin; //fopen("input", "r");
      yy_buffer_state *my_string_buffer  = yy_scan_string(input.c_str());
      yyparse();
      printf("a=%f\n", gMatlab->getUser("xwu")->getVar("a")->getScalaValue()) ;
      printf("here in main1.\n");
      yy_delete_buffer (my_string_buffer);
q
      printf("here in main.\n");
      //   input = "x = a * rand(2, 2);"; // + rand(2, 2);";
      input = "[ U S V ] = svd([2 0; 0 1]);"; // + rand(2, 2);";
      my_string_buffer  = yy_scan_string(input.c_str());
      yyparse();
      printf("U = %f.\n", gMatlab->getUser("xwu")->getVar("U")->getScalaValue()) ;
      printf("V = %f.\n", gMatlab->getUser("xwu")->getVar("V")->getScalaValue()) ;
      printf("S = %f.\n", gMatlab->getUser("xwu")->getVar("S")->getScalaValue()) ;
      yy_delete_buffer (my_string_buffer);

    }
  return 0;

}
