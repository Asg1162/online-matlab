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

class ParserI:public Parser{
 public :
  virtual string command(const string &cmd, const Ice::Current&)
    {
      std::string output;
      // command is in the format of username:parseinput
      int n = cmd.find(':');
      string com = cmd.substr(0,n); // first sub-field is the command
      cout << "received " << cmd << endl;
      if (strcmp(com.c_str(), "CMD") == 0) //  a command
        {
          string rest(cmd.c_str() + n + 1);
          int m = rest.find(":");
          string username = rest.substr(0,m);
          Command *command = new Command(rest.c_str() + m+1, output, username );
          gOutput.str(std::string()); // clear the buffer
          command->lock();
          gMatlab->getCritiqueQueue()->push(command);
          cout << "received command " << cmd << endl;
          command->wait();
          cout << "wait done " << cmd << endl;      
          delete command;
          cout << " generate parsing output " <<  output << endl;
          return output;
        }
      else if (strcmp(com.c_str(),"LOGIN") == 0)
        {
          string username(cmd.c_str() + n + 1);
          // add user space
          cout << " add user " << username << endl;
          // TODO 
          gMatlab->newUser(username);
        }
      else
        {
          assert(strcmp(com.c_str(), "LOGOUT") == 0);
          string username(cmd.c_str() + n + 1);
          cout << " remove user " << username << endl;
          gMatlab->delUser(username);
          
        }
    }
};

int main(int argc, char **argv)
{
  gMatlab  = new Matlab(0);


#if 1
  if (argc != 3)
    {
      // TODO 
      cout << "usage: parser hostname portid" << endl;
      return 1;
    }

  // todo make gpuId from arguments
  /* connect to the load balancer server */
  // TODO initialize GPU
  string lbhost(argv[1]);
  string lbport(argv[2]);
  char hostname[256];
  gethostname(hostname, 256);
  strcat(hostname, ":0"); // gpu id TODO
  // "Backend:tcp -h hostname -p portnumber"
  string lbconfig = "Backend:tcp -h ";
  lbconfig += lbhost;
  lbconfig += " -p ";
  lbconfig += lbport;

  int status = 0;
  Ice::CommunicatorPtr ic;
  try {
    ic = Ice::initialize(argc, argv);
    Ice::ObjectPrx base = ic->stringToProxy(lbconfig.c_str());
    BackendPrx be = BackendPrx::checkedCast(base);
    if (!be)
      throw "Invalid proxy";
    be->addNode(hostname);
  } catch (const Ice::Exception& ex) {
    cerr << ex << endl;
    status = 1;
  } catch (const char* msg) {
    cerr << msg << endl;
    status = 1;
  }

  /* setup the command parsing server */
  try {
    Ice::ObjectAdapterPtr adapter
      = ic->createObjectAdapterWithEndpoints(
                                             "Parser", "tcp -p 10001");
    Ice::ObjectPtr object = new ParserI();
    adapter->add(object, ic->stringToIdentity("Parser"));
    adapter->activate();
    ic->waitForShutdown();
  } catch (const Ice::Exception& e) {
    cerr << e << endl;
    status = 1;
  }    catch (const char* msg) {
    cerr << msg << endl;
    status = 1;
  }
  if (ic) {
    try {
      ic->destroy();
    } catch (const Ice::Exception& e) {
      cerr << e << endl;
      status = 1;
    }
  }
  return status;

#else

  

  string username = "xwu";
  gCurUser = username;
  gMatlab->newUser(gCurUser);

  int n = 1;
  while(n--)
    {
      //yy_flex_debug = 1;
      std::string input = "a = [1 2 ;  3 1];";
      std::string output;

      Command *command = new Command(input.c_str(), output, username);
      command->lock();
      gMatlab->getCritiqueQueue()->push(command);
      cout << "received command " << input << endl;
      command->wait();
      cout << "wait done " << input << endl;      
      delete command;
      std::string output1 = gOutput.str();
      cout << "return output " << output1 << endl;

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

    }
#endif 
  delete gMatlab;

  return 0;

}
