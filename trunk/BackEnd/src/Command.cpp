#include "../include/Command.h"
#include "../parser.h"
#include "../yacc.tab.h"
#include <string>
#include <sstream>
extern int yy_flex_debug;
extern int yyparse();
extern  std::string gCurUser;
struct yy_buffer_state;
extern yy_buffer_state *yy_scan_string( const char *yy_str );
void yy_delete_buffer (yy_buffer_state *b );

std::stringstream gOutput;

namespace ONLINE_MATLAB{

  void Command::run()
    {
      lock();
      gCurUser = mCurUser;
      yy_buffer_state *my_string_buffer  = yy_scan_string(mInput);
      yyparse();
      yy_delete_buffer (my_string_buffer);

      std::cout << "******* run input " << mInput << std::endl;
      mOutput = gOutput.str();
      signal();
      std::cout << "command signal. " << this << std::endl;
      unlock();
    }


} // namespace
