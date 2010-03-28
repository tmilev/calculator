#include <iostream>
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

extern void static_html1( std::stringstream& output);

int main(int argc, char **argv)
{ std::string inputString, inputPath, tempS;
	std::cin >> inputString;
	if (inputString=="")
	{
#ifdef WIN32
    char buffer[2000];
		size_t tempI=1500;
		::getenv_s(&tempI, buffer,1500,"QUERY_STRING");
		inputString=buffer;
#else
		inputString=::getenv("QUERY_STRING");
//		inputString=::getenv("QUERY_STRING");
#endif
	}
	//the below comment is for debug purposes when testing offline please dont delete it!
	//inputString="textDim=3&textNumVectors=9&textCoord0=1&textCoord0=0&textCoord0=0&textCoord1=0&textCoord1=1&textCoord1=0&textCoord2=0&textCoord2=0&textCoord2=1&textCoord3=1&textCoord3=1&textCoord3=0&textCoord4=0&textCoord4=1&textCoord4=2&textCoord5=0&textCoord5=1&textCoord5=1&textCoord6=1&textCoord6=1&textCoord6=2&textCoord7=1&textCoord7=1&textCoord7=1&textCoord8=1&textCoord8=2&textCoord8=2&buttonGo=Go";
	std::cout << "Content-Type: text/html\n\n";
	//std::cout << "inputString: "<<inputString;
	std::cout.flush();
//	for (int i=0;i<argc;i++)
//	{ std::cout<< " argument "<<i<<": "<< argv[i];
//	}
 // std::cout
  //std::cout <<  "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
	//					<< std::endl;
  //std::cout << "<html lang=\"en\" dir=\"LTR\">"<< std::endl;
  //std::stringstream out;
  //inputString="textType=A&textRank=4";
  getPath(argv[0],inputPath);
  ComputationSetup theComputationSetup;
  int choice =::CGIspecificRoutines::ReadDataFromCGIinput
    (inputString, theComputationSetup,inputPath);
  //std::cout<< "choice " <<choice <<"       ";
  if (choice ==0 || choice==1)
  { std::stringstream tempSS;
    static_html1(tempSS);
    std::string tempS;
    tempS=tempSS.str();
    std::cout<<tempS;
    //std::cout<<inputString;
    if (choice==0)//default choice
    {	//std::cout<<"before computation setup";
    //	theComputationSetup.flagComputingPartialFractions=false;
    //	std::cout <<"before Run!";
      theComputationSetup.Run();
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
  } else if (choice==2)
  { if (theComputationSetup.WeylGroupIndex<9)
    { theComputationSetup.DoTheRootSAComputation();
//      theComputationSetup.theRootSubalgebras.ComputeDebugString();
      //inputPath.append("../tmp/WeylHtml.html");
      inputPath.append("../htdocs/tmp/");
      std::string serverPath="/tmp/";
      std::string header;
      std::stringstream tempOut;
      header="http://vector-partition.jacobs-university.de/cgi-bin/vector_partition_linux_cgi?";
      header.append(inputString);
      tempOut <<"Permanent link to this page: <br>\n"<<"<a href=\""
              <<  header<<"\">"<<header<<"</a>\n<br>\n"
              <<"Main page: <br>\n"
              <<"<a href=\"http://vector-partition.jacobs-university.de"
              <<"/cgi-bin/vector_partition_linux_cgi\">"
              <<"http://vector-partition.jacobs-university.de"
              <<"/cgi-bin/vector_partition_linux_cgi</a> <br>\n";
      header=tempOut.str();
      theComputationSetup.theRootSubalgebras.ElementToHtml
        ( header,inputPath,serverPath,*theComputationSetup.theGlobalVariablesContainer->Default());
      std::cout <<"<HTML>"<<"<META http-equiv=\"refresh\" content=\"0; "
                <<"url=../tmp/rootHtml.html\"> <BODY>"<< inputPath;
    }
  }
  std::cout<<"</BODY>\n</HTML>";
	std::cout.flush();
  return 0;   // To avoid Apache errors.
}
