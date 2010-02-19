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
  { 
#ifdef WIN32  
		if (path[i]=='\\' )
    { output.resize(i+1);
      return;
    }
#elif
		if (path[i]=='/')
    { output.resize(i+1);
      return;
    }
#endif
  }
}



int main(int argc, char **argv)
{ std::string inputString;
	std::cin >> inputString;
	ComputationSetup theComputationSetup;
	std::cout << "Content-Type: text/html\n\n";
  //std::cout <<  "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
	//					<< std::endl;
  //std::cout << "<html lang=\"en\" dir=\"LTR\">"<< std::endl;
  std::string tempS;
  getPath(argv[0],tempS);
  std::stringstream out;
  out<<tempS;
#ifdef WIN32
	tempS="C:\\todor\\math\\rootFKFT\\cpp\\vector_partition.html";
#elif
  out<<"../vector_partition.html";
  tempS1=out.str();
#endif
	std::cout<<tempS<<inputString;
  std::fstream fileHeaderHtml;
  std::cout<<"before the vicious cycle";
  rootFKFTcomputation::OpenDataFile(fileHeaderHtml,tempS);
  char buffer[1024];
  while (!fileHeaderHtml.eof())
  { fileHeaderHtml.read(buffer,1024);
    std::cout.write(buffer, fileHeaderHtml.gcount());
    std::cout<<"inside the vicious cycle";
  }
  if(::CGIspecificRoutines::ReadDataFromCGIinput(inputString, theComputationSetup))
  {	std::cout<<"before computation setup";
		theComputationSetup.Run();
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
    { theComputationSetup.VPVectors.TheObjects[i].TheObjects[j].ElementToString(tempS);
      std::cout<< "\trootsArray["<<i<< "]["<<j <<"]="<<tempS<<";";
    }
  }
	std::cout	<<"\n\tgeneratePageFromDimAndNum("
						<<theComputationSetup.theChambers.AmbientDimension<<","
						<<theComputationSetup.VPVectors.size <<","<<-1<<");\n</script>\n";
	//std::cout<<ParallelComputing::GlobalPointerCounter<<tempS1;
  std::cout<<"</BODY>\n</HTML>";
  return 0;   // To avoid Apache errors.
}


