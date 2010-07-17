#include <iostream>
#ifndef WIN32
#include <unistd.h>
#endif
#include "polyhedra.h"

extern int GlobalPointerCounter;

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
{ std::string inputString, inputPath, tempS;
  getPath(argv[0], inputPath);
  inputPath.append("../polyhedra.h");
  CGIspecificRoutines::FormatCPPSourceCode(inputPath);
  std::cout << inputPath;
  std::cin>>inputPath;
	return 0;   // To avoid Apache errors.
}
