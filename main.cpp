#include <iostream>
#include "polyhedra.h"

void getPath(char* path, std::string& output)
{ if (path==0) return;
  int length=0;
  output= path;
  while (output[length]!=0 && length<150)
    length++;
  for (int i=length-1;i>=0;i--)
  {
#ifdef WIN32
		if (output[i]=='\\' )
    { output.resize(i+1);
      return;
    }
#else
		if (output[i]=='/')
    { output.resize(i+1);
      return;
    }
#endif
  }
}


int main(int argc, char **argv)
{ std::string inputPath, tempS;
  getPath(argv[0], inputPath);
  std::cout<<"Weyl group letter (A B C D E or F) (capital letter pls): ";
  char tempC;
  int tempI;
  std::cin>> tempC;
  std::cout << "\nRank of root system (<=8 pls): ";
  std:: cin>> tempI;
  DyckPaths tempD;
  tempD.AmbientWeyl.MakeArbitrary(tempC, tempI);
  tempD.GenerateAllDyckPathsTypesABC();
  tempD.ComputeDebugString(true);
  std::cout<< tempD.DebugString;

  std::string chamberFileName;
  std::fstream chamberFile;
  chamberFileName=inputPath;
  chamberFileName.append("DyckPaths.html");
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(chamberFile, chamberFileName, false, true, false);
  chamberFile<<"<HTML><BODY>\n Input: \n"<< tempD.DebugString<<"</BODY></HTML>";
  chamberFile.close();

	return 0;   // To avoid Apache errors.
}




