#include "polyhedra.h"
// This file is meant to be for people to modify if they do not want to modify the main files polyhedra.cpp or polyhedra.h
// The reason I would recommend that is because the file polyhedra.cpp compiles very slowly (around 30 seconds), so small modifications
// take a long time to check. Using a separate file and linking it to the GUI is the solution I chose (may not be the best, see below for discussion).
// If the code you write here reaches the mature phase where you have realized all functions you think it should have
// and you are generally satisfied with it, simply cut & paste it in the main files (the class declarations in polyhedra.h and the implementation in polyhedra.cpp).

//Discussion: other options for speeding up the compilation that I have considered.
//1. Cut up the main files polyhedra.h and polyhedra.cpp into small quick-to-compile pieces. This might be the best solution in the long run. However, I do not want to
//   do it just yet, because 1) I am not sure what should those parts be - it is not yet clear in my head how to partition the code in conceptually distinct pieces
//   2) this would certainly create additional maintainance work 3) this will increase the learning curve for a person wanting to just use the program and wanting to eventually
//   modify some tiny bit and 4) I got an advice on the c++ forum www.cplusplus.com that partitioning the .h file will eventually lead to slower compile times,
//   especially with the massive use of templates that I do. Therefore, such a partitioning should not be done before the code reaches
//   greater maturity (see also point (1) ).
//2. Use precompiled headers or some other voodoo. I am tocally against that. Those are compiler specific, will require me to learn extra unnecessary info which might
//    be out of date in a few years, and will make even higher entry learning curve for another to join the project. This is bad.
//    I should mention in favor of Microsoft that their IDE does recompile very quickly small modifications of the file polyhedra.cpp. I believe it does so by
//    keeping a database of your recently changed code, and recompiling only the recently changed pieces. Hats off to Microsoft for doing that completely programmatically,
//    and not bothering the programmer with stupid things such as how to set up precompiled headers.
//
// To whomever might be reading this (if anyone): happy hacking and I hope you find my code useful, that it didn't cause you many headaches, and that you
// did something useful with it! Cheers!

extern IndicatorWindowVariables IndicatorWindowGlobalVariables;
class ElementWeylAlgebra
{
private:
  PolynomialRationalCoeff StandardOrder;
public:
  std::string DebugString;
  void ComputeDebugString(bool useBeginEqnArray, bool useXYs){this->ElementToString(this->DebugString, useXYs, true, useBeginEqnArray);};
  void ElementToString(std::string& output, ListBasicObjects<std::string>& alphabet, bool useLatex, bool useBeginEqnArray);
	void ElementToString(std::string& output, bool useXYs, bool useLatex, bool useBeginEqnArray);
  int NumVariables;
  void MakeGEpsPlusEpsInTypeD(int i, int j, int NumVars);
  void MakeGEpsMinusEpsInTypeD(int i, int j, int NumVars);
  void MakeGMinusEpsMinusEpsInTypeD(int i, int j, int NumVars);
  void Makexixj(int i, int j, int NumVars);
  void Makexi(int i, int NumVars);
  void Makedi(int i, int NumVars);
  void Makedidj(int i, int j, int NumVars);
  void Makexidj(int i, int j, int NumVars);
  void Nullify(int NumVars);
  void SetNumVariablesPreserveExistingOnes(int newNumVars);
  void TimesConstant(Rational& theConstant)
  { this->StandardOrder.TimesConstant(theConstant);
  };
  void MultiplyOnTheLeft(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables);
  void MultiplyOnTheRight(ElementWeylAlgebra& standsOnTheRight, GlobalVariables& theGlobalVariables);
  void LieBracketOnTheLeft(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables);
  void LieBracketOnTheLeftMakeReport(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables, std::string& report);
  void LieBracketOnTheRightMakeReport(ElementWeylAlgebra& standsOnTheRight, GlobalVariables& theGlobalVariables, std::string& report);
  void LieBracketOnTheRight(ElementWeylAlgebra& standsOnTheRight, GlobalVariables& theGlobalVariables);
  void Assign(const ElementWeylAlgebra& other)
  { this->StandardOrder.Assign(other.StandardOrder);
    this->NumVariables=other.NumVariables;
  };
  void Subtract(ElementWeylAlgebra& other)
  { this->StandardOrder.Subtract(other.StandardOrder);
  };
  void MakeConst (int NumVars, Rational& theConst);
  void Add(ElementWeylAlgebra& other)
  { this->StandardOrder.AddPolynomial(other.StandardOrder);
  };
  void MultiplyTwoMonomials( Monomial<Rational>& left, Monomial<Rational>& right, PolynomialRationalCoeff& OrderedOutput, GlobalVariables& theGlobalVariables);
  ElementWeylAlgebra()
  { this->NumVariables=0; };
  void operator=(const std::string& input);
  bool IsLetter(char theLetter);
  bool IsIndex(char theIndex);
  bool IsNumber(char theNumber);
};

class WeylParser;
class WeylParserNode
{
  public:
  std::string DebugString;
  void ComputeDebugString(){this->ElementToString(DebugString);};
  void ElementToString(std::string& output);
  WeylParser* owner;
  int indexParent;
  ListBasicObjects<int> children;
  ElementWeylAlgebra Value;
  int Operation; //using tokenTypes from class WeylParser
  void operator=(const WeylParserNode& other)
  { this->owner=other.owner;
    this->children.CopyFromBase(other.children);
    this->indexParent=other.indexParent;
  };
  void Evaluate(GlobalVariables& theGlobalVariables);
  WeylParserNode();
};

//the below class was written and implemented by an idea of helios from the forum of www.cplusplus.com
class WeylParser: public ListBasicObjects<WeylParserNode>
{
  public:
  std::string DebugString;
  void ComputeDebugString(){this->ElementToString(DebugString);};
  void ElementToString(std::string& output);
  enum tokenTypes
  { tokenExpression, tokenEmpty, tokenDigit, tokenPlus, tokenMinus, tokenUnderscore,  tokenTimes, tokenDivide, tokenPower, tokenOpenBracket, tokenCloseBracket,
    tokenOpenLieBracket, tokenCloseLieBracket, tokenX, tokenPartialDerivative, tokenComma, tokenLieBracket
  };
  ListBasicObjects<int> TokenBuffer;
  ListBasicObjects<int> ValueBuffer;
  ListBasicObjects<int> TokenStack;
  ListBasicObjects<int> ValueStack;
  int numEmptyTokensAtBeginning;
  int NumVariables;
  ElementWeylAlgebra Value;
  void ParserInit(const std::string& input);
  void Evaluate(GlobalVariables& theGlobalVariables);
  void Parse(const std::string& input);
  void ParseNoInit(int indexFrom, int indexTo);
  void ParseAndCompute(const std::string& input, std::string& output, GlobalVariables& theGlobalVariables);
  bool ApplyRules(int lookAheadToken);
  bool lookAheadTokenProhibitsPlus(int theToken);
  bool lookAheadTokenProhibitsTimes(int theToken);
  void AddLetterOnTop(int index);
  void AddPartialOnTop(int index);
  void Own(int indexParent, int indexChild1, int indexChild2);
  void ExtendOnTop(int numNew);
  void TokenToStringStream(std::stringstream& out, int theToken);
  void AddPlusOnTop();
  void AddMinusOnTop();
  void AddTimesOnTop();
  void AddLieBracketOnTop();
  void DecreaseStackSetExpressionLastNode(int Decrease);
  void ComputeNumberOfVariablesAndAdjustNodes();
  WeylParser()
  { this->numEmptyTokensAtBeginning=5;
  };
};

bool ElementWeylAlgebra::IsLetter(char theLetter)
{
  return false;
}

void ElementWeylAlgebra::operator=(const std::string& input)
{ /*for (int i=0; i<input.size(); i++)
  { std::ostream formatter;
    if (input.at(i)=='x')

  }*/
}

void ElementWeylAlgebra::MultiplyTwoMonomials( Monomial<Rational>& left, Monomial<Rational>& right, PolynomialRationalCoeff& OrderedOutput, GlobalVariables& theGlobalVariables)
{ Monomial<Rational> buffer;
  SelectionWithDifferentMaxMultiplicities& tempSel=theGlobalVariables.selWeylAlgebra1;
  assert(left.NumVariables%2==0);
  int theDimension=left.NumVariables/2;
  tempSel.Multiplicities.initFillInObject(theDimension, 0);
  tempSel.MaxMultiplicities.SetSizeExpandOnTopNoObjectInit(theDimension);
  for (int i=0; i<theDimension; i++)
    tempSel.MaxMultiplicities.TheObjects[i]=left.degrees[theDimension+i];
  tempSel.elements.initFillInObject(theDimension,0);
  buffer.init(left.NumVariables);
  assert(left.NumVariables==right.NumVariables);
  OrderedOutput.Nullify(left.NumVariables);
  int numCycles= tempSel.getTotalNumSubsets();
  Rational coeffProd;
  coeffProd.Assign(left.Coefficient);
  coeffProd.MultiplyBy(right.Coefficient);
  for (int i=0; i<numCycles; i++)
  { buffer.Coefficient.Assign(coeffProd);
    for (int k=0; k< theDimension; k++)
    { short multDrop=(short)tempSel.Multiplicities.TheObjects[k];
      buffer.Coefficient.MultiplyBy(Rational::NChooseK(left.degrees[theDimension+k], multDrop)*Rational::NChooseK(right.degrees[k], multDrop)* Rational::Factorial(multDrop));
      buffer.degrees[k]=left.degrees[k]+right.degrees[k]-multDrop;
      buffer.degrees[k+theDimension]= left.degrees[k+theDimension]+right.degrees[k+theDimension]-multDrop;
    }
    OrderedOutput.AddMonomial(buffer);
    tempSel.IncrementSubset();
  }
}

void ElementWeylAlgebra::LieBracketOnTheLeftMakeReport(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables, std::string& report)
{ std::stringstream out;
  this->ComputeDebugString(false, false);
  standsOnTheLeft.ComputeDebugString(false, false);
  out << "\\begin{eqnarray*}&&["<< standsOnTheLeft.DebugString<<" , "<< this->DebugString<< " ]= ";
  this->LieBracketOnTheLeft(standsOnTheLeft, theGlobalVariables);
  this->ComputeDebugString(false, false);
  out << this->DebugString<<"\\end{eqnarray*}\n";
  report=out.str();
}

void ElementWeylAlgebra::LieBracketOnTheRightMakeReport(ElementWeylAlgebra& standsOnTheRight, GlobalVariables& theGlobalVariables, std::string& report)
{ std::stringstream out;
  this->ComputeDebugString(false, false);
  standsOnTheRight.ComputeDebugString(false, false);
  out << "\\begin{eqnarray*}&&["<< this->DebugString<< " , "<<standsOnTheRight.DebugString<<" ]= ";
  this->LieBracketOnTheRight(standsOnTheRight, theGlobalVariables);
  this->ComputeDebugString(false, false);
  out << this->DebugString<<"\\end{eqnarray*}\n";
  report=out.str();
}

void ElementWeylAlgebra::LieBracketOnTheLeft(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables)
{ ElementWeylAlgebra tempEl1, tempEl2;
  tempEl1.Assign(*this);
  tempEl1.MultiplyOnTheLeft(standsOnTheLeft, theGlobalVariables);
  //tempEl1.ComputeDebugString(false);
  tempEl2.Assign(standsOnTheLeft);
  tempEl2.MultiplyOnTheLeft(*this, theGlobalVariables);
  //tempEl2.ComputeDebugString(false);
  this->Assign(tempEl1);
  this->Subtract(tempEl2);
  //this->ComputeDebugString(false);
}

void ElementWeylAlgebra::MakeConst(int NumVars, Rational& theConst)
{ this->Nullify(NumVariables);
  Monomial<Rational> tempM;
  tempM.init((short)this->NumVariables*2);
  tempM.Coefficient.Assign(theConst);
  this->StandardOrder.AddMonomial(tempM);
}

void ElementWeylAlgebra::LieBracketOnTheRight(ElementWeylAlgebra& standsOnTheRight, GlobalVariables& theGlobalVariables)
{ ElementWeylAlgebra tempEl1, tempEl2;
  tempEl1.Assign(standsOnTheRight);
  tempEl1.MultiplyOnTheLeft(*this, theGlobalVariables);
  //tempEl1.ComputeDebugString(false);
  tempEl2.Assign(*this);
  tempEl2.MultiplyOnTheLeft(standsOnTheRight, theGlobalVariables);
  //tempEl2.ComputeDebugString(false);
  this->Assign(tempEl1);
  this->Subtract(tempEl2);
  //this->ComputeDebugString(false);
}

void ElementWeylAlgebra::MultiplyOnTheLeft(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables)
{ PolynomialRationalCoeff buffer;
  PolynomialRationalCoeff Accum;
  Accum.Nullify((short) this->NumVariables*2);
  for (int j=0; j<standsOnTheLeft.StandardOrder.size; j++)
    for (int i=0; i<this->StandardOrder.size; i++)
    { this->MultiplyTwoMonomials(standsOnTheLeft.StandardOrder.TheObjects[j], this->StandardOrder.TheObjects[i], buffer, theGlobalVariables);
      Accum.AddPolynomial(buffer);
    }
  this->StandardOrder.Assign(Accum);
}

void ElementWeylAlgebra::MultiplyOnTheRight(ElementWeylAlgebra& standsOnTheRight, GlobalVariables& theGlobalVariables)
{ PolynomialRationalCoeff buffer;
  PolynomialRationalCoeff Accum;
  Accum.Nullify((short) this->NumVariables*2);
  for (int j=0; j<standsOnTheRight.StandardOrder.size; j++)
    for (int i=0; i<this->StandardOrder.size; i++)
    { this->MultiplyTwoMonomials(this->StandardOrder.TheObjects[i], standsOnTheRight.StandardOrder.TheObjects[j], buffer, theGlobalVariables);
      Accum.AddPolynomial(buffer);
    }
  this->StandardOrder.Assign(Accum);
}

