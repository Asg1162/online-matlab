#include "utility.h"

int main(int argc, char **argv)
{
  gMatlab  = new Matlab(0);

  string username = "xwu";
  gCurUser = username;
  gMatlab->newUser(gCurUser);

  //yy_flex_debug = 1;
  std::string output;
 
  
  runtest("a = rand(2, 3);" , output, username);
  runtest("b = rand(3, 2);" , output, username);
  runtest("a * b;" , output, username);  
  runtest("d = rand(2, 2);" , output, username);

 
  //  yyin = stdin; //std::cin; //fopen("input", "r");
  
  delete gMatlab;
  
  return 0;

}
