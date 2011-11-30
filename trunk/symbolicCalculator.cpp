#include <iostream>
#include <vector>
#include <map>
#include <sstream>

class CommandList;
class Command;
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
  public:
  CommandList* theBoss;
  int commandIndex;
  ///////////////////////////////////////
  int theOperation;
  int theData;
  std::vector<Expression> children;
  std::string correspondingString;
  std::string errorString;
  void reset(CommandList* newBoss, int indexOfTheCommand)
  { this->theBoss=newBoss;
    this->commandIndex=indexOfTheCommand;
    this->children.resize(0);
    this->theOperation=-1;
  }
  std::string ElementToString(int recursionDepth=0, int maxRecursionDepth=-1)
  { if (maxRecursionDepth>0)
      if(recursionDepth<maxRecursionDepth)
        return "...";
    for (unsigned i=0; i<this->children.size(); i++)
    {
    }
  }
  std::string GetOperation()const;
  Expression()
  { this->theBoss=0;
    this->errorString="";
    this->theData=0;
    this->theOperation=-1;
  }
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

class SyntacticElement
{
  public:
  int controlIndex;
  Expression theData;
  void operator=(const SyntacticElement& other)
  { this->controlIndex=other.controlIndex;
    this->theData=other.theData;
  }
  SyntacticElement(){}
  SyntacticElement(const SyntacticElement& other)
  { this->operator=(other);
  }
};

class Command
{
public:
  hashedVector<std::string, hashString> Dictionary;
  CommandList* theBoss;
  std::vector<SyntacticElement> syntacticSoup;
  std::vector<SyntacticElement> expressionStacK;
  Expression finalValue;
  int numEmptyTokensStart;
  std::string ErrorString;
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
  { SyntacticElement& middle=this->expressionStacK[this->expressionStacK.size()-2];
    SyntacticElement& left = this->expressionStacK[this->expressionStacK.size()-3];
    SyntacticElement& right = this->expressionStacK[this->expressionStacK.size()-1];
    middle.theData.theOperation=this->GetOperationIndex(middle.controlIndex);
    middle.controlIndex=this->GetExpressionIndex();
    middle.theData.children.push_back(left.theData);
    middle.theData.children.push_back(right.theData);
    left=middle;
    this->expressionStacK.resize(this->expressionStacK.size()-2);
    return true;
  }
  int GetOperationIndex(int controlIndex);
  int GetExpressionIndex();
  SyntacticElement GetEmptySyntacticElement();
  bool ApplyOneRule(const std::string& lookAhead);
  bool isSeparator(char c);
  void reset(CommandList* owner=0)
  { this->numEmptyTokensStart=6;
    this->expressionStacK.resize(0);
    this->syntacticSoup.resize(0);
    this->finalValue.reset(owner, -1);
    this->theBoss=owner;
    this->ErrorString="";
  }
  bool SetError(const std::string& theError)
  { this->ErrorString=theError;
    return true;
  }
  Command()
  { this->reset();
  }
};

class CommandList
{
public:
  hashedVector<std::string, hashString> controlSequences;
  hashedVector<std::string, hashString> operations;
  hashedVector<std::string, hashString> variableDictionary;
  std::vector<Command> theCommands;

  hashedVector<std::string, hashString> cashedExpressions;

  SyntacticElement GetSyntacticElementEnd()
  { SyntacticElement result;
    result.controlIndex=this->controlSequences.GetIndex(";");
    return result;
  }

  void AddEmptyHeadedCommand();
  CommandList()
  { this->controlSequences.reset();
    this->operations.reset();
    this->variableDictionary.reset();
    this->cashedExpressions.reset();
    this->controlSequences.AddOnTop("+");
    this->controlSequences.AddOnTop("-");
    this->controlSequences.AddOnTop("/");
    this->controlSequences.AddOnTop("*");
    this->controlSequences.AddOnTop(":=");
    this->controlSequences.AddOnTop("^");
    this->controlSequences.AddOnTop("+");
    this->controlSequences.AddOnTop("\\circ");
    this->controlSequences.AddOnTop("Variable");

    this->operations.AddOnTop(this->controlSequences);

    this->controlSequences.AddOnTop(" ");//empty token
    this->controlSequences.AddOnTop("Expression");
    this->controlSequences.AddOnTop("(");
    this->controlSequences.AddOnTop(")");
    this->controlSequences.AddOnTop("[");
    this->controlSequences.AddOnTop("]");
    this->controlSequences.AddOnTop(";");//exression end
  }
  void ExtractExpressions()
  { std::string lookAheadToken;
    for (unsigned j=0; j<this->theCommands.size(); j++)
    { this->theCommands[j].expressionStacK.resize(0);
      for (unsigned i=0; i<this->theCommands[j].syntacticSoup.size(); i++)
      { if (i+1<this->theCommands[j].syntacticSoup.size())
          lookAheadToken=this->controlSequences[this->theCommands[j].syntacticSoup[i+1].controlIndex];
        else
          lookAheadToken=";";
        this->theCommands[j].expressionStacK.push_back(this->theCommands[j].syntacticSoup[i]);
        while(this->theCommands[j].ApplyOneRule(lookAheadToken))
        {}
      }
    }
  }
  void Evaluate(const std::string& input)
  { std::vector<Command> startingExpressions;
    this->ParseFillDictionary(input);
    this->ExtractExpressions();
  }
  bool isSeparator(char c);
  void ParseFillDictionary(const std::string& input);
};

