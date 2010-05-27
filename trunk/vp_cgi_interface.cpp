#include <iostream>
#include "polyhedra.h"

extern int GlobalPointerCounter;
/*
struct bmpfile_magic {
  unsigned char magic[2];
};

struct bmpfile_header {
	unsigned int filesz;
	unsigned short creator1;
	unsigned short creator2;
	unsigned int bmp_offset;
};

struct bmp_dib_v3_header_t
{ unsigned int header_sz;
	unsigned int width;
	unsigned int height;
	unsigned short nplanes;
	unsigned short bitspp;
	unsigned int compress_type;
	unsigned int bmp_bytesz;
	unsigned int hres;
	unsigned int vres;
	unsigned int ncolors;
	unsigned int nimpcolors;
};

void MakeABitmap(std::string& fileName, std::fstream& outputFileOpenWithPreparedHeader)
{	//	format taken from http://en.wikipedia.org/wiki/BMP_file_format , Feb 18, 2010
  outputFileOpenWithPreparedHeader.open(fileName.c_str(),std::fstream::trunc|std::fstream::out| std::fstream::binary);
	outputFileOpenWithPreparedHeader.clear(std::ios::goodbit);// false);
	::bmpfile_magic h1;
	::bmpfile_header h2;
	::bmp_dib_v3_header_t h3;
	int size=10000;
	h1.magic[0]='B'; h1.magic[1]='M';
	h2.bmp_offset=14;
	h2.creator1=0;
	h2.creator2=0;
	h2.filesz=size*3+54;
	h3.width=100;
	h3.height=100;
	h3.header_sz=40;
	h3.nplanes=1;
	h3.bitspp=24;
	h3.bmp_bytesz=size*3;
	h3.compress_type=0;
	h3.hres=2835;
	h3.vres=2835;
	h3.nimpcolors=0;
	h3.ncolors=0;
	outputFileOpenWithPreparedHeader.write((char*)(&h1),2);
	outputFileOpenWithPreparedHeader.write((char*)(&h2),12);
	outputFileOpenWithPreparedHeader.write((char*)(&h3),40);
	for (int i=0;i<size;i++)
	{	char tempUI=100;
		outputFileOpenWithPreparedHeader.write(&tempUI,1);
		outputFileOpenWithPreparedHeader.write(&tempUI,1);
		outputFileOpenWithPreparedHeader.write(&tempUI,1);
	}
	outputFileOpenWithPreparedHeader.close();
}
*/

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
//  inputString="textType = g textRank = 2 buttonGoRootSA = rootSA+diagrams";
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
  if (choice ==0 || choice==1)
  { std::stringstream tempSS;
    static_html1(tempSS);
    std::string tempS;
    tempS=tempSS.str();
    std::cout<<tempS;
    if (choice==0)//default choice
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
  } else if (choice==2)
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
      ::system(MkDirCommand.c_str());
      std::string header;
      std::stringstream tempOut;
      header="http://vector-partition.jacobs-university.de/cgi-bin/vector_partition_linux_cgi?";
      header.append(inputString);
      tempOut <<"Permanent link to this page: <br>\n"<<"<a href=\"" << header<<"\">"<<header<<"</a>\n<br>\n" <<"Main page: <br>\n" <<"<a href=\"http://vector-partition.jacobs-university.de"
                      <<"/cgi-bin/vector_partition_linux_cgi\">" <<"http://vector-partition.jacobs-university.de" <<"/cgi-bin/vector_partition_linux_cgi</a> <br>\n";
      header=tempOut.str();
      theComputationSetup.theRootSubalgebras.ElementToHtml( header,PhysicalPath,serverPath,*theComputationSetup.theGlobalVariablesContainer->Default());
      std::cout << "<HTML>"<<"<META http-equiv=\"refresh\" content=\"0; " << "url="<<serverPath<< "rootHtml.html\"> <BODY>"
                      << serverPath <<"<br> input string: <br>\n"<< inputString << "<br>"<<PhysicalPath;
    }
  } else if (choice==3)
  { std::cout << "<FORM method=\"POST\" name=\"formRootSAs\" action=\"/cgi-bin/vector_partition_linux_cgi\">\n" <<" Type(A,B,C,D,E,F,G): <input type=\"text\" size =\"1\" name=\"textType\" value=\"E\">\n"
              << "Dimension(<=8): <input type=\"text\" size=\"1\" name=\"textRank\" value=\"6\">\n"<< "<input type=\"submit\" name=\"buttonGoRootSA\" value=\"rootSA diagrams\"	>\n"<< "</FORM>\n";
  } else if (choice==4)
  { std::cout <<"Choice 4 taken!";
		SltwoSubalgebras tempSAs;
		tempSAs.Compute(*theComputationSetup.theGlobalVariablesContainer->Default(),true);
		tempSAs.ComputeDebugString( *theComputationSetup.theGlobalVariablesContainer->Default(), tempSAs.theWeylGroup, false, false);
		std::cout <<"\n<br>\n"<<tempSAs.DebugString;
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
