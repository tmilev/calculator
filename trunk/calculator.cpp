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
//	inputString="textInput=+asf&buttonGo=Go";

	std::cout << "Content-Type: text/html\n\n";
  std::cout << "<html><head><title>Vector partition calculator</title>";
  std::cout << "<script src=\"../htdocs/easy/load.js\"></script></head> ";
  std::cout << "<body>";

  std::string civilizedInput=inputString;
  std::string inputStringRaw=inputString;

  int tempInt=(signed)civilizedInput.size();
  int numNonRelevantStart=10;
  int civilizedLength=0;
  for (; civilizedLength<tempInt; civilizedLength++)
    if (civilizedLength+numNonRelevantStart<tempInt)
    { if (civilizedInput[civilizedLength+numNonRelevantStart]!='&')
        civilizedInput[civilizedLength]=civilizedInput[civilizedLength+numNonRelevantStart];
      else
        break;
    }
  civilizedInput.resize(civilizedLength);
  if (civilizedInput!="")
    CGIspecificRoutines::CivilizedStringTranslationFromCGI(civilizedInput, civilizedInput);
  Parser theParser;
  GlobalVariables theGlobalVariables;
  theParser.DefaultWeylLetter='B';
  theParser.DefaultWeylRank=3;
  //For debugging:
//  civilizedInput= "[g_4h_2, 6h_1]";
//  civilizedInput= "[5g_2,6h_1g_2+5]";
  ParallelComputing::cgiLimitRAMuseNumPointersInList=30000000;

  std::string theResult = theParser.ParseEvaluateAndSimplify(civilizedInput, theGlobalVariables);
  theParser.ComputeDebugString(theGlobalVariables);
  std::string beginMath="<DIV class=\"math\" scale=\"50\">";
  std::string endMath ="</DIV>";
  std::cout << "<table>\n <tr valign=\"top\">\n <td>";
  std::cout << "\n<FORM method=\"POST\" name=\"formCalculator\" action=\"/cgi-bin/calculator\">\n";
  std::cout << "<input type=\"textarea\" rows=\"60\" cols=\"60\" name=\"textInput\" value=\"" << civilizedInput << "\"></input>\n<br>\n";
  std::cout << "<input type=\"submit\" name=\"buttonGo\" value=\"Go\"	> ";
  std::cout << "\n</FORM>";
//  \n<FORM method=\"POST\" name=\"formCalculator\" action=\"/cgi-bin/calculator\">\n <input type=\"textarea\" rows=\"60\" cols=\"60\" name=\"textInput\" \"></textarea>\n<br>\n";
  std::cout << "<br>result: " << theResult;
  std::cout <<"</td>";
  std::cout << " <td></td>\n<td>\n";
  theParser.theLieAlgebra.ElementToStringLieBracket(tempS, true, true, theGlobalVariables);
  std::cout << tempS;

  std::cout << "<br><br><br><br><br>Debugging printouts follow.<br>Number of pointers used:" << ParallelComputing::GlobalPointerCounter << "<br>raw input string: " << inputStringRaw;
  std::cout << "<br>civilized input string: " << civilizedInput;
  std::cout << "<br>Parser debug string:<br> " << theParser.DebugString;
  std::cout << "\n</td></table>";
  std::cout << "</body></html>";
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

void SemisimpleLieAlgebra::ElementToStringLieBracket(std::string& output, bool useHtml, bool useLatex, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  std::string tempS;
  root tempRoot;
  std::string beginMath="<DIV class=\"math\" scale=\"50\">";
  std::string endMath ="</DIV>";
  int numRoots=this->theWeyl.RootSystem.size;
  int theDimension = this->theWeyl.KillingFormMatrix.NumRows;
//  out << beginMath << "\\begin{array}{ccc}a& a&a\\\\a&a&a\\end{array}";

  out << beginMath << "\\begin{array}{";
  ElementUniversalEnveloping tempElt1, tempElt2, tempElt3;
  for (int i=0; i<numRoots+theDimension+1; i++)
    out << "c";
  out << "}[\\bullet, \\bullet]\n";
  for (int i=0; i<this->theLiebracketPairingCoefficients.NumRows; i++)
  { tempElt1.MakeOneGeneratorCoeffOne(i, *this);
    tempElt1.ElementToString(tempS);
    out << " & " << tempS;
  }
  out << "\\\\\n";
  Rational tempRat;
  for (int i=0; i<this->theLiebracketPairingCoefficients.NumRows; i++)
  { tempElt1.MakeOneGeneratorCoeffOne(i, *this);
    tempElt1.ElementToString(tempS);
    out << tempS;
    for (int j=0; j<this->theLiebracketPairingCoefficients.NumRows; j++)
    { tempElt2.MakeOneGeneratorCoeffOne(j, *this);
      tempElt1.LieBracketOnTheRight(tempElt2, tempElt3);
      tempElt3.Simplify();
      tempElt3.ElementToString(tempS);
      out << "& " << tempS;
    }
    out << "\\\\\n";
  }
  out << "\\end{array}" << endMath;
  output=out.str();
}

void CGIspecificRoutines::ReplaceEqualitiesAndAmpersantsBySpaces(std::string& inputOutput)
{ for (int i=0; i<(signed)inputOutput.size(); i++)
    if (inputOutput[i]=='=' || inputOutput[i]=='&')
      inputOutput[i]=' ';
}
