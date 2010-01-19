#include "../include/Command.h"
#include "../parser.h"
#include "../yacc.tab.h"
#include "../include/ParseException.h"
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
      try {
        yyparse();
        std::cout << "************* run input " << mInput << std::endl;
        mOutput = gOutput.str();
        cout << "*************** return output " << mOutput << endl << endl;
      } catch (ParseException &e)
         {
           mOutput = e.getReason();
           // TODO           e.cleanup();  a virtual function to do clean up according to real type
           cout << "inside exception handling " << mOutput << endl;
         }
      yy_delete_buffer (my_string_buffer);
      signal();
      unlock();
    }


} // namespace
