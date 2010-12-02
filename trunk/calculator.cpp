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
  std::cout << "<html><title>Vector partition calculator</title><body>";
  std::string civilizedInput=inputString;
  std::string inputStringRaw=inputString;

  int tempInt=(signed)civilizedInput.size();
  int numNonRelevantStart=10;
  int numNonRelevantEnd=12;
  for (int i=0; i<tempInt; i++)
  { if (i+numNonRelevantStart<tempInt)
      civilizedInput[i]=civilizedInput[i+numNonRelevantStart];
    else
      civilizedInput[i]='|';
  }
  if (tempInt-numNonRelevantStart-numNonRelevantEnd>=0)
    civilizedInput.resize(tempInt-numNonRelevantStart-numNonRelevantEnd);
  if (civilizedInput!="")
    CGIspecificRoutines::CivilizedStringTranslationFromCGI(civilizedInput, civilizedInput);
  Parser theParser;
  GlobalVariables theGlobalVariables;
  theParser.DefaultWeylLetter='B';
  theParser.DefaultWeylRank=3;
  std::string theResult = theParser.ParseEvaluateAndSimplify(civilizedInput, theGlobalVariables);
  theParser.ComputeDebugString(theGlobalVariables);
  std::cout << "<FORM method=\"POST\" name=\"formCalculator\" action=\"/cgi-bin/calculator\">\n <input type=\"textarea\" cols=\"50\" rows=\"30\" name=\"textInput\" value=\"" << civilizedInput << " \">\n<br>\n";
  std::cout << "<input type=\"submit\" name=\"buttonGo\" value=\"Go\"	> ";
  std::cout << "\n</FORM>";
  std::cout << "<br>result: " << theResult;
  std::cout << "<br>raw input string: " << inputStringRaw;
  std::cout << "<br>civilized input string: " << civilizedInput;
  std::cout << "<br>Parser debug string: " <<theParser.DebugString;
  std::cout << "</body></html>";
	return 0;   // To avoid Apache errors.
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