void ElementWeylAlgebra::ElementToString(std::string& output, ListBasicObjects<std::string>& alphabet, bool useLatex, bool useBeginEqnArray)
{ std::stringstream out;
  std::string tempS;
  int numLettersSinceLastNewLine=0;
  if (useLatex && useBeginEqnArray)
    out << "\\begin{eqnarray*}&&";
  for (int i=0; i<this->StandardOrder.size; i++)
  { this->StandardOrder.TheObjects[i].Coefficient.ElementToString(tempS);
    bool hasMinus=(tempS[0]=='-');
    if (this->StandardOrder.TheObjects[i].TotalDegree()!=0)
    { if (tempS=="1")
        tempS="";
      if (tempS=="-1")
        tempS="-";
    }
    if (!hasMinus && i!=0)
      out<<'+';
    out << tempS;
    for (int k=0; k<this->NumVariables; k++)
      if (this->StandardOrder.TheObjects[i].degrees[k]!=0)
      { out <<"{"<<alphabet.TheObjects[k]<<"}";
        int tempI=(int) this->StandardOrder.TheObjects[i].degrees[k];
        if (tempI!=1)
          out <<"^{"<< tempI<<"}";
      }
    for (int k=0; k<this->NumVariables; k++)
      if (this->StandardOrder.TheObjects[i].degrees[this->NumVariables+ k]!=0)
      { out <<"\\partial_{"<<alphabet.TheObjects[k]<<"}";
				numLettersSinceLastNewLine++;
        int tempI=this->StandardOrder.TheObjects[i].degrees[this->NumVariables+k];
        if (tempI!=1)
          out <<"^{" <<tempI<<"}";
      }
    if (numLettersSinceLastNewLine>12 && i!= this->StandardOrder.size-1 && useLatex)
    { numLettersSinceLastNewLine=0;
      out <<"\\\\&&\n";
    }
  }
  if (useLatex && useBeginEqnArray)
    out << "\\end{eqnarray*}";
  output=out.str();
}

void ElementWeylAlgebra::ElementToString(std::string& output, bool useXYs, bool useLatex, bool useBeginEqnArray)
{ ListBasicObjects<std::string> alphabet;
  alphabet.SetSizeExpandOnTopNoObjectInit(this->NumVariables);
  int numCycles=this->NumVariables;
  if (useXYs)
  { assert(this->NumVariables%2==0);
    numCycles= this->NumVariables/2;
  }
  for (int i=0; i<numCycles; i++)
  { std::stringstream out, out2;
    out << "x_"<<i+1;
    alphabet.TheObjects[i]= out.str();
    if (useXYs)
    { out2<< "y_"<< i+1;
      alphabet.TheObjects[i+numCycles]= out2.str();
    }
  }
  this->ElementToString(output, alphabet, useLatex, useBeginEqnArray);
}

void ElementWeylAlgebra::SetNumVariablesPreserveExistingOnes(int newNumVars)
{ if (newNumVars<this->NumVariables)
    this->NumVariables=newNumVars;
  PolynomialRationalCoeff Accum;
  Accum.Nullify((short)newNumVars*2);
  Accum.MakeActualSizeAtLeastExpandOnTop(this->StandardOrder.size);
  Monomial<Rational> tempM;
  for (int i=0; i<this->StandardOrder.size; i++)
  { tempM.init((short)newNumVars*2);
    for (int j=0; j< this->NumVariables; j++)
    { tempM.degrees[j]=this->StandardOrder.TheObjects[i].degrees[j];
      tempM.degrees[j+newNumVars]=this->StandardOrder.TheObjects[i].degrees[j+this->NumVariables];
    }
    tempM.Coefficient.Assign(this->StandardOrder.TheObjects[i].Coefficient);
    Accum.AddMonomial(tempM);
  }
  this->NumVariables= newNumVars;
  this->StandardOrder.Assign(Accum);
}

void ElementWeylAlgebra::MakeGEpsPlusEpsInTypeD(int i, int j, int NumVars)
{ this->Nullify(NumVars*2);
  Monomial<Rational> tempMon;
  tempMon.Coefficient.MakeOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[i]=1;
  tempMon.degrees[j+this->NumVariables+NumVars]=1;
  this->StandardOrder.AddMonomial(tempMon);

  tempMon.Coefficient.MakeMOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[j]=1;
  tempMon.degrees[i+this->NumVariables+NumVars]=1;
  this->StandardOrder.AddMonomial(tempMon);
}

void ElementWeylAlgebra::MakeGEpsMinusEpsInTypeD(int i, int j, int NumVars)
{ this->Nullify(NumVars*2);
  Monomial<Rational> tempMon;
  tempMon.Coefficient.MakeOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[i]=1;
  tempMon.degrees[j+this->NumVariables]=1;
  this->StandardOrder.AddMonomial(tempMon);

  tempMon.Coefficient.MakeMOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[j+NumVars]=1;
  tempMon.degrees[i+this->NumVariables+NumVars]=1;
  this->StandardOrder.AddMonomial(tempMon);
}

void ElementWeylAlgebra::MakeGMinusEpsMinusEpsInTypeD(int i, int j, int NumVars)
{ this->Nullify(NumVars*2);
  Monomial<Rational> tempMon;
  tempMon.Coefficient.MakeOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[i+NumVars]=1;
  tempMon.degrees[j+this->NumVariables]=1;
  this->StandardOrder.AddMonomial(tempMon);

  tempMon.Coefficient.MakeMOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[j+NumVars]=1;
  tempMon.degrees[i+this->NumVariables]=1;
  this->StandardOrder.AddMonomial(tempMon);
}

void ElementWeylAlgebra::Makedidj(int i, int j, int NumVars)
{ this->Nullify(NumVars);
  Monomial<Rational> tempMon;
  tempMon.Coefficient.MakeOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[i+NumVars]++;
  tempMon.degrees[j+NumVars]++;
  this->StandardOrder.AddMonomial(tempMon);
}

void ElementWeylAlgebra::Makexixj(int i, int j, int NumVars)
{ this->Nullify(NumVars);
  Monomial<Rational> tempMon;
  tempMon.Coefficient.MakeOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[i]++;
  tempMon.degrees[j]++;
  this->StandardOrder.AddMonomial(tempMon);
}

void ElementWeylAlgebra::Makexi(int i, int NumVars)
{ this->Nullify(NumVars);
  Monomial<Rational> tempMon;
  tempMon.Coefficient.MakeOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[i]++;
  this->StandardOrder.AddMonomial(tempMon);
}

void ElementWeylAlgebra::Makedi(int i, int NumVars)
{ this->Nullify(NumVars);
  Monomial<Rational> tempMon;
  tempMon.Coefficient.MakeOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[i+NumVars]++;
  this->StandardOrder.AddMonomial(tempMon);
}

void ElementWeylAlgebra::Makexidj(int i, int j, int NumVars)
{ this->Nullify(NumVars);
  Monomial<Rational> tempMon;
  tempMon.Coefficient.MakeOne();
  tempMon.MakeConstantMonomial((short)this->NumVariables*2, tempMon.Coefficient);
  tempMon.degrees[i]=1;
  tempMon.degrees[NumVars+j]=1;
  this->StandardOrder.AddMonomial(tempMon);
}

void ElementWeylAlgebra::Nullify(int NumVars)
{ this->NumVariables=NumVars;
  this->StandardOrder.Nullify ((short)this->NumVariables*2);
}

int DebugCounter=-1;
WeylParser debugParser;

void WeylParser::ParserInit(const std::string& input)
{ this->TokenStack.MakeActualSizeAtLeastExpandOnTop(input.size());
  this->ValueStack.MakeActualSizeAtLeastExpandOnTop(input.size());
  this->MakeActualSizeAtLeastExpandOnTop(input.size());
  this->TokenStack.size=0;
  this->ValueStack.size=0;
  this->size=0;
  for (int i=0; i<(signed) input.size(); i++)
  { switch (input.at(i))
    { case '0': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(0); break;
      case '1': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(1); break;
      case '2': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(2); break;
      case '3': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(3); break;
      case '4': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(4); break;
      case '5': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(5); break;
      case '6': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(6); break;
      case '7': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(7); break;
      case '8': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(8); break;
      case '9': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDigit); this->ValueBuffer.AddObjectOnTop(9); break;
      case '[': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenOpenLieBracket); this->ValueBuffer.AddObjectOnTop(0); break;
      case ']': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenCloseLieBracket); this->ValueBuffer.AddObjectOnTop(0); break;
      case '(': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenOpenBracket); this->ValueBuffer.AddObjectOnTop(0); break;
      case ',': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenComma); this->ValueBuffer.AddObjectOnTop(0); break;
      case ')': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenCloseBracket); this->ValueBuffer.AddObjectOnTop(0); break;
      case '^': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenPower); this->ValueBuffer.AddObjectOnTop(0); break;
      case '+': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenPlus); this->ValueBuffer.AddObjectOnTop(0); break;
      case '-': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenMinus); this->ValueBuffer.AddObjectOnTop(0); break;
      case '_': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenUnderscore); this->ValueBuffer.AddObjectOnTop(0); break;
      case '/': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenDivide); this->ValueBuffer.AddObjectOnTop(0); break;
      case 'x': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenX); this->ValueBuffer.AddObjectOnTop(0); break;
      case 'd': this->TokenBuffer.AddObjectOnTop(WeylParser::tokenPartialDerivative); this->ValueBuffer.AddObjectOnTop(0); break;
    }
  }
  this->ValueStack.size=0;
  this->TokenStack.size=0;
  this->ValueStack.MakeActualSizeAtLeastExpandOnTop(this->ValueBuffer.size);
  this->TokenStack.MakeActualSizeAtLeastExpandOnTop(this->TokenBuffer.size);
  for (int i=0; i<this->numEmptyTokensAtBeginning; i++)
  { this->TokenStack.AddObjectOnTop(this->tokenEmpty);
    this->ValueStack.AddObjectOnTop(0);
  }
}

void WeylParser::Parse(const std::string& input)
{ this->ParserInit(input);
  this->ParseNoInit(0, this->TokenBuffer.size-1);
  this->ComputeNumberOfVariablesAndAdjustNodes();
}

void WeylParser::ParseNoInit(int indexFrom, int indexTo)
{ if (indexFrom<0 || indexTo>= this->TokenBuffer.size)
    return;
  for (int i=indexFrom; i<=indexTo; i++)
  { this->ValueStack.AddObjectOnTop(this->ValueBuffer.TheObjects[i]);
    this->TokenStack.AddObjectOnTop(this->TokenBuffer.TheObjects[i]);
    int lookAheadToken=this->tokenEmpty;
    if (i<this->TokenBuffer.size-1)
      lookAheadToken=this->TokenBuffer.TheObjects[i+1];
    while(this->ApplyRules(lookAheadToken))
    {
    }
  }
}

bool WeylParser::lookAheadTokenProhibitsPlus(int theToken)
{ if (theToken==this->tokenPlus)
    return false;
  if (theToken==this->tokenMinus)
    return false;
  if (theToken==this->tokenCloseBracket)
    return false;
  if (theToken==this->tokenCloseLieBracket)
    return false;
  return true;
}

bool WeylParser::lookAheadTokenProhibitsTimes(int theToken)
{ if (theToken==this->tokenPower)
    return true;
  return false;
}

bool WeylParser::ApplyRules(int lookAheadToken)
{ if (this->TokenStack.size<2)
    return false;
  int IndexLast=this->TokenStack.size-1;
  if (this->TokenStack.TheObjects[IndexLast]==this->tokenEmpty)
  { this->TokenStack.PopLastObject();
    this->ValueStack.PopLastObject();
    return true;
  }
  if (this->TokenStack.TheObjects[IndexLast]== this->tokenCloseLieBracket && this->TokenStack.TheObjects[IndexLast-1]==this->tokenExpression &&
      this->TokenStack.TheObjects[IndexLast-2]==this->tokenComma && this->TokenStack.TheObjects[IndexLast-3]==this->tokenExpression &&
      this->TokenStack.TheObjects[IndexLast-4]==this->tokenOpenLieBracket)
  { this->AddLieBracketOnTop();
    return true;
  }
  if (this->TokenStack.TheObjects[IndexLast-1]==this->tokenUnderscore && this->TokenStack.TheObjects[IndexLast]==this->tokenDigit && this->TokenStack.TheObjects[IndexLast-2]==this->tokenPartialDerivative)
  { this->AddPartialOnTop(this->ValueStack.TheObjects[IndexLast]);
    return true;
  }
  if (this->TokenStack.TheObjects[IndexLast-1]==this->tokenUnderscore && this->TokenStack.TheObjects[IndexLast]==this->tokenDigit && this->TokenStack.TheObjects[IndexLast-2]==this->tokenX)
  { this->AddLetterOnTop(this->ValueStack.TheObjects[IndexLast]);
    return true;
  }
  if (this->TokenStack.TheObjects[IndexLast]==this->tokenExpression && this->TokenStack.TheObjects[IndexLast-1]==this->tokenExpression && !this->lookAheadTokenProhibitsTimes(lookAheadToken))
  { this->AddTimesOnTop();
    return true;
  }
  if( this->TokenStack.TheObjects[IndexLast-1]==this->tokenPlus && this->TokenStack.TheObjects[IndexLast]==this->tokenExpression &&
      this->TokenStack.TheObjects[IndexLast-2]==this->tokenExpression && !this->lookAheadTokenProhibitsPlus(lookAheadToken))
  { this->AddPlusOnTop();
    return true;
  }
  if( this->TokenStack.TheObjects[IndexLast-1]==this->tokenMinus && this->TokenStack.TheObjects[IndexLast]==this->tokenExpression &&
      this->TokenStack.TheObjects[IndexLast-2]==this->tokenExpression && !this->lookAheadTokenProhibitsPlus(lookAheadToken))
  { this->AddMinusOnTop();
    return true;
  }
  return false;
}

void WeylParser::DecreaseStackSetExpressionLastNode(int Decrease)
{ this->TokenStack.size-=Decrease;
  this->ValueStack.size-=Decrease;
  *this->TokenStack.LastObject()=this->tokenExpression;
  *this->ValueStack.LastObject()=this->size-1;
}

void WeylParser::AddLetterOnTop(int index)
{ if (index<=0)
    return;
  this->ExtendOnTop(1);
  this->LastObject()->Value.Makexi(index-1, index);
  this->DecreaseStackSetExpressionLastNode(2);
}

void WeylParser::AddPartialOnTop(int index)
{ if (index<=0)
    return;
  this->ExtendOnTop(1);
  this->LastObject()->Value.Makedi(index-1, index);
  this->DecreaseStackSetExpressionLastNode(2);
}

void WeylParser::AddPlusOnTop()
{ this->ExtendOnTop(1);
  WeylParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenPlus;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-3], this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(2);
}

