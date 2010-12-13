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

extern void static_html1( std::stringstream& output);

int main(int argc, char **argv)
{ std::string inputString, inputPath, tempS;
	std::cin >> inputString;
	if (inputString=="")
	{
#ifdef WIN32
    char buffer[2000];
		size_t tempI=1500;
		::getenv_s(&tempI, buffer, 1500, "QUERY_STRING");
		inputString=buffer;
#else
		inputString=getenv("QUERY_STRING");
//		inputString=::getenv("QUERY_STRING");
#endif
	}
	getPath(argv[0], inputPath);
//	inputString="textInput=+asf&buttonGo=Go";
//  inputString="weylLetterInput=B&weyRankInput=3&textInput=%2B&buttonGo=Go";

	std::cout << "Content-Type: text/html\n\n";
  std::cout << "<html><head><title>Vector partition calculator</title>";
  std::cout << "<script src=\"/easy/load.js\"></script> ";
  std::cout << "\n</head>\n<body>\n";
//  std::cout << inputString;
  List<std::string> inputStrings;
  CGIspecificRoutines::ChopCGIInputStringToMultipleStrings(inputString, inputStrings);
  inputStrings.SetSizeExpandOnTopNoObjectInit(3);
  std::string& civilizedInput= inputStrings.TheObjects[2];
  std::string& inputRankString = inputStrings.TheObjects[1];
  std::string& inputWeylString = inputStrings.TheObjects[0];
  CGIspecificRoutines::CivilizedStringTranslationFromCGI(civilizedInput, civilizedInput);
  Parser theParser;
  GlobalVariables theGlobalVariables;
  if (inputWeylString!="")
    theParser.DefaultWeylLetter=inputWeylString[0];
  else
    theParser.DefaultWeylLetter='A';
  if (inputRankString!="")
  { std::stringstream tempStream;
    tempStream << inputRankString;
    tempStream.seekg(0);
    tempStream >> theParser.DefaultWeylRank;
  } else
    theParser.DefaultWeylRank=1;
  CGIspecificRoutines::MakeSureWeylGroupIsSane(theParser.DefaultWeylLetter, theParser.DefaultWeylRank);
  //For debugging:
  ParallelComputing::cgiLimitRAMuseNumPointersInList=30000000;
//  civilizedInput="[ig_2,g_{-3}^{n_3}g_{-5}^{n_2}g_{-7}^{n_1}]";
//  theParser.DefaultWeylRank=3;
//  theParser.DefaultWeylLetter='B';
  if (theParser.DefaultWeylLetter=='B' && theParser.DefaultWeylRank==3)
  { theParser.theHmm.MakeG2InB3(theParser, theGlobalVariables);
    //theParser.theHmm.RestrictedRootSystem.ElementToString(tempS, false, true, true);
    //std::cout << tempS;
  }
  std::string theResult = theParser.ParseEvaluateAndSimplify(civilizedInput, theGlobalVariables);
  theParser.ComputeDebugString(theGlobalVariables);

  std::string beginMath="<DIV class=\"math\" scale=\"50\">";
  std::string endMath ="</DIV>";
  std::cout << "<table>\n <tr valign=\"top\">\n <td></td><td></td><td>";
  std::cout << " <img src=\"../karlin.gif\" width=\"46\" height=\"48\"></img>&nbsp<img src=\"../jacobs_logo.png\" width=\"128\" height=\"44\"></img><br>";
  std::cout << "</td><tr valign=\"top\">\n<td>";
  std::cout << "\n<FORM method=\"POST\" name=\"formCalculator\" action=\"/cgi-bin/calculator\">\n" ;
  std::cout << "<input type=\"text\" size =\"1\" name=\"weylLetterInput\" value=\"" << theParser.DefaultWeylLetter << "\"></input>";
  std::cout << "<input type=\"text\" size =\"1\" name=\"weylRankInput\" value=\"" << theParser.DefaultWeylRank << "\"></input>\n<br>\n";
  std::cout << "<textarea rows=\"3\" cols=\"30\" name=\"textInput\" onkeypress=\"if (event.keyCode == 13) {this.form.submit(); return false;}\">" << civilizedInput  << "</textarea>\n<br>\n";
//  std::cout << "<input type=\"textarea\" size=\"20\" name=\"textInput\" value=\"" << civilizedInput << "\"></input>\n<br>\n";
  std::cout << "<input type=\"submit\" name=\"buttonGo\" value=\"Go\"	> ";
  std::cout << "\n</FORM>";
//  \n<FORM method=\"POST\" name=\"formCalculator\" action=\"/cgi-bin/calculator\">\n <input type=\"textarea\" rows=\"60\" cols=\"60\" name=\"textInput\" \"></textarea>\n<br>\n";
  std::cout << "<br>result: " << theResult;
  std::cout << "</td>";
  std::cout << " <td></td>\n<td>\n";
  std::stringstream tempStream;
  inputPath.append("../htdocs/tmp/");
  std::stringstream tempStream2;
  tempStream2 << theParser.DefaultWeylLetter << theParser.DefaultWeylRank << "/";
  inputPath.append(tempStream2.str());
  tempStream << theParser.DefaultWeylLetter << theParser.DefaultWeylRank << "table";
  std::string fileNameLieBracketNoEnding=tempStream.str();
  std::string fileNameLieBracketFullPathNoEnding=inputPath;
  fileNameLieBracketFullPathNoEnding.append(fileNameLieBracketNoEnding);
  std::cout << "<img src=\"/tmp/" << tempStream2.str() << fileNameLieBracketNoEnding << ".png\"></img>";
  List<std::string> LatexCommands;
  std::string latexCommandTemp;
  std::string fileNameLieBracketFullPathPNGEnding;
  fileNameLieBracketFullPathPNGEnding=fileNameLieBracketFullPathNoEnding;
  fileNameLieBracketFullPathPNGEnding.append(".png");
  LatexCommands.size=0;
  if (!CGIspecificRoutines::FileExists(fileNameLieBracketFullPathPNGEnding))
  { std::cout << "<br>the file: " << fileNameLieBracketFullPathPNGEnding << " does not exist<br>";
    std::fstream lieBracketFile;
    std::string tempFileName= fileNameLieBracketFullPathNoEnding;
    tempFileName.append(".tex");
    std::stringstream tempCommand;
    tempCommand << "mkdir " << inputPath;
    tempS=tempCommand.str();
    system(tempS.c_str());
    CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(lieBracketFile, tempFileName, false, true, false);
    theParser.theHmm.theRange.ElementToStringLieBracket(tempS, false, true, true, theGlobalVariables);
    lieBracketFile << "\\documentclass{article}\\begin{document}\\pagestyle{empty}\n" << tempS << "\n\\end{document}";
    lieBracketFile.close();
    std::stringstream tempStreamLatex, tempStreamPNG;
    tempStreamLatex << "latex  -output-directory=" << inputPath << " " << inputPath << fileNameLieBracketNoEnding << ".tex";
    latexCommandTemp=tempStreamLatex.str();
    LatexCommands.AddObjectOnTop(latexCommandTemp);
    tempStreamPNG << "dvipng " << fileNameLieBracketFullPathNoEnding << ".dvi -o " << fileNameLieBracketFullPathNoEnding << ".png -T tight";
    latexCommandTemp= tempStreamPNG.str();
    LatexCommands.AddObjectOnTop(latexCommandTemp);
  } else
    std::cout << "<br><br><br>the file: " << fileNameLieBracketFullPathPNGEnding << " exists<br>";
  std::cout << "<br><br><br><br><br>Debugging printouts follow.<br>Number of pointers used:" << ParallelComputing::GlobalPointerCounter << "<br>raw input string: " << inputString;
  std::cout << "<br>civilized input strings: " << civilizedInput << "<br> chopped strings: <br>";
  for (int i=0; i<inputStrings.size; i++)
    std::cout << "string " << i << ": " << inputStrings.TheObjects[i] << "<br>";
  std::cout << "system commands: <br>";
  for (int i=0; i<LatexCommands.size; i++)
    std::cout << LatexCommands.TheObjects[i] << "<br>";
  std::cout << "<br>Parser debug string:<br> " << theParser.DebugString;
//  theParser.theLieAlgebra.ComputeDebugString();
//  std::cout << "<br>details:<br> " << theParser.theLieAlgebra.ElementToStringLieBracketPairing();
  std::cout << "\n</td></table>";
  std::cout << "</body></html>";
  std::string command1, command2;
  std::cout << "<!--";
  std::cout.flush();
  for (int i=0; i<LatexCommands.size; i++)
    system(LatexCommands.TheObjects[i].c_str());
  std::cout << "-->";
	return 0;   // Toavoid Apache errors.
}

