#include <iostream>

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
#else
		if (path[i]=='/')
    { output.resize(i+1);
      return;
    }
#endif
  }
}

extern void static_html1( std::stringstream& output);

int main(int argc, char **argv)
{ std::string inputString;
	std::cin >> inputString;
	//the below comment is for debug purposes when testing offline please dont delete it!
	//inputString="textDim=3&textNumVectors=9&textCoord0=1&textCoord0=0&textCoord0=0&textCoord1=0&textCoord1=1&textCoord1=0&textCoord2=0&textCoord2=0&textCoord2=1&textCoord3=1&textCoord3=1&textCoord3=0&textCoord4=0&textCoord4=1&textCoord4=2&textCoord5=0&textCoord5=1&textCoord5=1&textCoord6=1&textCoord6=1&textCoord6=2&textCoord7=1&textCoord7=1&textCoord7=1&textCoord8=1&textCoord8=2&textCoord8=2&buttonGo=Go";
	std::cout << "Content-Type: text/html\n\n";
 // std::cout
  //std::cout <<  "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
	//					<< std::endl;
  //std::cout << "<html lang=\"en\" dir=\"LTR\">"<< std::endl;
  //std::stringstream out;
  std::stringstream tempSS;

  static_html1(tempSS);
  std::string tempS;
  tempS=tempSS.str();
  std::cout<<tempS;
  //return 0;
  //tempS= out.str();
  //std::cout<< tempS;

  ComputationSetup theComputationSetup;
  //std::cout<<inputString;
  if(::CGIspecificRoutines::ReadDataFromCGIinput(inputString, theComputationSetup))
  //if (false)
  {	//std::cout<<"before computation setup";
  //	theComputationSetup.flagComputingPartialFractions=false;
	//	std::cout <<"before Run!";
    theComputationSetup.Run();
	  std::cout << "Computation Successfully completed";
		//std::cout <<"Run ok!";
		::CGIspecificRoutines::MakeReportFromComputationSetup(theComputationSetup);
  }
  else
  {	WeylGroup tempWeyl;
		tempWeyl.MakeArbitrary('B',3);
		tempWeyl.ComputeRho();
		theComputationSetup.VPVectors.CopyFromBase(tempWeyl.RootsOfBorel);
		theComputationSetup.theChambers.AmbientDimension=3;
  }
  //std::cout.flush();
  std::cout<<"<script type=\"text/javascript\">\n";
  std::cout<< "\tvar rootsArray= new Array(" <<theComputationSetup.VPVectors.size << ");";
  for (int i=0;i<theComputationSetup.VPVectors.size;i++)
  { std::cout	<< "\n\trootsArray["<<i<<"]= new Array("
							<< theComputationSetup.theChambers.AmbientDimension<<");\n";
		//std::cout.flush();
		for (int j=0;j<theComputationSetup.theChambers.AmbientDimension;j++)
    { //std::cout.flush();
			theComputationSetup.VPVectors.TheObjects[i].TheObjects[j].ElementToString(tempS);
      std::cout<< "\trootsArray["<<i<< "]["<<j <<"]="<<tempS<<";";
    }
//    std::cout.flush();
  }
	std::cout	<<"\n\tgeneratePageFromDimAndNum("
						<<theComputationSetup.theChambers.AmbientDimension<<","
						<<theComputationSetup.VPVectors.size <<","<<-1<<");\n</script>\n";
	//std::cout<<ParallelComputing::GlobalPointerCounter<<tempS1;
  std::cout<<"</BODY>\n</HTML>";
	std::cout.flush();
  return 0;   // To avoid Apache errors.
}