void WeylParser::AddMinusOnTop()
{ this->ExtendOnTop(1);
  WeylParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenMinus;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-3], this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(2);
}

void WeylParser::AddTimesOnTop()
{ this->ExtendOnTop(1);
  WeylParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenTimes;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-2], this->ValueStack.TheObjects[this->ValueStack.size-1]);
  this->DecreaseStackSetExpressionLastNode(1);
}

void WeylParser::AddLieBracketOnTop()
{ this->ExtendOnTop(1);
  WeylParserNode* theNode=this->LastObject();
  theNode->Operation=this->tokenLieBracket;
  this->Own(this->size-1, this->ValueStack.TheObjects[this->ValueStack.size-4], this->ValueStack.TheObjects[this->ValueStack.size-2]);
  this->DecreaseStackSetExpressionLastNode(4);
}

void WeylParser::ParseAndCompute(const std::string& input, std::string& output, GlobalVariables& theGlobalVariables)
{ std::stringstream out; std::string tempS;
  this->Parse(input);
  out <<"\\begin{eqnarray*}&&" << input<<" = \\\\\n";
  this->Evaluate(theGlobalVariables);
  this->Value.ElementToString(tempS, false, true, false);
  out <<tempS;
  out <<"\\end{eqnarray*}";
  output=out.str();
}

void WeylParser::Own(int indexParent, int indexChild1, int indexChild2)
{ WeylParserNode* theNode= & this->TheObjects[indexParent];
  theNode->children.SetSizeExpandOnTopNoObjectInit(2);
  theNode->children.TheObjects[0]=indexChild1;
  theNode->children.TheObjects[1]=indexChild2;
  this->TheObjects[indexChild1].indexParent= indexParent;
  this->TheObjects[indexChild2].indexParent= indexParent;
}

void WeylParser::Evaluate(GlobalVariables& theGlobalVariables)
{ if (this->TokenStack.size== this->numEmptyTokensAtBeginning+1)
    if (*this->TokenStack.LastObject()==this->tokenExpression)
    { WeylParserNode* theNode=&this->TheObjects[this->ValueStack.TheObjects[this->numEmptyTokensAtBeginning]];
      theNode->Evaluate(theGlobalVariables);
      this->Value.Assign(theNode->Value);
    }
}

void WeylParser::ExtendOnTop(int numNew)
{ this->SetSizeExpandOnTopNoObjectInit(this->size+numNew);
  for (int i=0; i<numNew; i++)
    this->TheObjects[this->size-1-i].owner=this;
}

void WeylParserNode::Evaluate(GlobalVariables& theGlobalVariables)
{ Rational tempRat;
  switch (this->Operation)
  { case WeylParser::tokenPlus: this->Value.Nullify(owner->NumVariables);
      for (int i=0; i<this->children.size; i++)
      { this->owner->TheObjects[this->children.TheObjects[i]].Evaluate(theGlobalVariables);
        this->Value.Add(this->owner->TheObjects[this->children.TheObjects[i]].Value);
      }
      break;
    case WeylParser::tokenTimes: tempRat.MakeOne();
      this->Value.MakeConst(owner->NumVariables, tempRat);
      for (int i=0; i<this->children.size; i++)
      { this->owner->TheObjects[this->children.TheObjects[i]].Evaluate(theGlobalVariables);
        this->Value.MultiplyOnTheRight(this->owner->TheObjects[this->children.TheObjects[i]].Value, theGlobalVariables);
      }
      break;
    case WeylParser::tokenLieBracket:
      this->owner->TheObjects[children.TheObjects[0]].Evaluate(theGlobalVariables);
      this->owner->TheObjects[children.TheObjects[1]].Evaluate(theGlobalVariables);
      this->Value.Assign(owner->TheObjects[children.TheObjects[0]].Value);
      this->Value.LieBracketOnTheRight(owner->TheObjects[children.TheObjects[1]].Value, theGlobalVariables);
      break;
    }

}

WeylParserNode::WeylParserNode()
{ this->Operation= WeylParser::tokenEmpty;
}


void WeylParser::ElementToString(std::string& output)
{ std::stringstream out; std::string tempS;
  out <<"\nToken stack: ";
  for (int i=this->numEmptyTokensAtBeginning; i<this->TokenStack.size; i++)
  { this->TokenToStringStream(out, this->TokenStack.TheObjects[i]);
    out <<", ";
  }
  out <<"\nValue stack: ";
  for (int i=this->numEmptyTokensAtBeginning; i<this->ValueStack.size; i++)
    out <<this->ValueStack.TheObjects[i]<<", ";
  out <<"\nElements: ";
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].ElementToString(tempS);
    out << " Index: "<< i<<" " <<tempS<<"; ";
  }
  output=out.str();
}

void WeylParser::TokenToStringStream(std::stringstream& out, int theToken)
{ switch(theToken)
  { case WeylParser::tokenX: out <<"x"; break;
    case WeylParser::tokenDigit: out <<"D"; break;
    case WeylParser::tokenPlus: out <<"+"; break;
    case WeylParser::tokenUnderscore: out <<"_"; break;
    case WeylParser::tokenEmpty: out <<" "; break;
    case WeylParser::tokenExpression: out<<"E"; break;
    case WeylParser::tokenOpenLieBracket: out <<"["; break;
    case WeylParser::tokenCloseLieBracket: out <<"]"; break;
    case WeylParser::tokenLieBracket: out <<"[,]"; break;
    case WeylParser::tokenComma: out <<","; break;
    case WeylParser::tokenPartialDerivative: out <<"d"; break;
    case WeylParser::tokenTimes: out << "*"; break;
    case WeylParser::tokenMinus: out<< "-"; break;
    default: out <<"?"; break;
  }
}

void WeylParser::ComputeNumberOfVariablesAndAdjustNodes()
{ this->NumVariables=0;
  for(int i=0; i<this->size; i++)
    if (this->NumVariables< this->TheObjects[i].Value.NumVariables)
      this->NumVariables=this->TheObjects[i].Value.NumVariables;
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].Value.SetNumVariablesPreserveExistingOnes(this->NumVariables);
}

void WeylParserNode::ElementToString(std::string& output)
{ std::stringstream out; std::string tempS;
  owner->TokenToStringStream(out, this->Operation);
  out <<" Value: ";
  this->Value.ElementToString(tempS, false, false, false);
  out << " " <<tempS<<" Children: ";
  for (int i=0; i<this->children.size; i++)
    out << this->children.TheObjects[i]<<", ";
  out <<"\n";
  output=out.str();
}


class IrreducibleFiniteDimensionalModule
{
  public:
  DyckPaths thePaths;//the Weyl group inside the Dyck paths is assumed to parametrize the module.
  MatrixLargeRational thePathMatrix;
  MatrixLargeRational theMatrix;
  roots theColumnsOfTheMatrix;
  void InitAndPrepareTheChambersForComputation(int IndexWeyl, CombinatorialChamberContainer& theChambers, GlobalVariables& theGlobalVariables);
  std::string DebugString;
  void ComputeDebugString(){this->ElementToString(this->DebugString);};
  void ElementToString(std::string& output);
};

void IrreducibleFiniteDimensionalModule::ElementToString(std::string& output)
{ std::stringstream out; std::string tempS;
  this->theMatrix.ElementToString(tempS);
  out <<"The matrix :\n "<< tempS<<"\n\n";
  this->thePathMatrix.ElementToString(tempS);
  out <<"The matrix of the polytope:\n "<< tempS<<"\n\n";
  this->thePaths.ElementToString(tempS, false);
  out << tempS;
  output = out.str();
}

void IrreducibleFiniteDimensionalModule::InitAndPrepareTheChambersForComputation(int IndexWeyl, CombinatorialChamberContainer& theChambers, GlobalVariables& theGlobalVariables)
{// reference:  E. Feigin, G. Fourier, P. Littelmann, "PBW filtration and bases for irreducible modules in type A_n", preprint 2009
  int theDimension= IndexWeyl;
  this->thePaths.AmbientWeyl.MakeAn(theDimension);
  this->thePaths.GenerateAllDyckPathsTypesABC();
  this->thePaths.ComputeGoodPathsExcludeTrivialPath();
  this->thePathMatrix.init( this->thePaths.GoodPaths.size, this->thePaths.PositiveRoots.size);
  this->thePathMatrix.NullifyAll(0);
  for (int i=0; i<this->thePaths.GoodPaths.size; i++)
    for (int j=0; j<this->thePaths.TheObjects[this->thePaths.GoodPaths.TheObjects[i]].thePathNodes.size; j++)
      this->thePathMatrix.elements[i][this->thePaths.TheObjects[this->thePaths.GoodPaths.TheObjects[i]].thePathNodes.TheObjects[j]]=1;
  this->theMatrix.Assign(this->thePathMatrix);
  this->theMatrix.Resize(this->thePaths.GoodPaths.size + theDimension, this->thePaths.PositiveRoots.size+this->thePaths.GoodPaths.size, true);
  for (int j=0; j<theDimension; j++)
  { for (int i=0; i<this->thePaths.PositiveRoots.size; i++)
      this->theMatrix.elements[j+this->thePaths.GoodPaths.size][i]= this->thePaths.PositiveRoots.TheObjects[i].TheObjects[j];
    for (int i=0; i<this->thePaths.GoodPaths.size; i++)
      this->theMatrix.elements[j+this->thePaths.GoodPaths.size][i+this->thePaths.PositiveRoots.size]=0;
  }
  for (int i=0; i<this->thePaths.GoodPaths.size; i++)
    for (int j=0; j<this->thePaths.GoodPaths.size; j++)
      this->theMatrix.elements[i][j+this->thePaths.PositiveRoots.size]=(i==j)? 1 : 0;
  this->theColumnsOfTheMatrix.AssignMatrixColumns(this->theMatrix);
  theChambers.theDirections.CopyFromBase(this->theColumnsOfTheMatrix);
  theChambers.AmbientDimension= this->theMatrix.NumRows;
  theChambers.CurrentIndex=theChambers.AmbientDimension-1;
  theChambers.theDirections.ComputeDebugString();

}

