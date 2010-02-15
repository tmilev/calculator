#include <exception>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include "polyhedra.h"
void getPath(char* path, std::string& output)
{ int length=0;
  if (path==0) return;
  while (path[length]!=0)
    length++;
  output= path;
  for (int i=length-1;i>=0;i--)
  { if (path[i]=='/')
    { output.resize(i+1);
      return;
    }
  }
}

int main(int argc, char **argv)
{ std::cout << "Content-Type: text/html\n\n";
  //std::cout <<  "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
   //         << std::endl;
  //std::cout << "<html lang=\"en\" dir=\"LTR\">"<< std::endl;
  std::string tempS;
  getPath(argv[0],tempS);
  std::stringstream out;
  out<<tempS;
  out<<"../../vector_partition.html";
  tempS=out.str();
  std::fstream fileHeaderHtml;
  rootFKFTcomputation::OpenDataFile(fileHeaderHtml,tempS);
  char buffer[1024];
  while (!fileHeaderHtml.eof())
  { fileHeaderHtml.read(buffer,1024);
    std::cout.write(buffer, fileHeaderHtml.gcount());

  }
//  std::cout << tempS;
  //fileHeaderHtml
  return 0;   // To avoid Apache errors.
}
