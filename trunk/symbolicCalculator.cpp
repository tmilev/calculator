#include <iostream>
#include <vector>
#include <map>
#include <sstream>

class Computation;
class Expression;

const int SomeRandomPrimesSize= 25;
//used for hashing various things.
const int SomeRandomPrimes[SomeRandomPrimesSize]={ 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911};

int hashString(const std::string& x)
{ int numCycles=x.size();
  if (numCycles>SomeRandomPrimesSize)
    numCycles=SomeRandomPrimesSize;
  int result=0;
  for (int i=0; i<numCycles; i++)
    result+=x[i]*SomeRandomPrimes[i];
  return result;
}

template <class element, int hashFunction(const element&)>
class hashedVector
{ std::vector<element> theElts;
  std::vector<std::vector<int> > hashedIndices;
  public:
  const element& operator[](int i)const {return this->theElts[i];}
  int GetHash(const element& x)
  { int result=hashFunction(x)% this->hashedIndices.size();
    if (result<0)
      result+=this->hashedIndices.size();
    return result;
  }
  int size()const{return this->theElts.size();}
  void reset()
  { if (this->theElts.size()<this->hashedIndices.size())
      for (unsigned i=0; i<this->theElts.size(); i++)
        this->hashedIndices[this->GetIndex(this->theElts[i])].resize(0);
    else
      for (unsigned i=0; i<this->hashedIndices.size(); i++)
        this->hashedIndices[i].resize(0);
    this->theElts.resize(0);
  }
  void resetSetHashSize(int N)
  { this->reset();
    if (N<1)
      N=1;
    this->hashedIndices.resize(N);
  }
  int GetIndex(const element& x)
  { int theHash=this->GetHash(x);
    std::vector<int>& currentHash=this->hashedIndices[theHash];
    for (unsigned i=0; i<currentHash.size(); i++)
      if (x==this->theElts[currentHash[i]])
        return i;
    return -1;
  }
  void reserve(int N){this->theElts.reserve(N);}
  bool Contains(const element & x)
  { return this->GetIndex(x)!=-1;
  }
  hashedVector()
  { this->resetSetHashSize(1000);
  }
  inline void push_back(const element& x)
  { this->AddOnTop(x);
  }
  bool AddNoRepetition(const element& x)
  { if (!this->Contains(x))
    { this->AddOnTop(x);
      return true;
    }
    return false;
  }
  void AddOnTop(const element& x)
  { this->theElts.push_back(x);
    this->hashedIndices[this->GetHash(x)].push_back(this->theElts.size()-1);
  }
  void AddOnTop(const hashedVector<element, hashFunction>& x)
  { this->reserve(x.size()+this->size());
    for (int i=0; i<x.size(); i++)
      this->AddOnTop(x[i]);
  }
  hashedVector(int initialHashSize)
  { this->resetSetHashSize(initialHashSize);
  }
};

class Expression
{
  int theOperation;
  Computation* theBoss;
  public:
  int theData;
  std::vector<Expression> children;
  std::string correspondingString;
  std::string errorString;
  void reset(Computation* newBoss)
  { this->theBoss=newBoss;
    this->children.resize(0);
    this->theOperation=-1;
  }
  std::string GetToken()const;
  Expression(){this->theBoss=0; this->errorString="";}
  Expression(const Expression& other)
  { this->operator=(other);
  }
  void operator=(const Expression& other)
  { this->children=other.children;
    this->theOperation=other.theOperation;
    this->correspondingString=other.correspondingString;
    this->theBoss=other.theBoss;
    this->errorString=other.errorString;
  }
};