void main_test_function(std::string& output, GlobalVariables& theGlobalVariables, ComputationSetup& theSetup, bool flagComputationAlreadyLoaded)
{ std::stringstream out;
  std::string tempS;
  ElementWeylAlgebra theElement, tempEl1, tempEl2, tempEl3, tempEl4, tempEl5, Accum;
  //out <<"\\documentclass{article}\\usepackage{latexsym}\\usepackage{amssymb}\\usepackage{amsmath}\n";
  //out << "\\addtolength{\\hoffset}{-3.5cm}\\addtolength{\\textwidth}{6.8cm}\\addtolength{\\voffset}{-3.3cm}\\addtolength{\\textheight}{6.3cm}";
 // out <<"\\begin{document}";
/*  WeylParser theParser;
  theParser.ParseAndCompute("x_1+x_1", tempS);*/
  /*
  if (DebugCounter==-1)
    debugParser.ParserInit("[x_1x_2, x_3d_2]");
  else
    debugParser.ParseNoInit(DebugCounter, DebugCounter);
  DebugCounter++;
  debugParser.ComputeDebugString();
  out << debugParser.DebugString;
  out << "\n";
  if (DebugCounter> debugParser.TokenBuffer.size)
  { debugParser.ComputeNumberOfVariablesAndAdjustNodes();
    debugParser.Evaluate(theGlobalVariables);
  }
  WeylParser theParser;
  theParser.ParseAndCompute("[x_1x_2, x_3d_2]", tempS, theGlobalVariables);
  debugParser.Value.ComputeDebugString(false,false);
  out << debugParser.Value.DebugString;
  out << "\n\n"<<tempS;*/
  SimpleLieAlgebra theG;
  SltwoSubalgebras tempSl2s;
  theG.FindSl2Subalgebras(tempSl2s, 'A', 3, theGlobalVariables);
  tempSl2s.ComputeDebugString(theGlobalVariables, theG.theWeyl, false, false);
  IndicatorWindowGlobalVariables.StatusString1= tempSl2s.DebugString;
  IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.FeedIndicatorWindow(IndicatorWindowGlobalVariables);
  return;
  IrreducibleFiniteDimensionalModule theModule;
  QuasiPolynomial tempP;
  if (!flagComputationAlreadyLoaded)
    theModule.InitAndPrepareTheChambersForComputation(3, theSetup.theChambers,theGlobalVariables);
  theSetup.theChambers.flagMustStop=false;
  theSetup.theChambers.flagIsRunning=true;
  theSetup.theChambers.flagReachSafePointASAP=false;
  theSetup.flagExperiment2ChambersAlreadyLoaded=true;
  theSetup.theChambers.SliceTheEuclideanSpace(theGlobalVariables);
/*  tempEl1.Makexixj(0,1,4);
  Rational tempRat=-1;
  tempEl2.Makedidj(0,1,4);
  tempEl2.TimesConstant(tempRat);
  theElement.Assign(tempEl2);
  theElement.LieBracketOnTheLeftMakeReport(tempEl1, theGlobalVariables, tempS);
  out << tempS;
  theElement.LieBracketOnTheRightMakeReport(tempEl1, theGlobalVariables, tempS);
  out << tempS;
  theElement.Assign(tempEl2);
  theElement.LieBracketOnTheLeftMakeReport(tempEl1, theGlobalVariables, tempS);
  out << tempS;
  theElement.LieBracketOnTheRightMakeReport(tempEl2, theGlobalVariables, tempS);
  out << tempS;
  tempEl3.Makexixj(0,0,4);
  tempEl4.Makedidj(0,0,4);
  tempEl4.TimesConstant(tempRat);
  theElement.Assign(tempEl4);
  theElement.LieBracketOnTheLeftMakeReport(tempEl3, theGlobalVariables, tempS);
  out << tempS;
  theElement.LieBracketOnTheRightMakeReport(tempEl4, theGlobalVariables, tempS);
  out <<tempS;
  theElement.Assign(tempEl4);
  theElement.LieBracketOnTheLeftMakeReport(tempEl3, theGlobalVariables, tempS);
  out << tempS;
  theElement.LieBracketOnTheRightMakeReport(tempEl3, theGlobalVariables, tempS);
  out <<tempS;
  tempEl3.Makexidj(0,1,4);
//  tempEl3.TimesConstant(tempRat);
  tempEl4.Makexidj(1,0,4);
  theElement.Assign(tempEl3);
  theElement.LieBracketOnTheRightMakeReport(tempEl4, theGlobalVariables, tempS);
  out << tempS;*/
  /*tempEl1.MakeGEpsPlusEpsInTypeD(0,1, 4);
  tempEl2.MakeGEpsMinusEpsInTypeD(0,1,4);
  tempEl3.MakeGEpsPlusEpsInTypeD(2,3, 4);
  tempEl4.MakeGEpsMinusEpsInTypeD(2,3,4);
  tempEl5.MakeGMinusEpsMinusEpsInTypeD(0,2,4);
  tempEl1.ComputeDebugString(false);
  tempEl2.ComputeDebugString(false);
  tempEl3.ComputeDebugString(false);
  tempEl4.ComputeDebugString(false);
  tempEl5.ComputeDebugString(false);



  ElementWeylAlgebra theH, tempEl6;
  theH.MakeGEpsPlusEpsInTypeD(0,2,4);
  tempEl6.MakeGMinusEpsMinusEpsInTypeD(0,2,4);
  theH.LieBracketOnTheLeft(tempEl6, theGlobalVariables);
  theH.ComputeDebugString(false);
  out<<"\n\\begin{eqnarray*}&&h=";
  out <<theH.DebugString;
  out <<"\n\\end{eqnarray*}\n";
  tempEl6.Assign(theH);
  tempEl6.LieBracketOnTheLeft(theH, theGlobalVariables);
  tempEl6.ComputeDebugString(false);
  out<<"\n\\begin{eqnarray*}&&[h,h]=";
  out <<tempEl6.DebugString;
  out <<"\n\\end{eqnarray*}\n";

  out<<"\n\\begin{eqnarray*}&&-[h,g^{-\\varepsilon_1-\\varepsilon_3}]=\n\\\\";
  out<<"&&-["<<theH.DebugString<<","<<tempEl5.DebugString <<"]=\n\\\\";
  tempEl6.Assign(theH);
  tempEl6.LieBracketOnTheLeft(tempEl5, theGlobalVariables);
  tempEl6.ComputeDebugString(false);
  out <<tempEl6.DebugString;
  out <<"\n\\end{eqnarray*}\n";

  out<<"\n\\begin{eqnarray*}\n&&(g^{-\\varepsilon_1-\\varepsilon_3})^2 g ^{\\varepsilon_1+\\varepsilon_2}g ^{\\varepsilon_1-\\varepsilon_2}g ^{\\varepsilon_3+\\varepsilon_4}g ^{\\varepsilon_3-\\varepsilon_4}\\\\\n ";
  out <<"&&=("<<tempEl5.DebugString<<")^2("<<tempEl1.DebugString<< ")("<<tempEl2.DebugString <<")("
      << tempEl3.DebugString <<")("<< tempEl4.DebugString<<")=\\\\";
  Accum.Assign(tempEl4);
  Accum.MultiplyOnTheLeft(tempEl3, theGlobalVariables);
  Accum.ComputeDebugString(false);
  out <<"&&("<<tempEl5.DebugString<<")^2("<<tempEl1.DebugString<< ")("<<tempEl2.DebugString <<")("
      << Accum.DebugString<<")=";
  out <<"\\end{eqnarray*}\n";
  out<<"\n\\begin{eqnarray*}&&\n";
  Accum.MultiplyOnTheLeft(tempEl2, theGlobalVariables);
  Accum.ComputeDebugString(false);
  out <<"("<<tempEl5.DebugString<<")^2("<<tempEl1.DebugString<< ")("<< Accum.DebugString<<")=\\\\\n";
  out <<"\\end{eqnarray*}\n";
  out<<"\n\\begin{eqnarray*}&&\n";
  Accum.MultiplyOnTheLeft(tempEl1, theGlobalVariables);
  Accum.ComputeDebugString(false);
  out <<"("<<tempEl5.DebugString<<")^2("<< Accum.DebugString<<")=\\\\\n";
  out <<"\\end{eqnarray*}\n";
  Accum.MultiplyOnTheLeft(tempEl5, theGlobalVariables);
  Accum.MultiplyOnTheLeft(tempEl5, theGlobalVariables);
  Accum.ComputeDebugString(false);
  out<<"\n\\begin{eqnarray*}&&\n";
  out <<Accum.DebugString;
  out <<"\n\\end{eqnarray*}\n";



  out<<"\n\\begin{eqnarray*}&&[h,\\bullet]=";
  theH.LieBracketOnTheLeft(Accum, theGlobalVariables);
  theH.ComputeDebugString(false);
  out <<theH.DebugString;
  out <<"\n\\end{eqnarray*}\n";

  tempEl2.MakeGEpsMinusEpsInTypeD(0,1,4);
  tempEl2.ComputeDebugString(false);
  out <<"\\begin{eqnarray*}&&\ng^{\\varepsilon_1-\\varepsilon_2}= ";
  out <<tempEl2.DebugString;
  out <<"\n\\end{eqnarray*}\n";
  tempEl3.MakeGMinusEpsMinusEpsInTypeD(0,1,4);
  tempEl3.ComputeDebugString(false);
  out <<"\\begin{eqnarray*}&&\ng^{-\\varepsilon_1-\\varepsilon_2}= ";
  out <<tempEl3.DebugString;
  out <<"\n\\end{eqnarray*}\n";
  tempEl4.Assign(tempEl3);
  tempEl4.MultiplyOnTheLeft(tempEl1,theGlobalVariables);
  tempEl4.ComputeDebugString(false);
  out <<"\\begin{eqnarray*}&&\ng^{\\varepsilon_1+\\varepsilon_2}g^{-\\varepsilon_1-\\varepsilon_2}= ";
  out <<tempEl4.DebugString;
  out <<"\n\\end{eqnarray*}\n";

  tempEl4.Assign(tempEl3);
  tempEl4.LieBracketOnTheLeft(tempEl1,theGlobalVariables);
  tempEl4.ComputeDebugString(false);
  out <<"\\begin{eqnarray*}&&\n[g^{\\varepsilon_1+\\varepsilon_2},g^{-\\varepsilon_1-\\varepsilon_2}]= ";
  out <<tempEl4.DebugString;

  out <<"\n\\end{eqnarray*}\n";
*/

//  out<<"\\end{document}";
  output=out.str();
  IndicatorWindowGlobalVariables.StatusString1=output;
  IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.FeedIndicatorWindow(IndicatorWindowGlobalVariables);
}

void DyckPaths::ComputeGoodPathsExcludeTrivialPath()
{ int counter=0;
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].IamComplete(*this))
      counter++;
  this->GoodPaths.MakeActualSizeAtLeastExpandOnTop(counter);
  this->GoodPaths.size=0;
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].IamComplete(*this))
      this->GoodPaths.AddObjectOnTop(i);
}


void CombinatorialChamber::WriteToFile(std::fstream& output, GlobalVariables& theGlobalVariables)
{ output << "Flags_and_indices: ";
  output << this->flagHasZeroPolynomial << " "<< this->flagExplored<< " " <<this->flagPermanentlyZero <<" ";
  output << this->IndexInOwnerComplex << " "<< this->IndexStartingCrossSectionNormal <<" "<< this->DisplayNumber <<"\nVertices:\n";
  this->AllVertices.WriteToFile(output, theGlobalVariables);
  output <<"Internal_point: ";
  this->InternalPoint.WriteToFile(output);
  output << "\nInternalWalls:\n";
  this->InternalWalls.WriteToFile(output, theGlobalVariables);
  output<<"\n" << this->Externalwalls.size<<"\n";
  for (int i=0; i<this->Externalwalls.size; i++)
  { this->Externalwalls.TheObjects[i].WriteToFile(output);
    output<<" ";
  }
}

void CombinatorialChamber::ReadFromFile(std::fstream& input, GlobalVariables& theGlobalVariables, CombinatorialChamberContainer& owner)
{ std::string tempS;
  int tempI;
  input >> tempS;
  input >> this->flagHasZeroPolynomial >> this->flagExplored>>this->flagPermanentlyZero;
  input >> this->IndexInOwnerComplex >> this->IndexStartingCrossSectionNormal;
  input >> this->DisplayNumber;
  input >> tempS;
  this->AllVertices.ReadFromFile(input, theGlobalVariables);
  input>>tempS;
  this->InternalPoint.ReadFromFile(input);
  input >> tempS;
  assert(tempS =="InternalWalls:");
  this->InternalWalls.ReadFromFile(input, theGlobalVariables);
  input >> tempI;
  this->Externalwalls.SetSizeExpandOnTopNoObjectInit(tempI);
  for (int i=0; i<this->Externalwalls.size; i++)
    this->Externalwalls.TheObjects[i].ReadFromFile(input, owner);
}

void CombinatorialChamberContainer::WriteToFile(std::fstream& output, GlobalVariables& theGlobalVariables)
{ this->LabelChamberIndicesProperly();
  output << "Num_pointers: "<< this->size<<"\n";
///////////////////////////////////////////////////
  output << "Dimension: "<< this->AmbientDimension<<" ";
  output << "CurrentIndex: "<< this->CurrentIndex<<"\n";
  output << "Directions:\n";
  this->theDirections.WriteToFile(output, theGlobalVariables);
  output << "\nNext_index_to_slice: "<< this->indexNextChamberToSlice<<"\n";
  output << "FirstNonExploredIndex: " << this->FirstNonExploredIndex<<" ";
  this->TheGlobalConeNormals.WriteToFile(output, theGlobalVariables);
  output << "\n";
  this->startingCones.WriteToFile(output, theGlobalVariables);
////////////////////////////////////////////////////
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i]!=0)
    { output <<"\nChamber:\n";
      this->TheObjects[i]->WriteToFile(output, theGlobalVariables);
    } else
      output <<"Empty\n";
}

void CombinatorialChamberContainer::ReadFromFile(std::fstream& input, GlobalVariables& theGlobalVariables)
{ std::string tempS;
  input.seekg(0);
  this->KillAllElements();
  int tempI;
  input >> tempS >> tempI;
  this->initAndCreateNewObjects(tempI);
///////////////////////////////////////////////////
  input >> tempS >> this->AmbientDimension;
  input >> tempS >> this->CurrentIndex;
  input >> tempS;
  this->theDirections.ReadFromFile(input, theGlobalVariables);
  input >> tempS >> this->indexNextChamberToSlice;
  input >> tempS >> this->FirstNonExploredIndex;
  this->TheGlobalConeNormals.ReadFromFile(input, theGlobalVariables);
  this->startingCones.ReadFromFile(input, theGlobalVariables);
////////////////////////////////////////////////////
  for (int i=0; i<this->size; i++)
  { input >> tempS;
    if (tempS=="Chamber:")
      this->TheObjects[i]->ReadFromFile(input, theGlobalVariables, *this);
    else
    { assert (tempS=="Empty");
      delete this->TheObjects[i];
      this->TheObjects[i]=0;
    }
  }
  this->flagIsRunning=true;
  this->flagMustStop=false;
  this->flagReachSafePointASAP=false;
}

void WallData::WriteToFile(std::fstream& output)
{ //output << this->indexInOwnerChamber <<" ";
  this->normal.WriteToFile(output);
  output<< " "<< this->NeighborsAlongWall.size<<" ";
  assert(this->NeighborsAlongWall.size==this->IndicesMirrorWalls.size);
  for (int i=0; i<this->NeighborsAlongWall.size; i++ )
    if (this->NeighborsAlongWall.TheObjects[i]==0)
			output<< -1<<" "<<-1<<" ";
    else
    { //assert(this->MirrorWall.TheObjects[i]!=0);
      output << this->NeighborsAlongWall.TheObjects[i]->IndexInOwnerComplex << " " << this->IndicesMirrorWalls.TheObjects[i]<<" ";
    }
}

void WallData::ReadFromFile(std::fstream& input, CombinatorialChamberContainer& owner)
{ //input >>this->indexInOwnerChamber;
  this->normal.ReadFromFile(input);
  int tempI, indexN, indexW;
  input >>tempI;
  this->NeighborsAlongWall.SetSizeExpandOnTopNoObjectInit(tempI);
  this->IndicesMirrorWalls.SetSizeExpandOnTopNoObjectInit(tempI);
  for (int i=0; i<this->NeighborsAlongWall.size; i++ )
  { input >> indexN>> indexW;
    if (indexN==-1)
    { this->NeighborsAlongWall.TheObjects[i]=0;
      this->IndicesMirrorWalls.TheObjects[i]=-1;
    }
    else
    { this->NeighborsAlongWall.TheObjects[i]= owner.TheObjects[indexN];
      this->IndicesMirrorWalls.TheObjects[i]=indexW;
    }
  }
}

void CombinatorialChamberContainer::WriteToDefaultFile()
{ std::fstream tempF;
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(tempF, "./theChambers.txt", false, true, false);
  GlobalVariables theGlobalVariables;
  this->WriteToFile(tempF, theGlobalVariables);
  tempF.close();
}

void CombinatorialChamberContainer::ReadFromDefaultFile()
{ std::fstream tempF;
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(tempF, "./theChambers.txt", false, false, false);
  GlobalVariables theGlobalVariables;
  this->ReadFromFile(tempF, theGlobalVariables);
  tempF.close();
}

void CombinatorialChamberContainer::PauseSlicing()
{
#ifdef WIN32
/*  WaitForSingleObject(this->mutexFlagCriticalSectionEntered, INFINITE);
  this->flagReachSafePointASAP=true;
  ReleaseMutex(this->mutexFlagCriticalSectionEntered);
  while(!this->flagIsRunning)
  {}*/
#else
  pthread_mutex_lock(&this->mutexFlagCriticalSectionEntered);
  this->flagReachSafePointASAP=true;
  pthread_mutex_unlock(&this->mutexFlagCriticalSectionEntered);
  while (!this->flagIsRunning)
  {}
#endif
}

void CombinatorialChamberContainer::ResumeSlicing()
{
#ifdef WIN32
//  WaitForSingleObject(this->mutexFlagCriticalSectionEntered, INFINITE);
//  this->flagIsRunning=true;
//  SetEvent(this->condContinue);
//  ReleaseMutex(this->mutexFlagCriticalSectionEntered);
#else
  pthread_mutex_lock(&this->mutexFlagCriticalSectionEntered);
  this->flagIsRunning=true;
  pthread_cond_signal(&this->condContinue);
  pthread_mutex_unlock(&this->mutexFlagCriticalSectionEntered);
#endif
}

