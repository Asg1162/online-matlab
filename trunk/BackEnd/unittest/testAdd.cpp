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

using namespace std;
using namespace Matcloud;

extern int yyparse();
extern FILE *yyin;
extern std::stringstream gOutput;

struct yy_buffer_state;
yy_buffer_state *yy_scan_string( const char *yy_str );

using namespace ONLINE_MATLAB;
Matlab *gMatlab;
std::string gCurUser;

void yy_delete_buffer (yy_buffer_state *b );

void runtest(const char * input, std::string &output,std::string &username)
{
  Command *command = new Command(input, output, username);
  command->lock();
  gMatlab->getCritiqueQueue()->push(command);
  cout << "received command " << input << endl;
  command->wait();
  cout << "wait done " << input << endl;      
  delete command;
  std::string output1 = gOutput.str();
  cout << "return output " << output1 << endl;

}

int main(int argc, char **argv)
{
  gMatlab  = new Matlab(0);

  string username = "xwu";
  gCurUser = username;
  gMatlab->newUser(gCurUser);

  //yy_flex_debug = 1;
  std::string input = "[u s v] = svd([1 2 ;  3 1]);";
  std::string output;
 
  runtest("a = [1 2 3; 3 4 5];" , output, username);
  runtest("b = [1 2 3 ;3 4 5];" , output, username);
  runtest("c = a-b;" , output, username);
 
  //  yyin = stdin; //std::cin; //fopen("input", "r");
  /*      yy_buffer_state *my_string_buffer  = yy_scan_string(input.c_str());
          yyparse();
          printf("a=%f\n", gMatlab->getUser("xwu")->getVar("a")->getScalaValue()) ;
          printf("here in main1.\n");
          yy_delete_buffer (my_string_buffer);
          
          printf("here in main.\n");
          //   input = "x = a * rand(2, 2);"; // + rand(2, 2);";
          input = "[ U S V ] = svd([2 0; 0 1]);"; // + rand(2, 2);";
          my_string_buffer  = yy_scan_string(input.c_str());
          yyparse();
          printf("U = %f.\n", gMatlab->getUser("xwu")->getVar("U")->getScalaValue()) ;
          printf("V = %f.\n", gMatlab->getUser("xwu")->getVar("V")->getScalaValue()) ;
          printf("S = %f.\n", gMatlab->getUser("xwu")->getVar("S")->getScalaValue()) ;
          yy_delete_buffer (my_string_buffer);*/
  
  
  delete gMatlab;
  
  return 0;

}
