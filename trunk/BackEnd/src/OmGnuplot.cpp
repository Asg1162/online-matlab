#include "../include/OmGnuplot.h"
#include "../include/ExeException.h"
#include <time.h>
#include <fstream>
#include <ctime>
#include <unistd.h>

#include <stdlib.h>

 extern std::string gCurUser;
namespace ONLINE_MATLAB{

  using namespace std;
  

  static char filePath[] = "/home/xwu3/matcloud/mysite/media/users/";


  void OmGnuplot :: createScript()
  {
    stringstream scriptFile;
    scriptFile << filePath << gCurUser << "/" << gCurUser << "_gnuplotscript" << ".scr" ;
    mScriptFile = scriptFile.str();
    ofstream sfile(mScriptFile.c_str(), ios::out);        

    // create the final graphic file name
    time_t seconds;
    seconds = time (NULL);
    stringstream ss, ss1;
    ss1 << gCurUser << "/" << gCurUser << "_" << seconds << ".png";
    ss << filePath <<  "/" << ss1.str();

    mGraphFile = ss1.str();
    // 
    sfile << "set terminal png " << endl;
    sfile << "set output \"" << ss.str() << "\""<<endl;
    sfile << "set title \"" <<  "\""<<endl;
    sfile << "plot " ;
    for (int i = 0; i < mPairs.size()-1; i++)
      {
        sfile  << "\'" << mPairs[i].getFileName() << "\' "  ;

        if (mPairs[i].getNbCol() == 2)
          sfile <<  "using 1:2 with linespoints title " << "\"" << mPairs[i].getY()->getName() << "\" ," ;
        else
          sfile <<  " with linespoints title " << "\"" << mPairs[i].getY()->getName() << "\"," ;
      }
    // last 
    sfile  << "\'" << mPairs[mPairs.size() - 1].getFileName() << "\' "  ;

    if (mPairs[mPairs.size() - 1].getNbCol() == 2)
      sfile <<  "using 1:2 with linespoints title " << "\"" << mPairs[mPairs.size() - 1].getY()->getName() << "\" " ;
    else
      sfile <<  " with linespoints title " << "\"" <<  mPairs[mPairs.size()-1].getY()->getName() << "\"" ;

    sfile << endl;
    sfile.close();
  }

  void OmGnuplot::launchGnuplot()
  {

    //    execlp("gnuplot", "\"", mScriptFile.c_str(), "\"");
    string command("gnuplot ");
    command += mScriptFile;
    //    command += string("\"");
    cout << command.c_str() << endl;    
    system(command.c_str());
  }

  
  void OmGnuplot::run()
  {
    // first generate data files
    writeToFile();
    
    // the generate script file
    createScript();

    // launch gnuplot
    launchGnuplot();
    
  }

} // namespace
