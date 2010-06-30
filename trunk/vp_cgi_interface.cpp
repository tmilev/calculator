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
	//inputString="textDim=4&textNumVectors=10&textCoord0=1&textCoord0=0&textCoord0=0&textCoord0=0&textCoord1=0&textCoord1=1&textCoord1=0&textCoord1=0&textCoord2=0&textCoord2=0&textCoord2=1&textCoord2=0&textCoord3=0&textCoord3=0&textCoord3=0&textCoord3=1&textCoord4=1&textCoord4=1&textCoord4=0&textCoord4=0&textCoord5=0&textCoord5=1&textCoord5=1&textCoord5=0&textCoord6=0&textCoord6=0&textCoord6=1&textCoord6=1&textCoord7=1&textCoord7=1&textCoord7=1&textCoord7=0&textCoord8=0&textCoord8=1&textCoord8=1&textCoord8=1&textCoord9=1&textCoord9=1&textCoord9=1&textCoord9=1&buttonGo=Default_computation&chamberNumber=-1";
	//inputString="textDim=4&textNumVectors=10&textCoord0=1&textCoord0=0&textCoord0=0&textCoord0=0&textCoord1=0&textCoord1=1&textCoord1=0&textCoord1=0&textCoord2=0&textCoord2=0&textCoord2=1&textCoord2=0&textCoord3=0&textCoord3=0&textCoord3=0&textCoord3=1&textCoord4=1&textCoord4=1&textCoord4=0&textCoord4=0&textCoord5=0&textCoord5=1&textCoord5=1&textCoord5=0&textCoord6=0&textCoord6=0&textCoord6=1&textCoord6=1&textCoord7=1&textCoord7=1&textCoord7=1&textCoord7=0&textCoord8=0&textCoord8=1&textCoord8=1&textCoord8=1&textCoord9=1&textCoord9=1&textCoord9=1&textCoord9=1&buttonSplitChambers=Chambers%2Bpartial_fractions&chamberNumber=-1";
	//inputString="textDim=4&textNumVectors=10&textCoord0=1&textCoord0=0&textCoord0=0&textCoord0=0&textCoord1=0&textCoord1=1&textCoord1=0&textCoord1=0&textCoord2=0&textCoord2=0&textCoord2=1&textCoord2=0&textCoord3=0&textCoord3=0&textCoord3=0&textCoord3=1&textCoord4=1&textCoord4=1&textCoord4=0&textCoord4=0&textCoord5=0&textCoord5=1&textCoord5=1&textCoord5=0&textCoord6=0&textCoord6=0&textCoord6=1&textCoord6=1&textCoord7=1&textCoord7=1&textCoord7=1&textCoord7=0&textCoord8=0&textCoord8=1&textCoord8=1&textCoord8=1&textCoord9=1&textCoord9=1&textCoord9=1&textCoord9=1&buttonOneChamber=Vector_partition_function_chamber_%23&chamberNumber=12";
	//inputString ="textType=E&textRank=6&buttonGoRootSA=rootSA+diagrams";
	//inputString ="SLtwos";
 //inputString="textType = g textRank = 2 buttonGoRootSA = rootSA+diagrams";
  //inputString="textType=D&textRank=4&buttonGoSl2SAs=sl%282%29+subalgebras";
	std::cout << "Content-Type: text/html\n\n";
	//std::cout << "inputString: "<<inputString;
	std::cout.flush();
  //inputString="textType=A&textRank=4";
  getPath(argv[0],inputPath);
  ComputationSetup theComputationSetup;
  theComputationSetup.flagDoCustomComputation=false;
  int choice =::CGIspecificRoutines::ReadDataFromCGIinput(inputString, theComputationSetup,inputPath);
  //std::cout<< "choice " <<choice <<"       ";
  std::string latexCommand1;
  std::string latexCommand2;    //std::cout<<inputString;
  inputPath.append("../htdocs/tmp/");
  if (choice ==CGIspecificRoutines::choiceDefaultNeedComputation || choice==CGIspecificRoutines::choiceInitAndDisplayMainPage)
  { std::stringstream tempSS;
    static_html1(tempSS);
    std::string tempS;
    tempS=tempSS.str();
    std::cout<<tempS;
    if (choice==CGIspecificRoutines::choiceDefaultNeedComputation)//default choice
    {	//std::cout<<"before computation setup";
    //	theComputationSetup.flagComputingPartialFractions=false;
    //	std::cout <<"before Run!";
      theComputationSetup.flagPartialFractionSplitPrecomputed=false;
      theComputationSetup.Run();
      ::CGIspecificRoutines::MakePFAndChamberReportFromComputationSetup(theComputationSetup);
      std::string pfFileName;
      std::fstream pfFile;
      pfFileName=inputPath;
      pfFileName.append("partial_fraction.tex");
      CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(pfFile, pfFileName, false, true, false);
      pfFile<< theComputationSetup.thePartialFraction.DebugString;
      pfFile.close();
      latexCommand1.append("pdflatex -output-directory=");
      latexCommand1.append(inputPath);
      latexCommand1.append(" ");
      latexCommand1.append(pfFileName);
      //std::cout<<"latex command:";
      //std::cout.flush();
      //std::cout<<"time for second report";
      if (theComputationSetup.flagComputingVectorPartitions)
			{ CGIspecificRoutines::MakeVPReportFromComputationSetup(theComputationSetup);
        std::string vpFileName;
        std::fstream vpFile;
        vpFileName=inputPath;
        vpFileName.append("vector_partition.tex");
        CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(vpFile, vpFileName, false, true, false);
        vpFile<< theComputationSetup.theOutput.DebugString;
        vpFile.close();
        latexCommand2.clear();
        latexCommand2.append("pdflatex -output-directory=");
        latexCommand2.append(inputPath);
        latexCommand2.append(" ");
        latexCommand2.append(vpFileName);
      }
      std::string chamberFileName;
      std::fstream chamberFile;
      chamberFileName=inputPath;
      chamberFileName.append("chambers.html");
      CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(chamberFile, chamberFileName, false, true, false);
      chamberFile<<"<HTML><BODY>"<< theComputationSetup.theChambers.DebugString<<"</BODY></HTML>";
      chamberFile.close();
      if (  theComputationSetup.DisplayNumberChamberOfInterest==-1 && theComputationSetup.flagComputingVectorPartitions)
        theComputationSetup.DisplayNumberChamberOfInterest= theComputationSetup.theChambers.TheObjects[theComputationSetup.theChambers.RootBelongsToChamberIndex(theComputationSetup.IndicatorRoot,0)]->DisplayNumber;
        //std::cout <<"Run ok!";
    }
    else
    {	WeylGroup tempWeyl;
      theComputationSetup.WeylGroupIndex=4;
      theComputationSetup.theChambers.AmbientDimension=4;
      tempWeyl.MakeArbitrary('A',theComputationSetup.theChambers.AmbientDimension);
      tempWeyl.ComputeRho(true);
      theComputationSetup.VPVectors.CopyFromBase(tempWeyl.RootsOfBorel);
    }
    //std::cout.flush();
    std::cout<<"<script type=\"text/javascript\">\n";
    std::cout<<"\tvar rootsArraySize=" <<theComputationSetup.VPVectors.size<<";\n";
    std::cout<<"\tvar rootsArrayDim=" <<theComputationSetup.theChambers.AmbientDimension<<";\n";
    std::cout<< "\tvar rootsArray= new Array(" <<theComputationSetup.VPVectors.size << ");";
    for (int i=0;i<theComputationSetup.VPVectors.size;i++)
    { std::cout	<< "\n\trootsArray["<<i<<"]= new Array(" << theComputationSetup.theChambers.AmbientDimension<<");\n";
      //std::cout.flush();
      for (int j=0;j<theComputationSetup.theChambers.AmbientDimension;j++)
      { //std::cout.flush();
        theComputationSetup.VPVectors.TheObjects[i].TheObjects[j].ElementToString(tempS);
        std::cout<< "\trootsArray["<<i<< "]["<<j <<"]="<<tempS<<";";
      }
  //    std::cout.flush();
    }
    std::cout	<< "\n\tgeneratePageFromDimAndNum(" << theComputationSetup.theChambers.AmbientDimension<<","<< theComputationSetup.VPVectors.size <<","<<-1<<","
                    << theComputationSetup.DisplayNumberChamberOfInterest<<");\n</script>\n";
    //std::cout<<ParallelComputing::GlobalPointerCounter<<tempS1;
  } else if (choice==CGIspecificRoutines::choiceGenerateDynkinTables)
  { if (theComputationSetup.WeylGroupIndex<9)
    { theComputationSetup.DoTheRootSAComputation();
//      theComputationSetup.theRootSubalgebras.ComputeDebugString();
      //inputPath.append("../tmp/WeylHtml.html");
      std::stringstream outServerPath;
      outServerPath<<"/tmp/"<< theComputationSetup.WeylGroupLetter<<theComputationSetup.WeylGroupIndex <<"/";
      std::string serverPath=outServerPath.str();
      std::stringstream outPhysicalPath;
      std::string PhysicalPath;
      outPhysicalPath << inputPath<<theComputationSetup.WeylGroupLetter<< theComputationSetup.WeylGroupIndex <<"/";
      PhysicalPath=outPhysicalPath.str();
      std::string MkDirCommand;
      std::stringstream outMkDirCommand;
      outMkDirCommand<< "mkdir " <<PhysicalPath;
      MkDirCommand=outMkDirCommand.str();
      system(MkDirCommand.c_str());
      std::string header;
      std::stringstream tempOut;
      header="http://vector-partition.jacobs-university.de/cgi-bin/vector_partition_linux_cgi?";
      header.append(inputString);
      tempOut <<"Permanent link to this page: <br>\n<a href=\"" << header<<"\">"<<header<<"</a>\n<br>\nMain page: <br>\n<a href=\"http://vector-partition.jacobs-university.de/cgi-bin/vector_partition_linux_cgi\"> http://vector-partition.jacobs-university.de/cgi-bin/vector_partition_linux_cgi</a> <br>\n";
      header=tempOut.str();
      theComputationSetup.theRootSubalgebras.ElementToHtml(header, PhysicalPath, serverPath, *theComputationSetup.theGlobalVariablesContainer->Default());
      std::cout << "<HTML>"<<"<META http-equiv=\"refresh\" content=\"0; url="<<serverPath<< "rootHtml.html\"> <BODY>"<< serverPath <<"<br> input string: <br>\n"<< inputString << "<br>"<<PhysicalPath;
    }
  } else if (choice==CGIspecificRoutines::choiceDisplayRootSApage)
  { std::cout << "<FORM method=\"POST\" name=\"formRootSAs\" action=\"/cgi-bin/vector_partition_linux_cgi\">\n Type(A,B,C,D,E,F,G): <input type=\"text\" size =\"1\" name=\"textType\" value=\"E\">\nDimension(<=8): <input type=\"text\" size=\"1\" name=\"textRank\" value=\"6\">\n<br>\n<input type=\"submit\" name=\"buttonGoRootSA\" value=\"rootSA diagrams\"	>\n<br><input type=\"submit\" name=\"buttonGoSl2SAs\" value=\"sl(2) subalgebras\"	>\n</FORM>\n";
  } else if (choice==CGIspecificRoutines::choiceGosl2)
  { SltwoSubalgebras theSl2s;
    SimpleLieAlgebra theSimpleLieAlgebra;
//    std::cout<<"<br>"<< theComputationSetup.WeylGroupLetter<< theComputationSetup.WeylGroupIndex;
    std::cout.flush();
    theSimpleLieAlgebra.FindSl2Subalgebras(theSl2s, theComputationSetup.WeylGroupLetter, theComputationSetup.WeylGroupIndex, *theComputationSetup.theGlobalVariablesContainer->Default());
    ////////////////////getting paths to output html
    std::stringstream outServerPath;
    outServerPath<<"/tmp/"<< theComputationSetup.WeylGroupLetter<<theComputationSetup.WeylGroupIndex <<"/sl2s/";
    std::string serverPath=outServerPath.str();
    std::stringstream outPhysicalPath;
    std::string PhysicalPath;
    outPhysicalPath << inputPath<<theComputationSetup.WeylGroupLetter<< theComputationSetup.WeylGroupIndex <<"/sl2s/";
    PhysicalPath=outPhysicalPath.str();
    std::string MkDirCommand;
    std::stringstream outMkDirCommand;
    outMkDirCommand<< "mkdir " <<PhysicalPath;
    MkDirCommand=outMkDirCommand.str();
    system(MkDirCommand.c_str());
    theSl2s.ElementToHtml(*theComputationSetup.theGlobalVariablesContainer->Default(), theSimpleLieAlgebra.theWeyl, true, PhysicalPath, serverPath);
    theSl2s.ComputeDebugString(*theComputationSetup.theGlobalVariablesContainer->Default(), theSimpleLieAlgebra.theWeyl, false, true);
    std::cout<<theSl2s.DebugString;
  }
  std::cout<<"</BODY>\n</HTML>";
	std::cout<<"<!--";
	std::cout.flush();
#ifndef WIN32
  ::system(latexCommand1.c_str());
  ::system(latexCommand2.c_str());
#endif
	std::cout<<"-->";
	std::cout.flush();
	return 0;   // To avoid Apache errors.
}