class Computation
{
public:
  hashedVector<std::string, hashString> Dictionary;
  hashedVector<std::string, hashString> controlSequences;
  std::vector<Expression> expressionStack;
  int numEmptyTokensStart;
  std::string ElementToString()
  { std::stringstream out;
    out << "Dictionary: ";
    for (int  i=0; i<this->Dictionary.size(); i++)
    { out << this->Dictionary[i];
      if (i!=this->Dictionary.size()-1)
        out << ", ";
    }
    return out.str();
  }
  void Evaluate(const std::string& input, std::vector<Expression>& output)
  { std::vector<std::vector< Expression> > startingExpressions;
    this->ParseFillDictionary(input, startingExpressions);
    this->ExtractExpressions(startingExpressions, output);
  }
  bool LookAheadAllowsPlus(const std::string& lookAhead)
  { return
      lookAhead=="+" || lookAhead=="-" ||
      lookAhead==")" || lookAhead=="end" ||
      lookAhead=="]" || lookAhead=="}"
      ;
  }
  bool LookAheadAllowsTimes(const std::string& lookAhead)
  { return
      lookAhead=="+" || lookAhead=="-" ||
      lookAhead=="*" || lookAhead=="/" ||
      lookAhead=="e" || lookAhead==")" ||
      lookAhead=="end" || lookAhead=="]" ||
      lookAhead=="}"
      ;
  }
  bool ReplaceEOEByE()
  { Expression& final=this->expressionStack[this->expressionStack.size()-2];
    Expression& left = this->expressionStack[this->expressionStack.size()-3];
    Expression& right = this->expressionStack[this->expressionStack.size()-1];
    final.children.push_back(left);
    final.children.push_back(right);
    std::swap(left, final);
    this->expressionStack.resize(this->expressionStack.size()-2);
    return true;
  }
  void ExtractExpressions
  (const std::vector<std::vector< Expression> >& theExpressions, std::vector<Expression>& output)
  { std::vector<Expression> expressionStack;
    std::string lookAheadToken;
    for (unsigned j=0; j<theExpressions.size(); j++)
    { this->expressionStack.resize(0);
      for (unsigned int i=0; i<theExpressions[j].size(); i++)
      { if (i+1<theExpressions[j].size())
          lookAheadToken=theExpressions[j][i+1].GetToken();
        else
          lookAheadToken="end";
        this->expressionStack.push_back(theExpressions[j][i]);
        while(this->ApplyOneRule(lookAheadToken))
        {}
      }
    }
  }
  bool ApplyOneRule(const std::string& lookAhead)
  { const Expression& lastE=this->expressionStack[expressionStack.size()-1];
    const std::string&    lastS=lastE.GetToken();
    if (lastS==" " && this->expressionStack.size()>this->numEmptyTokensStart)
    { this->expressionStack.resize(this->expressionStack.size()-1);
      return false;
    }
    const Expression& secondToLastE=this->expressionStack[expressionStack.size()-2];
    const std::string&    secondToLastS=secondToLastE.GetToken();
    const Expression& thirdToLastE=this->expressionStack[expressionStack.size()-3];
    const std::string&    thirdToLastS=thirdToLastE.GetToken();
    const Expression& fourthToLastE=this->expressionStack[expressionStack.size()-4];
    const std::string&    fourthToLastS=fourthToLastE.GetToken();
    const Expression& fifthToLastE=this->expressionStack[expressionStack.size()-5];
    const std::string&    fifthToLastS=fifthToLastE.GetToken();
    //"e" stands for expression
    if (lastS=="e" && secondToLastS=="+" && thirdToLastS=="e" && this->LookAheadAllowsPlus(lookAhead) )
      return this->ReplaceEOEByE();
    if (lastS=="e" && secondToLastS=="*" && thirdToLastS=="e" && this->LookAheadAllowsTimes(lookAhead) )
      return this->ReplaceEOEByE();

    return false;
  }

  void ParseFillDictionary
  (const std::string& input, std::vector<std::vector< Expression> >& output)
  ;
  bool isSeparator(char c);
  void reset()
  { this->Dictionary.reset();
    this->Dictionary.AddOnTop(" ");
    this->Dictionary.AddOnTop("\\");
    this->Dictionary.AddOnTop(";");
    this->controlSequences.AddOnTop("+");
    this->controlSequences.AddOnTop("-");
    this->controlSequences.AddOnTop("/");
    this->controlSequences.AddOnTop("*");
    this->controlSequences.AddOnTop(":=");
    this->controlSequences.AddOnTop("^");
    this->controlSequences.AddOnTop("+");
    this->controlSequences.AddOnTop("(");
    this->controlSequences.AddOnTop(")");
    this->controlSequences.AddOnTop("[");
    this->controlSequences.AddOnTop("]");
    this->numEmptyTokensStart=6;
  }
  Computation()
  { this->reset();
  }
};

std::string Expression::GetToken()const
{ if (this->children.size()>0)
    return "e"; //stands for expression
  if (this->theBoss->controlSequences.Contains(this->theBoss->Dictionary[this->theData]))
    return this->theBoss->Dictionary[this->theData];
  return "e"; //stands for expression
}

bool Computation::isSeparator(char c)
{ switch(c)
  { case ' ':
    case ':':
    case ',':
    case ';':
    case '+':
    case '=':
    case '/':
    case '\\':
    case '[':
    case ']':
    case '(':
    case ')':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return true;
    default:
      return false;
  }
}

