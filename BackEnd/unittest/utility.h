#pragma once

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

using namespace std;

void runtest(const char * input, std::string &output,std::string &username)
{
  Command *command = new Command(input, output, username);
  command->lock();
  gOutput.str("");
  gMatlab->getCritiqueQueue()->push(command);
  cout << "*************** received command " << input << endl;
  command->wait();
  delete command;
  std::string output1 = gOutput.str();
  cout << "*************** return output " << output1 << endl << endl;

}
