#include <exception>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include "polyhedra.h"

extern int GlobalPointerCounter;

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
  std::string tempS2, tempS1;
  getPath(argv[0],tempS1);
  std::stringstream out;
  out<<tempS1;
  out<<"../vector_partition.html";

  tempS1=out.str();
  std::fstream fileHeaderHtml;
  rootFKFTcomputation::OpenDataFile(fileHeaderHtml,tempS1);
  char buffer[1024];
  while (!fileHeaderHtml.eof())
  { fileHeaderHtml.read(buffer,1024);
    std::cout.write(buffer, fileHeaderHtml.gcount());
  }
  std::cin >> tempS1;
  if(::CGIspecificRoutines::ReadDataFromCGIinput(tempS1, theComputationSetup))
  {	theComputationSetup.Run();
		::CGIspecificRoutines::MakeReportFromComputationSetup(theComputationSetup);
  }
  else
  {	WeylGroup tempWeyl;
		tempWeyl.MakeArbitrary('B',3);
		tempWeyl.ComputeRho();
		theComputationSetup.VPVectors.CopyFromBase(tempWeyl.RootsOfBorel);
		theComputationSetup.theChambers.AmbientDimension=3;
	//	std::cout<<"asdfadsfasdfasdf";
  }
  std::cout<<"<script type=\"text/javascript\">\n";
  std::cout<< "\tvar rootsArray= new Array(" <<theComputationSetup.VPVectors.size << ");";
  for (int i=0;i<theComputationSetup.VPVectors.size;i++)
  { std::cout	<< "\n\trootsArray["<<i<<"]= new Array("
							<< theComputationSetup.theChambers.AmbientDimension<<");\n";
    for (int j=0;j<theComputationSetup.theChambers.AmbientDimension;j++)
    { theComputationSetup.VPVectors.TheObjects[i].TheObjects[j].ElementToString(tempS2);
      std::cout<< "\trootsArray["<<i<< "]["<<j <<"]="<<tempS2<<";";
    }
  }
	std::cout	<<"\n\tgeneratePageFromDimAndNum("
						<<theComputationSetup.theChambers.AmbientDimension<<","
						<<theComputationSetup.VPVectors.size <<","<<-1<<");\n</script>\n";
	//std::cout<<ParallelComputing::GlobalPointerCounter<<tempS1;
  std::cout<<"</BODY>\n</HTML>";
  return 0;   // To avoid Apache errors.
}