void CombinatorialChamberContainer::SliceTheEuclideanSpace(GlobalVariables& theGlobalVariables)
{ this->SliceTheEuclideanSpace(this->theDirections, this->CurrentIndex, this->AmbientDimension, 0, theGlobalVariables);
}

void Cone::WriteToFile(std::fstream& output, GlobalVariables& theGlobalVariables)
{ this->roots::WriteToFile(output, theGlobalVariables);
  output <<"\nChamberTestArray: "<<this->ChamberTestArray.size<<" ";
  for (int i=0; i<this->ChamberTestArray.size; i++)
    output<< this->ChamberTestArray.TheObjects[i]<<" ";
}

void Cone::ReadFromFile(std::fstream& input, GlobalVariables& theGlobalVariables)
{ std::string tempS; int tempI;
  this->roots::ReadFromFile(input, theGlobalVariables);
  input >> tempS>> tempI;
  this->ChamberTestArray.SetSizeExpandOnTopNoObjectInit(tempI);
  for (int i=0; i<this->ChamberTestArray.size; i++)
    input >> this->ChamberTestArray.TheObjects[i];
}

void simplicialCones::WriteToFile(std::fstream& output, GlobalVariables& theGlobalVariables)
{ this->theFacets.WriteToFile(output);
  output <<"\nConesHavingFixedNormal: ";
  output << this->ConesHavingFixedNormal.size<<" ";
  for (int i=0; i<this->ConesHavingFixedNormal.size; i++)
  { output << this->ConesHavingFixedNormal.TheObjects[i].size<<" ";
    for (int j=0; j<this->ConesHavingFixedNormal.TheObjects[i].size; j++)
      output<< this->ConesHavingFixedNormal.TheObjects[i].TheObjects[j]<<" ";
  }
  output <<"\ntheCones: "<<this->size<< " ";
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].WriteToFile(output, theGlobalVariables);
    output <<" ";
  }
}

void simplicialCones::ReadFromFile(std::fstream& input, GlobalVariables& theGlobalVariables)
{ std::string tempS; int tempI;
  this->theFacets.ReadFromFile(input);
  input >>tempS;
  assert(tempS=="ConesHavingFixedNormal:");
  input >> tempI;
  this->ConesHavingFixedNormal.SetSizeExpandOnTopNoObjectInit(tempI);
  for (int i=0; i<this->ConesHavingFixedNormal.size; i++)
  { input >> tempI;
    this->ConesHavingFixedNormal.TheObjects[i].SetSizeExpandOnTopNoObjectInit(tempI);
    for (int j=0; j<this->ConesHavingFixedNormal.TheObjects[i].size; j++)
      input>>this->ConesHavingFixedNormal.TheObjects[i].TheObjects[j];
   }
  input>>tempS>>tempI;
  assert(tempS=="theCones:");
  this->SetSizeExpandOnTopNoObjectInit(tempI);
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].ReadFromFile(input, theGlobalVariables);
}

void hashedRoots::WriteToFile(std::fstream& output)
{ int theDimension=0;
  if (this->size>0)
    theDimension= this->TheObjects[0].size;
  output << "Num|dim: " <<this->size<<" "<< theDimension<<" ";
  for (int i=0; i<this->size; i++)
    for (int j=0; j<theDimension; j++)
    { this->TheObjects[i].TheObjects[j].WriteToFile(output);
      output<<" ";
    }
}

void hashedRoots::ReadFromFile(std::fstream& input)
{ int theDimension; std::string tempS;
  int theSize;
  this->ClearTheObjects();
  input >> tempS >> theSize>> theDimension;
  this->MakeActualSizeAtLeastExpandOnTop(theSize);
  root tempRoot;
  tempRoot.SetSizeExpandOnTopLight(theDimension);
  for (int i=0; i<theSize; i++)
  { for (int j=0; j<theDimension; j++)
      tempRoot.TheObjects[j].ReadFromFile(input);
    this->AddObjectOnTopHash(tempRoot);
   }
}

void slTwo::ElementToHtmlCreateFormulaOutputReference(const std::string& formulaTex, std::stringstream& output, bool usePNG, bool useHtml, SltwoSubalgebras& container, std::string* physicalPath, std::string* htmlPathServer)
{ if (!usePNG)
  { output<<formulaTex;
    //if (useHtml)
      //output<<"\n<br>\n";
    return;
  }
  std::stringstream tempStream;
  container.texFileNamesForPNG.SetSizeExpandOnTopNoObjectInit(container.texFileNamesForPNG.size+1);
  container.texStringsEachFile.SetSizeExpandOnTopNoObjectInit(container.texFileNamesForPNG.size);
  (*container.texStringsEachFile.LastObject())=formulaTex;
  tempStream<<(*physicalPath) <<"fla";
  tempStream<<container.texFileNamesForPNG.size<<".tex";
  container.texFileNamesForPNG.TheObjects[container.texFileNamesForPNG.size-1]=tempStream.str();
  output <<"<img src=\""<< (*htmlPathServer) <<"fla"<<container.texFileNamesForPNG.size<<".png\">";
  if (useHtml)
    output<<"\n<br>\n";
}

void slTwo::ElementToString(std::string& output, GlobalVariables& theGlobalVariables, SltwoSubalgebras& container, int indexInContainer, bool useLatex, bool useHtml, bool usePNG, std::string* physicalPath, std::string* htmlPathServer)
{ std::stringstream out;  std::string tempS;
  this->hCharacteristic.ElementToString(tempS);
  out <<"<a name=\"sl2index" << indexInContainer<< "\">h-characteristic: "<<  tempS <<"</a>";
  this->preferredAmbientSimpleBasis.ElementToString(tempS, false, false, false);
  if (physicalPath==0 || htmlPathServer==0)
  { usePNG=false;
    useHtml=false;
  }
  if (useHtml)
    out << "<br>";
  out<<"\nSimple basis ambient algebra w.r.t defining h: "<< tempS;
  roots tempRoots;
  MatrixLargeRational tempMat;
  if (useHtml)
    out << "<br>";
  if (this->IndicesContainingRootSAs.size>1)
  { out <<"Number of containing regular subalgebras: " << this->IndicesContainingRootSAs.size;
    if (useHtml)
    out << "<br>";
  }
  for (int i=0; i<this->IndicesContainingRootSAs.size; i++)
  { out << "\nContaining regular subalgebra number " << i+1 << ": ";
    rootSubalgebra& currentSA= container.theRootSAs.TheObjects[this->IndicesContainingRootSAs.TheObjects[i]];
    if (useHtml)
    { out << "<a href=\"" << (*htmlPathServer) << "../rootHtml_rootSA" << this->IndicesContainingRootSAs.TheObjects[i] << ".html\">";
      currentSA.theDynkinDiagram.ElementToString(tempS);
      CGIspecificRoutines::clearDollarSigns(tempS, tempS);
    }
    currentSA.theDynkinDiagram.GetSimpleBasisInBourbakiOrder(tempRoots);
    out << tempS;
    if(useHtml)
      out << "</a>";
    tempRoots.ElementToString(tempS, useLatex, useHtml, false);
    if (useHtml)
      out << "<br>";
    out << "\nSimple basis subalgebra: " << tempS;
    currentSA.theDynkinDiagram.GetKillingFormMatrixUseBourbakiOrder(tempMat, this->owner->theWeyl);
    if (!usePNG)
      tempMat.ElementToString(tempS, useHtml, useLatex);
    else
      tempMat.ElementToString(tempS, false, true);
    if (useHtml)
      out << "<br>";
    out << "\nSymmetric Cartan matrix in Bourbaki order:\n";
    if (useHtml)
    { out << "<br>";
      this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
    } else
      out <<tempS;
  }
  out <<"\nsl(2)-module decomposition of the ambient Lie algebra: ";
  std::stringstream tempStream2;
  if (useLatex || usePNG)
    tempStream2<<"$";
  for (int i=0; i<this->HighestWeights.size; i++)
  { if (this->MultiplicitiesHighestWeights.TheObjects[i]>1)
      tempStream2<< this->MultiplicitiesHighestWeights.TheObjects[i];
    tempStream2 << "V_{"<<this->HighestWeights.TheObjects[i]<<"}";
    if (i!=this->HighestWeights.size-1)
      tempStream2 <<"+";
  }
  if (useLatex || usePNG)
    tempStream2<<"$";
  tempS= tempStream2.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  container.IndicesSl2decompositionFlas.SetSizeExpandOnTopNoObjectInit(container.size);
  container.IndicesSl2decompositionFlas.TheObjects[indexInContainer]=container.texFileNamesForPNG.size-1;
  out <<"\nThe below list one possible realization of the sl(2) subalgebra.";
  if (useHtml)
    out <<"\n<br>\n";
  std::stringstream tempStreamH, tempStreamE, tempStreamF, tempStreamHE, tempStreamHF, tempStreamEF;
  this->theH.ElementToString(tempS, *this->owner, useHtml, useLatex);
  tempStreamH<<"\n$h=$ $" <<tempS<<"$";
  tempS= tempStreamH.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  this->theE.ElementToString(tempS, *this->owner, useHtml, useLatex);
  tempStreamE<<"\n$e=$ $" <<tempS<<"$";
  tempS= tempStreamE.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  this->theF.ElementToString(tempS, *this->owner, useHtml, useLatex);
  tempStreamF <<"\n$f=$ $" <<tempS<<"$";
  tempS= tempStreamF.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS,out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  out <<"\n\nThe below are the Lie brackets of the above elements. Printed for debugging.";
  if (useHtml)
    out <<"\n<br>\n";
  this->bufferEbracketF.ElementToString(tempS, *this->owner, useHtml, useLatex);
  tempStreamEF<< "\n$[e,f]=$ $" <<tempS <<"$";
  tempS= tempStreamEF.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  this->bufferHbracketE.ElementToString(tempS, *this->owner, useHtml, useLatex);
  tempStreamHE<< "\n$[h,e]=$ $" <<tempS <<"$";
  tempS= tempStreamHE.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  this->bufferHbracketF.ElementToString(tempS, *this->owner, useHtml, useLatex);
  tempStreamHF<< "\n$[h,f]=$ $" <<tempS <<"$";
  tempS= tempStreamHF.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  //this->theSystemMatrixForm.ElementToString(tempS);
  //out <<"\nSystem matrix form we try to solve:\n"<< tempS;
  //this->theSystemColumnVector.ElementToString(tempS);
  //out <<"\nColumn vector of the system:\n"<<tempS;
  std::stringstream tempStreamActual;
  for (int i=0; i<this->theSystemToBeSolved.size; i++)
  { this->theSystemToBeSolved.TheObjects[i].ElementToString(tempS);
    if (tempS=="")
    { if (useLatex || usePNG)
        tempStreamActual<<"~\\\\";
      else
        tempStreamActual<<"\n\n";
    }
    else
      tempStreamActual<<"\\noindent \\begin{eqnarray*}&&"<< tempS<<"=0\\end{eqnarray*}\n\n";
  }
  out << "\nThe system we need to solve:\n";
  if (useHtml)
    out <<"\n<br>\n";
  tempS= tempStreamActual.str();
  this->ElementToHtmlCreateFormulaOutputReference(tempS, out, usePNG, useHtml, container, physicalPath, htmlPathServer);
  output = out.str();
}

void slTwo::ElementToHtml(std::string& filePath)
{ std::fstream theFile;
  std::string theFileName=filePath;
  theFileName.append("theSlTwo.txt");
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(theFile, filePath, false, true, false);
}

void SimpleLieAlgebra::FindSl2Subalgebras(SltwoSubalgebras& output, char WeylLetter, int WeylRank, GlobalVariables& theGlobalVariables)
{ output.theRootSAs.GenerateAllReductiveRootSubalgebrasUpToIsomorphism(theGlobalVariables, WeylLetter, WeylRank, true, true);
  //output.theRootSAs.ComputeDebugString(false, false, false, 0, 0, theGlobalVariables);
  output.IndicesSl2sContainedInRootSA.SetSizeExpandOnTopNoObjectInit(output.theRootSAs.size);
  output.IndicesSl2sContainedInRootSA.MakeActualSizeAtLeastExpandOnTop(output.theRootSAs.size*2);
  for (int i=0; i<output.IndicesSl2sContainedInRootSA.size; i++)
    output.IndicesSl2sContainedInRootSA.TheObjects[i].size=0;
  IndicatorWindowGlobalVariables.StatusString1=output.theRootSAs.DebugString;
  IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.FeedIndicatorWindow(IndicatorWindowGlobalVariables);
  for (int i=0; i<output.theRootSAs.size-1; i++)
    output.theRootSAs.TheObjects[i].GetSsl2SubalgebrasAppendListNoRepetition(output, i, theGlobalVariables, *this);
  for (int i=0; i<output.size; i++)
  { slTwo& theSl2= output.TheObjects[i];
    theSl2.IndicesMinimalContainingRootSA.MakeActualSizeAtLeastExpandOnTop(theSl2.IndicesContainingRootSAs.size);
    theSl2.IndicesMinimalContainingRootSA.size=0;
    for (int j=0; j<theSl2.IndicesContainingRootSAs.size; j++)
    { bool isMinimalContaining=true;
//      rootSubalgebra& currentRootSA = output.theRootSAs.TheObjects[];
      for (int k=0; k<theSl2.IndicesContainingRootSAs.size; k++)
      { rootSubalgebra& theOtherRootSA = output.theRootSAs.TheObjects[theSl2.IndicesContainingRootSAs.TheObjects[k]];
        if (theOtherRootSA.indicesSubalgebrasContainingK.ContainsObject(theSl2.IndicesContainingRootSAs.TheObjects[j]))
        { isMinimalContaining=false;
          break;
        }
      }
      if (isMinimalContaining)
        theSl2.IndicesMinimalContainingRootSA.AddObjectOnTopNoRepetitionOfObject(theSl2.IndicesContainingRootSAs.TheObjects[j]);
    }
  }

  //tempRootSA.GetSsl2Subalgebras(tempSl2s, theGlobalVariables, *this);
}

