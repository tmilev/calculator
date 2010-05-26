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
//   modify some tiny bit of it and 4) I got an advice on the c++ forum I go to, www.cplusplus.com, that partitioning the .h file will eventually lead to slower compile times
//   on all pieces of code that use it, especially with the massive use of templates that I do. Therefore, such a partitioning should not be done before the code reaches
//   greater maturity (see also point (1) ).
//2. Use precompiled headers or some other voodoo. I am tocally against that. Those are compiler specific, will require me to learn extra unnecessary info which might
//    be out of date in a few years, and will make even higher entry learning curve for another to join the project. This is bad.
//    I should mention in favor of Microsoft that their IDE does recompile very quickly small modifications of the file polyhedra.cpp. I believe it does so by
//    keeping a database of your recently changed code, and recompiling only the recently changed pieces. Hats off to Microsoft for doing that completely programmatically,
//    and not bothering the programmer with stupid things such as how to set up precompiled headers.
//
// To whomever might be reading this (if anyone): happy hacking and I hope you find my code useful, that it didn't cause you many headaches, and that you
// did something useful with it! Cheers!
class WeylParser;
class WeylParserNode
{
  public:
  WeylParser* owner;
  int indexParent;
  ListBasicObjects<int> children;
  void operator=(const WeylParserNode& other)
  { this->owner=other.owner;
    this->children.CopyFromBase(other.children);
    this->indexParent=other.indexParent;
  };
};

class WeylParser
{
};

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
  void Makedidj(int i, int j, int NumVars);
  void Makexidj(int i, int j, int NumVars);
  void Nullify(int NumVars);
  void TimesConstant(Rational& theConstant)
  { this->StandardOrder.TimesConstant(theConstant);
  };
  void MultiplyOnTheLeft(ElementWeylAlgebra& standsOnTheLeft, GlobalVariables& theGlobalVariables);
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
  void MultiplyTwoMonomials( Monomial<Rational>& left, Monomial<Rational>& right, PolynomialRationalCoeff& OrderedOutput, GlobalVariables& theGlobalVariables);
  ElementWeylAlgebra()
  { this->NumVariables=0; };
  void operator=(const std::string& input);
  bool IsLetter(char theLetter);
  bool IsIndex(char theIndex);
  bool IsNumber(char theNumber);
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

void main_test_function(std::string& output, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  std::string tempS;
  ElementWeylAlgebra theElement, tempEl1, tempEl2, tempEl3, tempEl4, tempEl5, Accum;
  out <<"\\documentclass{article}\\usepackage{latexsym}\\usepackage{amssymb}\\usepackage{amsmath}\n";
  out << "\\addtolength{\\hoffset}{-3.5cm}\\addtolength{\\textwidth}{6.8cm}\\addtolength{\\voffset}{-3.3cm}\\addtolength{\\textheight}{6.3cm}";
  out <<"\\begin{document}";
  theElement="x_1+x_1";
  tempEl1.Makexixj(0,1,4);
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
  out << tempS;
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

  out<<"\\end{document}";
  output=out.str();
}