bool CommandList::isSeparator(char c)
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

SyntacticElement Command::GetEmptySyntacticElement()
{ SyntacticElement result;
  result.controlIndex=this->theBoss->controlSequences.GetIndex(" ");
  result.theData.reset(0,-1);
  return result;
}

void CommandList::AddEmptyHeadedCommand()
{ this->theCommands.resize(this->theCommands.size()+1);
  Command& currentCommand=this->theCommands[this->theCommands.size()-1];
  currentCommand.reset(this);
  SyntacticElement emptyElement=currentCommand.GetEmptySyntacticElement();
  for (int i=0; i<currentCommand.numEmptyTokensStart; i++)
  { currentCommand.expressionStacK.push_back(emptyElement);
  }
}

void CommandList::ParseFillDictionary
  (const std::string& input)
{ std::string current;
  this->theCommands.reserve(1000);
  this->theCommands.resize(0);
  char LookAheadChar;
  this->AddEmptyHeadedCommand();
  SyntacticElement currentElement;
  for (unsigned i=0; i<input.size(); i++)
  { current.push_back(input[i]);
    if (i+1<input.size())
      LookAheadChar=input[i+1];
    else
      LookAheadChar=' ';
    if ((current=="\\" && LookAheadChar=='\\')|| current==";")
    { this->theCommands[this->theCommands.size()-1].syntacticSoup.push_back(this->GetSyntacticElementEnd());
      this->AddEmptyHeadedCommand();
    }
    if ((this->isSeparator(current[0]) || this->isSeparator(LookAheadChar) )&& current!=" ")
    { if (this->controlSequences.Contains(current))
      { currentElement.controlIndex=this->controlSequences.GetIndex(current);
        currentElement.theData.reset(this, -1);
        this->theCommands[this->theCommands.size()-1].syntacticSoup.push_back(currentElement);
      } else
      { this->variableDictionary.AddNoRepetition(current);
        currentElement.controlIndex=this->controlSequences.GetIndex("Variable");
        currentElement.theData.reset(this, this->theCommands.size()-1);
        currentElement.theData.theOperation=this->operations.GetIndex("Variable");
        currentElement.theData.theData=this->variableDictionary.GetIndex(current);
        this->theCommands[this->theCommands.size()-1].syntacticSoup.push_back(currentElement);
      }
    }
  }
}

int Command::GetOperationIndex(int controlIndex)
{ return this->theBoss->operations.GetIndex(this->theBoss->controlSequences[controlIndex]);
}

int Command::GetExpressionIndex()
{ return this->theBoss->controlSequences.GetIndex("Expression");
}

bool Command::ApplyOneRule(const std::string& lookAhead)
{ const SyntacticElement& lastE=this->expressionStacK[this->expressionStacK.size()-1];
  const std::string&    lastS=this->theBoss->controlSequences[lastE.controlIndex];
  if (lastS==" " && signed (this->expressionStacK.size())>this->numEmptyTokensStart)
  { this->expressionStacK.resize(this->expressionStacK.size()-1);
    return false;
  }
  const SyntacticElement& secondToLastE=this->expressionStacK[this->expressionStacK.size()-2];
  const std::string&    secondToLastS=this->theBoss->controlSequences[secondToLastE.controlIndex];
  const SyntacticElement& thirdToLastE=this->expressionStacK[this->expressionStacK.size()-3];
  const std::string&    thirdToLastS=this->theBoss->controlSequences[thirdToLastE.controlIndex];
  const SyntacticElement& fourthToLastE=this->expressionStacK[this->expressionStacK.size()-4];
  const std::string&    fourthToLastS=this->theBoss->controlSequences[fourthToLastE.controlIndex];
  const SyntacticElement& fifthToLastE=this->expressionStacK[this->expressionStacK.size()-5];
  const std::string&    fifthToLastS=this->theBoss->controlSequences[fifthToLastE.controlIndex];
  if (lastS=="Variable")
    return this->SetError("The keyword \"Variable\" is reserved. Please use another word.");
  if (lastS=="Expression" && secondToLastS=="+" && thirdToLastS=="e" && this->LookAheadAllowsPlus(lookAhead) )
    return this->ReplaceEOEByE();
  if (lastS=="Expression" && secondToLastS=="*" && thirdToLastS=="e" && this->LookAheadAllowsTimes(lookAhead) )
    return this->ReplaceEOEByE();
  return false;
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
  for (signed i=0; i<signed(inputString.size())-numBogusChars; i++)
    inputString[i]=inputString[i+numBogusChars];
  int newSize=signed(inputString.size())-numBogusChars;
  if (newSize<0)
    newSize=0;
  inputString.resize(newSize);
  inputString = "f~(x):=x+1";

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
  std::cout << "<hr>input: " << "<span class=\"math\">" << inputString << "</span>";
  std::cout << "<hr>result: ";
  std::cout << "</body></html>";

	return 0;   // To avoid Apache errors.
}