void rootSubalgebra::GetSsl2SubalgebrasAppendListNoRepetition(SltwoSubalgebras& output, int indexInContainer, GlobalVariables& theGlobalVariables, SimpleLieAlgebra& theLieAlgebra)
{ //reference: Dynkin, semisimple Lie algebras of simple lie algebras, theorems 10.1-10.4
  Selection theRootsWithZeroCharacteristic;
  roots RootsWithCharacteristic2;
  RootsWithCharacteristic2.MakeActualSizeAtLeastExpandOnTop(this->PositiveRootsK.size);
  DynkinDiagramRootSubalgebra tempDiagram;
  int theRelativeDimension = this->SimpleBasisK.size;
  theRootsWithZeroCharacteristic.init(theRelativeDimension);
  MatrixLargeRational InvertedRelativeKillingForm;
  InvertedRelativeKillingForm.init(theRelativeDimension, theRelativeDimension);
  for (int k=0; k<theRelativeDimension; k++)
    for (int j=0; j<theRelativeDimension; j++)
      InvertedRelativeKillingForm.elements[k][j]=this->AmbientWeyl.RootScalarKillingFormMatrixRoot(this->SimpleBasisK.TheObjects[k], this->SimpleBasisK.TheObjects[j]);
  InvertedRelativeKillingForm.Invert(theGlobalVariables);
  theLieAlgebra.ComputeChevalleyConstants(this->AmbientWeyl.WeylLetter, this->AmbientWeyl.KillingFormMatrix.NumRows, theGlobalVariables);
  //int relativeDimension =
  int numCycles= MathRoutines::TwoToTheNth(theRootsWithZeroCharacteristic.MaxSize);
  roots tempRoots;
  tempRoots.MakeActualSizeAtLeastExpandOnTop(theRootsWithZeroCharacteristic.MaxSize);
  roots relativeRootSystem;
  this->PositiveRootsK.GetCoordsInBasis(this->SimpleBasisK, relativeRootSystem, theGlobalVariables);
  slTwo theSl2;
  theSl2.owner = &theLieAlgebra;
  for (int i=0; i<numCycles; i++, theRootsWithZeroCharacteristic.incrementSelection())
  { tempRoots.size=0;
    for (int j=0; j<theRootsWithZeroCharacteristic.CardinalitySelection; j++)
      tempRoots.AddObjectOnTop(this->SimpleBasisK.TheObjects[theRootsWithZeroCharacteristic.elements[j]]);
    tempDiagram.ComputeDiagramTypeModifyInput(tempRoots, this->AmbientWeyl);
    int theSlack=0;
    RootsWithCharacteristic2.size=0;
    for (int j=0; j<relativeRootSystem.size; j++)
    { Rational DimTwoSpace=0;
      for (int k=0; k<theRelativeDimension; k++)
        if (!theRootsWithZeroCharacteristic.selected[k])
        { DimTwoSpace+=relativeRootSystem.TheObjects[j].TheObjects[k];
          if (DimTwoSpace>1)
            break;
        }
      if (DimTwoSpace==1)
      { theSlack++;
        RootsWithCharacteristic2.AddObjectOnTop(this->PositiveRootsK.TheObjects[j]);
      }
    }
    int theDynkinEpsilon = tempDiagram.NumRootsGeneratedByDiagram() + theRelativeDimension - theSlack;
    //if Dynkin's epsilon is not zero the subalgebra cannot be an S sl(2) subalgebra.
    //otherwise, as far as I understand, it always is (but generators still have to be found)
    //this is done in the below code.
    theRootsWithZeroCharacteristic.ComputeDebugString();
    if (theDynkinEpsilon==0)
    { root tempRoot, tempRoot2;
      tempRoot.MakeZero(theRelativeDimension);
      for (int k=0; k<theRelativeDimension; k++)
        if(!theRootsWithZeroCharacteristic.selected[k])
          tempRoot.TheObjects[k]=2;
      InvertedRelativeKillingForm.ActOnAroot(tempRoot, tempRoot2);
      theSl2.theH.Nullify(theLieAlgebra);
      theSl2.theH.Hcomponent.MakeZero(this->AmbientWeyl.KillingFormMatrix.NumRows);
      for(int j=0; j<theRelativeDimension; j++)
        theSl2.theH.Hcomponent+= this->SimpleBasisK.TheObjects[j]*tempRoot2.TheObjects[j];
      theSl2.theE.Nullify(theLieAlgebra);
      theSl2.theF.Nullify(theLieAlgebra);
      //theSl2.ComputeDebugString(false, false, theGlobalVariables);
      if(theLieAlgebra.AttemptExtendingHEtoHEFWRTSubalgebra(RootsWithCharacteristic2, relativeRootSystem, theRootsWithZeroCharacteristic, this->SimpleBasisK, theSl2.theH.Hcomponent, theSl2.theE, theSl2.theF, theSl2.theSystemMatrixForm, theSl2.theSystemToBeSolved, theSl2.theSystemColumnVector, theGlobalVariables))
      { int indexIsoSl2;
        theSl2.MakeReportPrecomputations(theGlobalVariables, output, output.size, indexInContainer, *this);
        if(output.ContainsSl2WithGivenHCharacteristic(theSl2.hCharacteristic, &indexIsoSl2))
        { output.TheObjects[indexIsoSl2].IndicesContainingRootSAs.AddObjectOnTop(indexInContainer);
          output.IndicesSl2sContainedInRootSA.TheObjects[indexInContainer].AddObjectOnTop(indexIsoSl2);
        }
        else
        { output.IndicesSl2sContainedInRootSA.TheObjects[indexInContainer].AddObjectOnTop(output.size);
          output.AddObjectOnTopHash(theSl2);
        }
      }
      else
        output.BadHCharacteristics.AddObjectOnTop(theSl2.theH.Hcomponent);
    }
  }
}

bool SimpleLieAlgebra:: AttemptExtendingHEtoHEFWRTSubalgebra(roots& RootsWithCharacteristic2, roots& relativeRootSystem, Selection& theZeroCharacteristics, roots& simpleBasisSA, root& h, ElementSimpleLieAlgebra& outputE, ElementSimpleLieAlgebra& outputF, MatrixLargeRational& outputMatrixSystemToBeSolved, PolynomialsRationalCoeff& outputSystemToBeSolved, MatrixLargeRational& outputSystemColumnVector, GlobalVariables& theGlobalVariables)
{ if (theZeroCharacteristics.CardinalitySelection== theZeroCharacteristics.MaxSize)
    return false;
  roots SelectedExtraPositiveRoots;
  roots rootsInPlay;
  rootsInPlay.size=0;
  SelectedExtraPositiveRoots.size=0;
  int theRelativeDimension = simpleBasisSA.size;
//  int theDimension= this->theWeyl.KillingFormMatrix.NumRows;
  assert(theRelativeDimension==theZeroCharacteristics.MaxSize);
  root tempRoot, tempRoot2;
  //format. We are looking for an sl(2) for which e= a_0 g^\alpha_0+\dots a_kg^\alpha_k, and f=b_0 g^{-\alpha_0}+... +b_kg^{-\alpha_k}
  //where the first \alpha's are ordered as in rootsInPlay.
  //Those are ordered as such: first come  the simple roots of characteristic 2, and the last \alpha's are the members of SelectedExtraPositiveRoots
  //(i.e. roots equal to the sum of one simple root of characteristic 2 a simple root of characteristic 0).
  // Then the first k variables of the polynomials below will be a_0,..., a_k., and the last k variables will be the b_i's
  // the l^th polynomial will correspond to the coefficient of g^\alpha_{l/2}, where l/2 is the index of the root of SelectedExtraPositiveRoots, if l is even, and to the
  // coefficient of  g^{-\alpha_{(l+1)/2}} otherwise
  for (int i=0; i<theRelativeDimension; i++)
    if (!theZeroCharacteristics.selected[i])
      rootsInPlay.AddObjectOnTop(simpleBasisSA.TheObjects[i]);
    else
      for (int j=0; j<theRelativeDimension; j++)
        if (!theZeroCharacteristics.selected[j])
        { tempRoot= simpleBasisSA.TheObjects[i]+simpleBasisSA.TheObjects[j];
          if (this->theWeyl.IsARoot(tempRoot))
            SelectedExtraPositiveRoots.AddObjectOnTop(tempRoot);
        }
  SelectedExtraPositiveRoots.size=0;
  for (int i=0; i<RootsWithCharacteristic2.size; i++)
    if (!simpleBasisSA.ContainsObject(RootsWithCharacteristic2.TheObjects[i]))
      SelectedExtraPositiveRoots.AddObjectOnTop(RootsWithCharacteristic2.TheObjects[i]);
  int numRootsChar2 = rootsInPlay.size;
  rootsInPlay.AddListOnTop(SelectedExtraPositiveRoots);
  int halfNumberVariables = rootsInPlay.size;
  int numberVariables = halfNumberVariables*2;
  Monomial<Rational> tempM;
  tempM.init((short)numberVariables);
  MatrixLargeRational coeffsF;
  coeffsF.init(1, halfNumberVariables);
  for (int i=0; i<numRootsChar2; i++)
    coeffsF.elements[0][i]=i+1;//(i%2==0)? 1: 2;
  for (int i=numRootsChar2; i<coeffsF.NumCols; i++)
    coeffsF.elements[0][i]=i+1;
  this->initHEFSystemFromECoeffs(theRelativeDimension, theZeroCharacteristics, rootsInPlay, simpleBasisSA, SelectedExtraPositiveRoots, numberVariables, numRootsChar2, halfNumberVariables, h, coeffsF, outputMatrixSystemToBeSolved, outputSystemColumnVector, outputSystemToBeSolved);
  MatrixLargeRational tempMat, tempMatColumn, tempMatResult;
  tempMat.Assign(outputMatrixSystemToBeSolved);
  tempMatColumn.Assign(outputSystemColumnVector);
  outputF.Nullify(*this);
  outputE.Nullify(*this);
//  if(MatrixLargeRational::Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(outputMatrixSystemToBeSolved, outputSystemColumnVector, tempMatResult))
  if(MatrixLargeRational::Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(tempMat, tempMatColumn, tempMatResult))
  { for (int i=0; i<rootsInPlay.size; i++)
    { outputF.SetCoefficient(-rootsInPlay.TheObjects[i], coeffsF.elements[0][i], *this);
      outputE.SetCoefficient(rootsInPlay.TheObjects[i], tempMatResult.elements[i][0], *this);
    }
    return true;
  }
  return true;
}

void SimpleLieAlgebra::initHEFSystemFromECoeffs(int theRelativeDimension, Selection& theZeroCharacteristics, roots& rootsInPlay, roots& simpleBasisSA, roots& SelectedExtraPositiveRoots, int numberVariables, int numRootsChar2, int halfNumberVariables, root& targetH, MatrixLargeRational& inputFCoeffs, MatrixLargeRational& outputMatrixSystemToBeSolved, MatrixLargeRational& outputSystemColumnVector, PolynomialsRationalCoeff& outputSystemToBeSolved)
{ root tempRoot;
  static int ProblemCounter=0;
  ProblemCounter++;
  Monomial<Rational> tempM;
  Rational tempRat;
  hashedRoots RootSpacesThatNeedToBeKilled;
  RootSpacesThatNeedToBeKilled.ClearTheObjects();
  //ListBasicObjects<int> IndicesEquationsByRootSpace;
  RootSpacesThatNeedToBeKilled.MakeActualSizeAtLeastExpandOnTop(this->theWeyl.RootSystem.size);
//  IndicesEquationsByRootSpace.MakeActualSizeAtLeastExpandOnTop(this->theWeyl.RootSystem.size);
  outputSystemToBeSolved.size=0;
  outputMatrixSystemToBeSolved.init(0, numberVariables);
  //outputSystemToBeSolved.ComputeDebugString();
  for (int i=0; i<rootsInPlay.size; i++)
    for (int j=0; j<rootsInPlay.size; j++)
    { tempRoot= rootsInPlay.TheObjects[i]-rootsInPlay.TheObjects[j];
      if (this->theWeyl.IsARoot(tempRoot))
      { int indexEquation= RootSpacesThatNeedToBeKilled.IndexOfObjectHash(tempRoot);
        if (indexEquation==-1)
        { RootSpacesThatNeedToBeKilled.AddObjectOnTopHash(tempRoot);
          indexEquation=outputSystemToBeSolved.size;
//          IndicesEquationsByRootSpace.AddObjectOnTop(indexEquation);
          outputSystemToBeSolved.SetSizeExpandOnTopNoObjectInit(outputSystemToBeSolved.size+1);
          outputSystemToBeSolved.LastObject()->Nullify((short)numberVariables);
        }
        tempM.init((short)numberVariables);
        tempM.degrees[i]=1;
        tempM.degrees[j+halfNumberVariables]=1;
        tempM.Coefficient= this->GetConstant(rootsInPlay.TheObjects[i], -rootsInPlay.TheObjects[j]);
        outputSystemToBeSolved.TheObjects[indexEquation].AddMonomial(tempM);
        //outputSystemToBeSolved.ComputeDebugString();
      }
    }
  int oldSize=outputSystemToBeSolved.size;
  outputSystemToBeSolved.SetSizeExpandOnTopNoObjectInit(oldSize+this->theWeyl.KillingFormMatrix.NumRows);
  for(int i=oldSize; i<outputSystemToBeSolved.size; i++)
    outputSystemToBeSolved.TheObjects[i].Nullify((short)numberVariables);
  //outputSystemToBeSolved.ComputeDebugString();
  for (int i=0; i<rootsInPlay.size; i++)
  { assert(rootsInPlay.size==halfNumberVariables);
    this->GetConstantOrHElement(rootsInPlay.TheObjects[i], -rootsInPlay.TheObjects[i], tempRat, tempRoot);
    for (int j=0; j<this->theWeyl.KillingFormMatrix.NumRows; j++)
    { tempM.init((short)numberVariables);
      tempM.degrees[i]=1;
      tempM.degrees[i+halfNumberVariables]=1;
      tempM.Coefficient= tempRoot.TheObjects[j];
      outputSystemToBeSolved.TheObjects[j+oldSize].AddMonomial(tempM);
    }
  }
  for (int i=0; i<this->theWeyl.KillingFormMatrix.NumRows; i++)
    outputSystemToBeSolved.TheObjects[i+oldSize].AddConstant(targetH.TheObjects[i]*(-1));
  outputMatrixSystemToBeSolved.init(outputSystemToBeSolved.size, halfNumberVariables);
  outputSystemColumnVector.init(outputSystemToBeSolved.size, 1);
  outputMatrixSystemToBeSolved.NullifyAll();
  outputSystemColumnVector.NullifyAll();
  for (int i=0; i<outputSystemToBeSolved.size; i++)
    for (int j=0; j<outputSystemToBeSolved.TheObjects[i].size; j++)
    { int lowerIndex=-1; int higherIndex=-1;
      Monomial<Rational>& theMonomial= outputSystemToBeSolved.TheObjects[i].TheObjects[j];
      for (int k=0; k<numberVariables; k++)
        if (theMonomial.degrees[k]==1)
        { if (k<halfNumberVariables)
            lowerIndex=k;
          else
          { higherIndex=k;
            break;
          }
        }
      if (lowerIndex==-1)
        outputSystemColumnVector.elements[i][0]= theMonomial.Coefficient*(-1);
      else
        outputMatrixSystemToBeSolved.elements[i][lowerIndex]=theMonomial.Coefficient* inputFCoeffs.elements[0][higherIndex-halfNumberVariables];
    }
//  outputSystemToBeSolved.ComputeDebugString();
}