void Computation::ParseFillDictionary
  (const std::string& input, std::vector<std::vector< Expression> >& output)
{ std::string current;
  output.reserve(1000);
  output.resize(0);
  char LookAheadChar;
  std::vector<Expression> emptyHead;
  Expression tempExpression;
  tempExpression.reset(this);
  tempExpression.theData=0;
  emptyHead.resize(this->numEmptyTokensStart, tempExpression);
  output.push_back(emptyHead);
  for (unsigned i=0; i<input.size(); i++)
  { current.push_back(input[i]);
    if (i+1<input.size())
      LookAheadChar=input[i+1];
    else
      LookAheadChar=' ';
    if ((current=="\\" && LookAheadChar=='\\')|| current==";")
      output.resize(output.size()+ 1, emptyHead);
    if ((this->isSeparator(current[0]) || this->isSeparator(LookAheadChar) )&& current!=" ")
    { this->Dictionary.AddNoRepetition(current);
      tempExpression.theData=this->Dictionary.GetIndex(current);
      output[output.size()-1].push_back(tempExpression);
      current="";
    }
  }
}


#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>


void getPath(char* path, std::string& output)
{ if (path==0)
    return;
  int length=0;
  output= path;
  while (output[length]!=0 && length<150)
    length++;
  for (int i=length-1; i>=0; i--)
    if (output[i]=='/')
    { output.resize(i+1);
      return;
    }
}

timeval ComputationStartTime, lastTimeMeasure;

double GetElapsedTimeInSeconds()
{ gettimeofday(&lastTimeMeasure, NULL);
  int miliSeconds =(lastTimeMeasure.tv_sec- ComputationStartTime.tv_sec)*1000+(lastTimeMeasure.tv_usec- ComputationStartTime.tv_usec)/1000;
  return ((double) miliSeconds)/1000;
}

bool AttemptToCivilize(std::string& readAhead, std::stringstream& out)
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
  if (readAhead=="%3B")
  { out << ";";
    return true;
  }
  if (readAhead=="%3C")
  { out << "<";
    return true;
  }
  if (readAhead=="%3E")
  { out << ">";
    return true;
  }
  if (readAhead=="%2F")
  { out << "/";
    return true;
  }
  if (readAhead=="%3A")
  { out << ":";
    return true;
  }
  if (readAhead=="%5E")
  { out << "^";
    return true;
  }
  if (readAhead=="%5C")
  { out << "\\";
    return true;
  }
  if (readAhead=="%26")
  { out << "&";
    return true;
  }
  if (readAhead=="%7D")
  { out << "}";
    return true;
  }
  if (readAhead=="%0D%0A")
  { return true;
  }
  return false;
}

void CivilizeCGIString(std::string& input, std::string& output)
{ std::string readAhead;
  std::stringstream out;
  int inputSize=(signed) input.size();
  for (int i=0; i<inputSize; i++)
  { readAhead="";
    for (int j=0; j<6; j++)
    { if (i+j<inputSize)
        readAhead.push_back(input[i+j]);
      if (AttemptToCivilize(readAhead, out))
      { i+=j;
        break;
      }
    }
  }
  output=out.str();
}

int main(int argc, char **argv)
{ std::string inputString, inputPath;
  std::string tempS;
	std::cin >> inputString;
//  gettimeofday(&ComputationStartGlobal, NULL);
	if (inputString=="")
    inputString=getenv("QUERY_STRING");
	getPath(argv[0], inputPath);
	std::cout << "Content-Type: text/html\n\n";
  std::cout << "<html><title>Symbolic calculator updated " << __DATE__ << "</title> <script src=\"/easy/load.js\"></script><body> ";
  CivilizeCGIString(inputString, inputString);
  int numBogusChars= 4;
  for (unsigned i=0; i<inputString.size()-numBogusChars; i++)
    inputString[i]=inputString[i+numBogusChars];
  inputString.resize(inputString.size()-numBogusChars);

  std::string beginMath="<div class=\"math\" scale=\"50\">";
  std::string endMath ="</div>";
  std::cout << "\n<form method=\"POST\" name=\"formCalculator\" action=\"/cgi-bin/symbolicCalculator\">\n" ;
  std::cout << "<textarea rows=\"3\" cols=\"30\" name=\"in1\" id=\"in1\" onkeypress=\"if (event.keyCode == 13)"
  << " { this.form.submit(); return false;}\">";
  std::cout << inputString;
  std::cout << "</textarea>\n<br>\n";
  std::cout << "<input type=\"submit\" name=\"\" value=\"Go\" onmousedown=\"storeSettings();\" > ";
//  std::cout << "<a href=\"/tmp/indicator.html\" target=\"_blank\"> Indicator window  </a>";
  std::cout << "\n</form>";
  std::cout << "<hr>result: " << inputString;
  std::cout << "</body></html>";

	return 0;   // To avoid Apache errors.
}