void CGIspecificRoutines::CivilizedStringTranslationFromCGI(std::string& input, std::string& output)
{ std::string readAhead;
  std::stringstream out;
  int inputSize=(signed) input.size();
  for (int i=0; i<inputSize; i++)
  { readAhead="";
    for (int j=0; j<3; j++)
    { if (i+j<inputSize)
        readAhead.push_back(input[i+j]);
      if (CGIspecificRoutines::AttemptToCivilize(readAhead, out))
      { i+=j;
        break;
      }
    }
  }
  output=out.str();
}

void CGIspecificRoutines::ChopCGIInputStringToMultipleStrings(const std::string& input, List<std::string>& output)
{ int inputLength= (signed) input.size();
  bool reading=false;
  output.SetSizeExpandOnTopNoObjectInit(1);
  for (int i=0; i<inputLength; i++)
  { if (input[i]=='=')
      reading=!reading;
    if (input[i]=='&')
    { output.SetSizeExpandOnTopNoObjectInit(output.size+1);
      output.LastObject()->reserve(1000);
      *output.LastObject()="";
      reading=false;
    }
    if (input[i]!='=' && input[i]!='&' && reading)
      output.LastObject()->push_back(input[i]);
  }
}

bool CGIspecificRoutines::AttemptToCivilize(std::string& readAhead, std::stringstream& out)
{ if (readAhead[0]!='%' && readAhead[0]!='&' && readAhead[0]!='+')
  { out << readAhead[0];
    return true;
  }
  if (readAhead=="&")
  { out << " ";
    return true;
  }
  if (readAhead=="+")
  { out << " ";
    return true;
  }
  if (readAhead=="%2B")
  { out << "+";
    return true;
  }
  if (readAhead=="%28")
  { out << "(";
    return true;
  }
  if (readAhead=="%29")
  { out << ")";
    return true;
  }
  if (readAhead=="%5B")
  { out << "[";
    return true;
  }
  if (readAhead=="%5D")
  { out << "]";
    return true;
  }
  if (readAhead=="%2C")
  { out << ",";
    return true;
  }
  if (readAhead=="%7B")
  { out << "{";
    return true;
  }
  if (readAhead=="%2F")
  { out << "/";
    return true;
  }
  if (readAhead=="%5E")
  { out << "^";
    return true;
  }
  if (readAhead=="%7D")
  { out << "}";
    return true;
  }
  return false;
}