bool SltwoSubalgebras::ContainsSl2WithGivenH(root& theH, int* outputIndex)
{ if (outputIndex!=0)
    *outputIndex=-1;
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].theH.Hcomponent==theH)
    { if (outputIndex!=0)
        *outputIndex=i;
      return true;
    }
  return false;
}

bool SltwoSubalgebras::ContainsSl2WithGivenHCharacteristic(root& theHCharacteristic, int* outputIndex)
{ if (outputIndex!=0)
    *outputIndex=-1;
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].hCharacteristic==theHCharacteristic)
    { if (outputIndex!=0)
        *outputIndex=i;
      return true;
    }
  return false;
}

void slTwo::MakeReportPrecomputations(GlobalVariables& theGlobalVariables, SltwoSubalgebras& container, int indexInContainer, int indexMinimalContainingRegularSA, rootSubalgebra& MinimalContainingRegularSubalgebra)
{ int theDimension=this->owner->theWeyl.KillingFormMatrix.NumRows;
  this->IndicesContainingRootSAs.size=0;
  roots tempRoots;
  tempRoots.CopyFromBase(MinimalContainingRegularSubalgebra.SimpleBasisK);
  this->owner->theWeyl.TransformToSimpleBasisGeneratorsWRTh(tempRoots, this->theH.Hcomponent);
  DynkinDiagramRootSubalgebra theDiagram;
  theDiagram.ComputeDiagramTypeKeepInput(tempRoots, this->owner->theWeyl);
  theDiagram.GetSimpleBasisInBourbakiOrder(tempRoots);
  this->IndicesContainingRootSAs.AddObjectOnTop(indexMinimalContainingRegularSA);
  tempRoots.MakeEiBasis(theDimension);
  this->owner->theWeyl.TransformToSimpleBasisGeneratorsWRTh(tempRoots, this->theH.Hcomponent);
  DynkinDiagramRootSubalgebra tempDiagram;
  tempDiagram.ComputeDiagramTypeKeepInput(tempRoots, this->owner->theWeyl);
  tempDiagram.GetSimpleBasisInBourbakiOrder(this->preferredAmbientSimpleBasis);
  this->hCharacteristic.SetSizeExpandOnTopLight(theDimension);
  for (int i=0; i<theDimension; i++)
    this->hCharacteristic.TheObjects[i]=this->owner->theWeyl.RootScalarKillingFormMatrixRoot(this->theH.Hcomponent, this->preferredAmbientSimpleBasis.TheObjects[i]);
  //this->hCharacteristic.ComputeDebugString();
  if (this->theE.NonZeroElements.MaxSize==this->owner->theWeyl.RootSystem.size && this->theF.NonZeroElements.MaxSize==this->owner->theWeyl.RootSystem.size && this->theH.NonZeroElements.MaxSize==this->owner->theWeyl.RootSystem.size)
  { this->owner->LieBracket(this->theE, this->theF, this->bufferEbracketF);
    this->owner->LieBracket(this->theH, this->theE, this->bufferHbracketE);
    this->owner->LieBracket(this->theH, this->theF, this->bufferHbracketF);
  }
  //theSl2.hCharacteristic.ComputeDebugString();
  this->ComputeModuleDecomposition();
}

void WeylGroup::PerturbWeightToRegularWRTrootSystem(const root& inputH, root& output)
{ output.Assign(inputH);
  int indexFirstNonRegular;
  while(!this->IsRegular(output, &indexFirstNonRegular))
  { root& theBadRoot= this->RootSystem.TheObjects[indexFirstNonRegular];
    Rational maxMovement=0; Rational tempRat1, tempRat2, tempMaxMovement;
    for (int i=0; i<this->RootsOfBorel.size; i++)
    { this->RootScalarKillingFormMatrixRoot(theBadRoot, this->RootsOfBorel.TheObjects[i], tempRat1);
      this->RootScalarKillingFormMatrixRoot(output, this->RootsOfBorel.TheObjects[i], tempRat2);
      if ((!tempRat1.IsEqualToZero()) && (!tempRat2.IsEqualToZero()))
      { tempMaxMovement = tempRat2/tempRat1;
        tempMaxMovement.AssignAbsoluteValue();
        if ((tempMaxMovement< maxMovement) || maxMovement.IsEqualToZero())
          maxMovement = tempMaxMovement;
      }
    }
    int tempInt=2;
    if (this->RootScalarKillingFormMatrixRoot(theBadRoot, inputH).IsNegative())
      tempInt=-2;
    output+=theBadRoot*maxMovement/tempInt;
  }
}

bool WeylGroup::IsRegular(root& input, int* indexFirstPerpendicularRoot)
{ if (indexFirstPerpendicularRoot!=0)
    *indexFirstPerpendicularRoot=-1;
  for (int i=0; i<this->RootSystem.size; i++)
    if (this->RootScalarKillingFormMatrixRoot(input, this->RootSystem.TheObjects[i]).IsEqualToZero())
    { if (indexFirstPerpendicularRoot!=0)
        *indexFirstPerpendicularRoot=i;
      return false;
    }
  return true;
}

void DynkinDiagramRootSubalgebra::GetSimpleBasisInBourbakiOrder(roots& output)
{ output.size=0;
  output.MakeActualSizeAtLeastExpandOnTop(this->RankTotal());
  for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
    this->GetSimpleBasisInBourbakiOrderOneComponentAppend(output, i);
}

void DynkinDiagramRootSubalgebra::GetSimpleBasisInBourbakiOrderOneComponentAppend(roots& outputAppend, int index)
{ std::string& theString= this->DynkinTypeStrings.TheObjects[index];
  // the order implemented here I took from the atlas of lie groups (http://www.liegroups.org/dissemination/spherical/explorer/rootSystem.cgi)
  // which should be the Bourbaki order. The order is as follows:
  // type A all roots are from left to right (or the other way round, whichever is your orientation)
  // in type B first comes the long roots in the order they appear in the diagram;
  // in types C, F and G first come the short roots
  // in type D first comes the long string, with the end node with lowest index;
  // then come the two end one-root strings in any order.
  // in type E the order is as below
  //  2
  //13456(78)
  //(2 is connected to 4)
  //The format of this function is in accordance with WeylGroup::GetEpsilonMatrix
  assert(theString.size()>0);
  if (theString.at(1)=='A'|| theString.at(1)=='B' || theString.at(1)=='G')
    outputAppend.AddListOnTop(this->SimpleBasesConnectedComponents.TheObjects[index]);
  if (theString.at(1)=='C' || theString.at(1)=='F')
    for (int i=this->SimpleBasesConnectedComponents.TheObjects[index].size-1; i>=0; i--)
      outputAppend.AddObjectOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[i]);
  if (theString.at(1)=='D')
  { int componentRank=this->SimpleBasesConnectedComponents.TheObjects[index].size;
    for (int i=componentRank-3; i>=0; i--)
      outputAppend.AddObjectOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[i]);
    outputAppend.AddObjectOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[componentRank-2]);
    outputAppend.AddObjectOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[componentRank-1]);
  }
  if (theString.at(1)=='E')
  { int componentRank=this->SimpleBasesConnectedComponents.TheObjects[index].size;
    outputAppend.AddObjectOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[componentRank-2]);
    outputAppend.AddObjectOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[componentRank-1]);
    outputAppend.AddObjectOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[componentRank-3]);
    for (int i=0; i<componentRank-3; i++)
      outputAppend.AddObjectOnTop(this->SimpleBasesConnectedComponents.TheObjects[index].TheObjects[i]);
  }
}

void DynkinDiagramRootSubalgebra::GetKillingFormMatrixUseBourbakiOrder(MatrixLargeRational& output, WeylGroup& theWeyl)
{ roots tempRoots;
  int theDimension= this->RankTotal();
  output.init(theDimension, theDimension);
  this->GetSimpleBasisInBourbakiOrder(tempRoots);
  for (int i=0; i<theDimension; i++)
    for (int j=0; j<theDimension; j++)
      output.elements[i][j]=theWeyl.RootScalarKillingFormMatrixRoot(tempRoots.TheObjects[i], tempRoots.TheObjects[j]);
}

//The below code is related to sl(2) subalgebras of simple Lie algebras
void slTwo::ComputeModuleDecomposition()
{	int theDimension= this->owner->theWeyl.KillingFormMatrix.NumRows;
  int IndexZeroWeight=this->owner->theWeyl.RootSystem.size;
	this->WeightSpaceDimensions.initFillInObject(2*(this->owner->theWeyl.RootSystem.size)+1,0);
	this->WeightSpaceDimensions.TheObjects[IndexZeroWeight]=theDimension;
	ListBasicObjects<int> BufferHighestWeights;
	bool possible=true;
	Rational tempRat;
	for (int k=0; k<this->owner->theWeyl.RootSystem.size;k++)
	{ root::RootScalarEuclideanRoot(this->hCharacteristic, this->owner->theWeyl.RootSystem.TheObjects[k], tempRat);
    assert(tempRat.DenShort==1);
		if (tempRat.NumShort>this->owner->theWeyl.RootSystem.size)
		{	possible=false;
      break;
    }
    this->WeightSpaceDimensions.TheObjects[IndexZeroWeight+tempRat.NumShort]++;
	}
	BufferHighestWeights.CopyFromBase(this->WeightSpaceDimensions);
  this->HighestWeights.MakeActualSizeAtLeastExpandOnTop(this->owner->theWeyl.RootSystem.size);
  this->MultiplicitiesHighestWeights.MakeActualSizeAtLeastExpandOnTop(this->owner->theWeyl.RootsOfBorel.size);
  this->HighestWeights.size=0;
  this->MultiplicitiesHighestWeights.size=0;
//  this->hCharacteristic.ComputeDebugString();

	std::string stop;
  if (true)
  { std::stringstream debug1;
    for (int i=0; i<IndexZeroWeight; i++)
    { debug1<< BufferHighestWeights.TheObjects[i]<<", ";
    }
    debug1<<"\n"<<BufferHighestWeights.TheObjects[IndexZeroWeight]<<"\n";
    for (int i=IndexZeroWeight+1; i<BufferHighestWeights.size; i++)
    { debug1<< BufferHighestWeights.TheObjects[i]<<", ";
    }
    stop=debug1.str();
  }
  for (int j=BufferHighestWeights.size-1; j>=IndexZeroWeight; j--)
  { int topMult = BufferHighestWeights.TheObjects[j];
    if (topMult<0)
    { possible=false;
      break;
    }
    if (topMult>0)
    {	this->HighestWeights.AddObjectOnTop(j-IndexZeroWeight);
      this->MultiplicitiesHighestWeights.AddObjectOnTop(topMult);
      if (j!=IndexZeroWeight)
        BufferHighestWeights.TheObjects[IndexZeroWeight*2-j]-=topMult;
      for (int k=j-2; k>=IndexZeroWeight; k-=2)
      {	BufferHighestWeights.TheObjects[k]-=topMult;
        if (k!=IndexZeroWeight)
           BufferHighestWeights.TheObjects[IndexZeroWeight*2-k]-=topMult;
        assert(BufferHighestWeights.TheObjects[k]==BufferHighestWeights.TheObjects[IndexZeroWeight*2-k]);
        if(BufferHighestWeights.TheObjects[k]<0)
        { possible=false;
          break;
        }
      }
    }
  }
  assert(possible);
}

void SltwoSubalgebras::ElementToString(std::string& output, GlobalVariables& theGlobalVariables, WeylGroup& theWeyl, bool useLatex, bool useHtml, bool usePNG, std::string* physicalPath, std::string* htmlPathServer)
{	std::string tempS; std::stringstream out; std::stringstream body;
  std::string tooltipHchar="Let h be in the Cartan s.a. Let \\alpha_1,...,\\alpha_n be simple roots w.r.t. h. Then the h-characteristic is the n-tuple (\\alpha_1(h),...,\\alpha_n(h))";
  std::string tooltipVDecomposition= "The sl(2) submodules of g are parametrized by their highest weight w.r.t. h. V_l is l+1 dimensional";
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].ElementToString(tempS, theGlobalVariables, *this, i, useLatex, useHtml, usePNG, physicalPath, htmlPathServer);
    body<< "Index "<< i<<": ";
    if(useHtml)
      body<<"<br>";
    body<<tempS;
    if(useHtml)
      body<<"<HR width=\"100%\">";
  }
  if(useHtml)
    out<<"<br>";
  out <<"Number of sl(2) subalgebras "<< this->size<<"\n";
  if(useHtml)
    out<<"<br><br><table><tr><td style=\"padding-right:20px\">" << CGIspecificRoutines::ElementToStringTooltip("Characteristic", tooltipHchar)  << "</td><td align=\"center\"> h</td><td style=\"padding-left:20px\" title=\""<<tooltipVDecomposition<<"\"> Decomposition of ambient Lie algebra</td> <td>Minimal containing regular SAs</td><td>Containing regular SAs </td> </tr>";
  if (this->BadHCharacteristics.size>0)
  { if (useHtml)
      out <<"<tr><td>Bad values of h</td><td>";
    this->BadHCharacteristics.ElementToString(tempS);
    out << tempS;
    if (useHtml)
      out <<"</td></tr>";
  }
  for (int i=0; i<this->size; i++)
  { slTwo& theSl2= this->TheObjects[i];
    if (useHtml)
      out << "<tr><td style=\"padding-right:20px\"><a href=\"./sl2s.html#sl2index"<< i << "\"title=\"" << tooltipHchar <<"\" >";
    out << theSl2.hCharacteristic.ElementToString();
    if (useHtml)
      out << "</a></td><td>";
    out << theSl2.theH.Hcomponent.ElementToString();
    if (useHtml)
      out << "</td><td style=\"padding-left:20px\" title=\""<<tooltipVDecomposition<<"\">";
    if (useHtml && usePNG)
      out << "<img src=\"./fla"<< this->IndicesSl2decompositionFlas.TheObjects[i]+1 <<  ".png\"></td><td>";
    else
      if (useHtml)
        out <<"</td><td>";
    for (int j=0; j<theSl2.IndicesMinimalContainingRootSA.size; j++)
    { rootSubalgebra& currentSA= this->theRootSAs.TheObjects[theSl2.IndicesMinimalContainingRootSA.TheObjects[j]];
      CGIspecificRoutines::clearDollarSigns(currentSA.theDynkinDiagram.DebugString, tempS);
      out << "<a href=\"../rootHtml_rootSA" << theSl2.IndicesMinimalContainingRootSA.TheObjects[j] << ".html\">" <<tempS<< "</a>"<<";  ";
    }
    if (useHtml)
      out<<"</td><td>";
    for (int j=0; j<theSl2.IndicesContainingRootSAs.size; j++)
    { rootSubalgebra& currentSA= this->theRootSAs.TheObjects[theSl2.IndicesContainingRootSAs.TheObjects[j]];
      CGIspecificRoutines::clearDollarSigns(currentSA.theDynkinDiagram.DebugString, tempS);
      out << "<a href=\"../rootHtml_rootSA" << theSl2.IndicesContainingRootSAs.TheObjects[j] << ".html\">" <<tempS<< "</a>"<<";  ";
    }
    if (useHtml)
      out <<"</td></tr>\n";
  }
  if (useHtml)
    out << "</table><HR width=\"100%\">";
  tempS=body.str();
  out <<tempS;
  output=out.str();
  return;
}

