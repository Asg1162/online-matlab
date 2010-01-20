extern int yy_flex_debug;

#include "parser.h"
#include "yacc.tab.h"
#include <iostream>
#include <string>
#include "include/Matlab.h"
#include "include/Matrix.h"
#include "include/Command.h"

#include <Ice/Ice.h>
#include <LoadBalancer.h>

// hostname
#include <unistd.h>
#include "utility.h"

using namespace std;
using namespace Matcloud;

extern int yyparse();
extern FILE *yyin;
extern std::stringstream gOutput;

struct yy_buffer_state;
yy_buffer_state *yy_scan_string( const char *yy_str );

using namespace ONLINE_MATLAB;

void yy_delete_buffer (yy_buffer_state *b );


int main(int argc, char **argv)
{
  gMatlab  = new Matlab(0);

  string username = "xwu3";
  gCurUser = username;
  gMatlab->newUser(gCurUser);

  //yy_flex_debug = 1;
  std::string input = "rand(2, 3)";
  std::string output;

  runtest("a =[1 2 3 4 5 6 7 8 9 10];", output, username);
  runtest("plot(a);" , output, username);

 
  //  yyin = stdin; //std::cin; //fopen("input", "r");
  
  delete gMatlab;
  
  return 0;

}
