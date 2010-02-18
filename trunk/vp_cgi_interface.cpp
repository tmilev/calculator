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
{ ComputationSetup theComputationSetup;
	std::cout << "Content-Type: text/html\n\n";
  //std::cout <<  "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
	//					<< std::endl;
  //std::cout << "<html lang=\"en\" dir=\"LTR\">"<< std::endl;
  std::string tempS;
  getPath(argv[0],tempS);
  std::stringstream out;
  out<<tempS;
 // out<<"../vector_partition.bmp";
  tempS=out.str();
//  std::fstream tempf;
 // ::CGIspecificRoutines::MakeABitmap(tempS,tempf);
  out<<"../vector_partition.html";

  tempS=out.str();
  std::fstream fileHeaderHtml;
  rootFKFTcomputation::OpenDataFile(fileHeaderHtml,tempS);
  char buffer[1024];
  while (!fileHeaderHtml.eof())
  { fileHeaderHtml.read(buffer,1024);
    std::cout.write(buffer, fileHeaderHtml.gcount());
  }
  WeylGroup tempWeyl;
  tempWeyl.MakeArbitrary('A',3);
  tempWeyl.ComputeRho();
  std::cout<<"<script type=\"text/javascript\">\n";
  std::cout<< "\tvar rootsArray= new Array(" <<tempWeyl.RootsOfBorel.size << ");";
  for (int i=0;i<tempWeyl.RootsOfBorel.size;i++)
  { std::cout<< "\n\trootsArray["<<i<<"]= new Array(3);\n";
    for (int j=0;j<3;j++)
    { tempWeyl.RootsOfBorel.TheObjects[i].TheObjects[j].ElementToString(tempS);
      std::cout<< "\trootsArray["<<i<< "]["<<j <<"]="<<tempS<<";";
    }
  }
	std::cout<<"\n\tgeneratePageFromDimAndNum(3,6,5);\n</script>\n";
  std::cin >> tempS;
	//std::cout<< tempS;
  if(::CGIspecificRoutines::ReadDataFromCGIinput(tempS, theComputationSetup))
  {	theComputationSetup.Run();
		::CGIspecificRoutines::MakeReportFromComputationSetup(theComputationSetup);
  }
	std::cout<<"</BODY>\n</HTML>";
  return 0;   // To avoid Apache errors.
}