void SltwoSubalgebras::ElementToHtml(GlobalVariables& theGlobalVariables, WeylGroup& theWeyl, bool usePNG, std::string& physicalPath, std::string& htmlPathServer)
{ std::string physicalPathSAs;
  std::string htmlPathServerSAs;
  physicalPathSAs= physicalPath;
  physicalPathSAs.append("../");
  htmlPathServerSAs= htmlPathServer;
  htmlPathServerSAs.append("../");
  std::string tempS;
  this->theRootSAs.ElementToHtml(tempS, physicalPathSAs, htmlPathServerSAs, this, theGlobalVariables);
  if(usePNG)
  { int numExpectedFiles= this->size*8;
    this->texFileNamesForPNG.MakeActualSizeAtLeastExpandOnTop(numExpectedFiles);
    this->texStringsEachFile.MakeActualSizeAtLeastExpandOnTop(numExpectedFiles);
    this->listSystemCommandsLatex.MakeActualSizeAtLeastExpandOnTop(numExpectedFiles);
    this->listSystemCommandsDVIPNG.MakeActualSizeAtLeastExpandOnTop(numExpectedFiles);
  }
  this->texFileNamesForPNG.size=0;
  this->texStringsEachFile.size=0;
  this->listSystemCommandsLatex.size=0;
  this->listSystemCommandsDVIPNG.size=0;
  std::stringstream out, outNotation;
  std::string fileName;
  std::fstream theFile, fileFlas;
  outNotation << "<a href=\"" << htmlPathServer << "StructureConstants.html\">Notation, structure constants and Weyl group info</a><br> <a href=\"../rootHtml.html\">Root subsystem table</a><br>";
  std::string notation= outNotation.str();
  this->ElementToString(tempS, theGlobalVariables, theWeyl, false, true, usePNG, &physicalPath, &htmlPathServer);
  out <<tempS;
  if(usePNG)
  { fileName= physicalPath;
    fileName.append("sl2s.html");
    CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(theFile, fileName, false, true, false);
    tempS= out.str();
    theFile<< "<HMTL><BODY>"<<notation<<"<a href=\""<< htmlPathServer<< "sl2s_nopng.html\"> plain html for your copy+paste convenience</a><br>\n" <<tempS<<"</HTML></BODY>";
    theFile.close();
  }
  fileName= physicalPath;
  fileName.append("sl2s_nopng.html");
  this->ElementToString(tempS, theGlobalVariables, theWeyl, false, true, false, &physicalPath, &htmlPathServer);
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(theFile, fileName, false, true, false);
  theFile<< "<HMTL><BODY>"<<notation<<"<a href=\""<< htmlPathServer<< "sl2s.html\"> "<<".png rich html for your viewing pleasure (might take a while to load; available only if you checked the check box)</a><br>\n" <<tempS<<"</HTML></BODY>";
  theFile.close();
  fileName= physicalPath;
  fileName.append("StructureConstants.html");
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(theFile, fileName, false, true, false);
  this->owner.ElementToString(tempS, true, false, usePNG, theGlobalVariables, &physicalPath, &htmlPathServer, &this->texFileNamesForPNG, &this->texStringsEachFile);
  theFile<< tempS;
  theFile.close();
  if (usePNG)
  { this->listSystemCommandsLatex.SetSizeExpandOnTopNoObjectInit(this->texFileNamesForPNG.size);
    this->listSystemCommandsDVIPNG.SetSizeExpandOnTopNoObjectInit(this->texFileNamesForPNG.size);
    for (int i=0; i<this->texFileNamesForPNG.size; i++)
    { CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(fileFlas, this->texFileNamesForPNG.TheObjects[i], false, true, false);
      fileFlas<< "\\documentclass{article}\\begin{document}\\pagestyle{empty}\n"<< this->texStringsEachFile.TheObjects[i]<<"\n\\end{document}";
      std::stringstream tempStreamLatex, tempStreamPNG;
      tempStreamLatex << "latex "<< " -output-directory="<< physicalPath<<" "<< this->texFileNamesForPNG.TheObjects[i];
      tempS= this->texFileNamesForPNG.TheObjects[i];
      tempS.resize(tempS.size()-4);
      tempStreamPNG << "dvipng "<< tempS <<".dvi -o " << tempS<<".png -T tight";
      this->listSystemCommandsLatex.TheObjects[i]= tempStreamLatex.str();
      this->listSystemCommandsDVIPNG.TheObjects[i]=tempStreamPNG.str();
      fileFlas.close();
    }
  }
}

void ComputationSetup::CountNilradicals(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ rootSubalgebra tempSA;
  inputData.theRootSubalgebras.flagUseDynkinClassificationForIsomorphismComputation=true;
	inputData.theRootSubalgebras.flagUsingActionsNormalizerCentralizerNilradical=true;
	inputData.theRootSubalgebras.flagCountingNilradicalsOnlyNoComputation=true;
	inputData.theRootSubalgebras.UpperLimitNumElementsWeyl=0;
	inputData.theRootSubalgebras.GenerateAllReductiveRootSubalgebrasUpToIsomorphism(theGlobalVariables, inputData.WeylGroupLetter, inputData.WeylGroupIndex, true, true);
//  inputData.theRootSubalgebras.ComputeDebugString(true, false, true, 0, 0, theGlobalVariables);
  inputData.theRootSubalgebras.numNilradicalsBySA.initFillInObject(inputData.theRootSubalgebras.size, 0);
  inputData.theRootSubalgebras.ComputeAllRootSubalgebrasUpToIso(theGlobalVariables, 0, inputData.theRootSubalgebras.size-6);
  std::stringstream out;
  int total=0;
  for (int i=0; i<inputData.theRootSubalgebras.size-6; i++)
  { rootSubalgebra& currentSA= inputData.theRootSubalgebras.TheObjects[i];
    out << currentSA.theDynkinDiagram.DebugString<< " & " <<currentSA.theCentralizerDiagram.DebugString<<" & " << inputData.theRootSubalgebras.numNilradicalsBySA.TheObjects[i]<<" \\\\\n";
    total+=inputData.theRootSubalgebras.numNilradicalsBySA.TheObjects[i];
  }
  out <<"Total: " <<total<<" nilradicals up to iso";
  std::string tempS;
  inputData.theRootSubalgebras.ElementToStringCentralizerIsomorphisms(tempS, true, false, 0, inputData.theRootSubalgebras.size-6, theGlobalVariables);
  IndicatorWindowGlobalVariables.StatusString1=tempS;
  IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=true;

  theGlobalVariables.FeedIndicatorWindow(IndicatorWindowGlobalVariables);
  theGlobalVariables.FeedIndicatorWindow(IndicatorWindowGlobalVariables);
  theGlobalVariables.FeedIndicatorWindow(IndicatorWindowGlobalVariables);
//		(*this->theGlobalVariablesContainer->Default(),0,this->theRootSubalgebras.size-1);
}

void rootSubalgebras::ElementToStringCentralizerIsomorphisms(std::string& output, bool useLatex, bool useHtml, int fromIndex, int NumToProcess, GlobalVariables& theGlobalVariables)
{ std::stringstream out; std::string tempS;
  //W'' stands for the graph isomorphisms of C(k_ss) extending to root system isomorphisms of the entire algebra.
  for (int i=fromIndex; i<NumToProcess; i++)
    this->GenerateActionKintersectBIsos(this->TheObjects[i], theGlobalVariables);
  if (useLatex)
    out <<"\\begin{tabular}{ccccc}$\\mathfrak{k}_{ss}$& $C(k_{ss})_{ss}$ & $\\#W''$ &$\\#W'''$&$\\#(W'''\\rtimes W'')$\\\\\\hline";
  if (useHtml)
    out << "<br><table><tr><td>k_{ss}</td><td></td><td>Weyl group of C(k_{ss})_{ss}</td><td>Outer automorphisms of C(k_{ss})_{ss}<td></tr>";
  for (int i=fromIndex; i<NumToProcess; i++)
  { rootSubalgebra& current= this->TheObjects[i];
    ReflectionSubgroupWeylGroup& theOuterIsos= this->CentralizerOuterIsomorphisms.TheObjects[i];
    ReflectionSubgroupWeylGroup& theIsos= this->CentralizerIsomorphisms.TheObjects[i];
    theOuterIsos.ComputeSubGroupFromGeneratingReflections(theOuterIsos.simpleGenerators, theOuterIsos.ExternalAutomorphisms, theGlobalVariables, 0, true);
    theIsos.ComputeSubGroupFromGeneratingReflections(theIsos.simpleGenerators, theIsos.ExternalAutomorphisms, theGlobalVariables, 0, true);
    if (useHtml)
      out <<"<td>";
    out << current.theDynkinDiagram.DebugString;
    if (useHtml)
      out <<"</td><td>";
    if (useLatex)
      out <<" & ";
    out << current.theCentralizerDiagram.DebugString;
    if (useHtml)
      out <<"</td><td>";
    if (useLatex)
      out <<" & ";
    out << theOuterIsos.size;
    if (useHtml)
      out <<"</td><td>";
    if (useLatex)
      out <<" & ";
    out << theIsos.size/theOuterIsos.size;
    if (useHtml)
      out <<"</td><td>";
    if (useLatex)
      out <<" & ";
    out << theIsos.size;
    if (useHtml)
      out <<"</td></tr>";
    if (useLatex)
      out <<" \\\\\n";
  }
  if (useLatex)
    out << "\\end{tabular}";
  if(useHtml)
    out << "</table><br>";
  output= out.str();
}

bool IsRegularWRT(roots& input, root& h1, root& h2, WeylGroup& theWeyl)
{ for (int i=0; i<input.size; i++)
    if (theWeyl.RootScalarKillingFormMatrixRoot(h1, input.TheObjects[i]).IsEqualToZero() && theWeyl.RootScalarKillingFormMatrixRoot(h2, input.TheObjects[i]).IsEqualToZero())
      return false;
  return true;
}

void ComputationSetup::ExperimentWithH(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ SimpleLieAlgebra theG;
  SltwoSubalgebras tempSl2s;
  theG.FindSl2Subalgebras(tempSl2s, inputData.WeylGroupLetter, inputData.WeylGroupIndex, theGlobalVariables);
  roots h1s, h2s;
  for (int i=0; i<tempSl2s.size; i++)
    for (int j=i+1; j<tempSl2s.size; j++)
    { root& h1= tempSl2s.TheObjects[i].theH.Hcomponent;
      root& h2= tempSl2s.TheObjects[j].theH.Hcomponent;
      if (tempSl2s.theRootSAs.AmbientWeyl.RootScalarKillingFormMatrixRoot(h1, h2).IsEqualToZero())
        if (IsRegularWRT(tempSl2s.theRootSAs.AmbientWeyl.RootsOfBorel, h1, h2, tempSl2s.theRootSAs.AmbientWeyl))
        { h1s.AddObjectOnTop(h1);
          h2s.AddObjectOnTop(h2);
        }
    }
  std::stringstream out;
  tempSl2s.theRootSAs.AmbientWeyl.ProjectOnTwoPlane(h1s.TheObjects[0], h2s.TheObjects[0], theGlobalVariables);
  for (int i=0; i<h1s.size; i++)
    out << "h1: " << h1s.TheObjects[i].ElementToString() << " h2: " << h2s.TheObjects[i].ElementToString() <<"\n\n";
  IndicatorWindowGlobalVariables.StatusString1 = out.str();
  IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.FeedIndicatorWindow(IndicatorWindowGlobalVariables);
}

void WeylGroup::ProjectOnTwoPlane(root& orthonormalBasisVector1, root& orthonormalBasisVector2, GlobalVariables& theGlobalVariables)
{ for (int i=0; i<this->RootSystem.size; i++)
  { double x= this->RootScalarKillingFormMatrixRoot(orthonormalBasisVector1, this->RootSystem.TheObjects[i]).DoubleValue()*10;
    double y= this->RootScalarKillingFormMatrixRoot(orthonormalBasisVector2, this->RootSystem.TheObjects[i]).DoubleValue()*10;
    theGlobalVariables.theDrawingVariables.drawLine(0, 0, x, y, 0, 0);
  }
}

void DrawingVariables::drawLine(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex)
{ if (this->theDrawFunction!=0)
    this->theDrawFunction(X1+ this->centerX, Y1+ this->centerY, X2+ this->centerX, Y2+ this->centerY, thePenStyle, ColorIndex);
}
