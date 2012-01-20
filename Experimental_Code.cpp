#include "polyhedra.h"

class Qsqrt2
{
public:
  Rational a;
  Rational b;
  friend std::iostream& operator<< (std::iostream& output, const Qsqrt2& theElt)
  { output << theElt.ElementToString();
    return output;
  }
  void operator =(const Qsqrt2& other)
  { this->a=other.a;
    this->b=other.b;
  }
  void operator =(int x)
  { this->a=x;
    this->b.MakeZero();
  }
  Qsqrt2(const Qsqrt2& other)
  { this->operator=(other);
  }
  Qsqrt2()
  { this->a.MakeZero();
    this->b.MakeZero();
  }
  void MakeAplusSqrt2B(int theA, int theB){ this->a=theA; this->b=theB;}
  Qsqrt2(int x)
  { this->operator=(x);
  }
  bool IsEqualToZero()const
  { return this->a.IsEqualToZero() && this->b.IsEqualToZero();
  }
  void Assign(const Qsqrt2& other)
  { this->operator=(other);
  }
  void ElementToString(std::string& output)
  { output=this->ElementToString();
  }
  std::string ElementToString()const
  { if (this->IsEqualToZero())
      return "0";
    std::stringstream out;
    if (!this->a.IsEqualToZero())
    { out << this->a.ElementToString();
      if (this->b.IsPositive())
        out << "+";
    }
    if (this->b.IsEqualToZero())
      return out.str();
    else
    { Rational tempRat=this->b;
      if (tempRat<0)
      { out << "-";
        tempRat.Minus();
      }
      if (tempRat!=1)
        out << tempRat.ElementToString();
      out << "\\sqrt{2}";
    }
    return out.str();
  }
  void operator+=(const Qsqrt2& other)
  { this->a+=other.a;
    this->b+=other.b;
  }
  void operator-=(const Qsqrt2& other)
  { this->a-=other.a;
    this->b-=other.b;
  }
  void operator*=(const Qsqrt2& other)
  { Qsqrt2 result;
    result.a=this->a*other.a+this->b*other.b*2;
    result.b=other.b*this->a+this->b*other.a;
    this->operator=(result);
  }
  void operator/=(const Rational& other)
  { this->a/=other;
    this->b/=other;
  }
  void operator/=(const Qsqrt2& otheR)
  { Qsqrt2 result, conjugate;
    conjugate=otheR;
    conjugate.b.Minus();
    result.operator=(*this);
    result*=conjugate;
    result/=(otheR.a*otheR.a-otheR.b*otheR.b*2);
    Qsqrt2 old=*this, oldOther=otheR;
    this->operator=(result);
    result*=oldOther;
    result-=old;
    if (!result.IsEqualToZero())
    { std::cout << "  start " << old.ElementToString() << "; oldOther: " << otheR.ElementToString();
      std::cout << "<hr>" << (otheR.a*otheR.a+otheR.b*otheR.b*2).ElementToString();
      old/=oldOther;
    }
    assert(result.IsEqualToZero());
  }
  void Minus()
  { this->a.Minus();
    this->b.Minus();
  }
  void Invert()
  { Qsqrt2 result;
    result=1;
    result/=*this;
    this->operator=(result);
  }
  void MultiplyBy(const Qsqrt2& other)
  { this->operator*=(other);
  }
  void Subtract(const Qsqrt2& other)
  { this->operator-=(other);
  }
};

template <class CoefficientType>
class ModuleSSalgebraNew
{
public:
  SemisimpleLieAlgebra theAlgebra;
  List<Matrix<CoefficientType> > ActionsChevalleyGenerators;
  void MakeFundamentalRep
(char WeylLetter, int theRank, GlobalVariables& theGlobalVariables)
  ;
};

template < > int List<Matrix<Qsqrt2> >::ListActualSizeIncrement=100;
template < > int List<Vector<Qsqrt2> >::ListActualSizeIncrement=100;

int ParserNode::EvaluateInvariantsExteriorPowerFundamentalRepsPlusTrivialReps
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  if (theNode.owner->DefaultWeylLetter!='B')
  { out << "My lazy master hasn't taught me how to compute that! Email him to fix that!";
    theNode.ExpressionType=theNode.typeString;
    return theNode.errorNoError;
  }
  out << "ere i am !";
  ModuleSSalgebraNew<Qsqrt2> theModule;
  theModule.MakeFundamentalRep('B', 2, theGlobalVariables);
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

template <class CoefficientType>
void ModuleSSalgebraNew<CoefficientType>::MakeFundamentalRep
(char WeylLetter, int theRank, GlobalVariables& theGlobalVariables)
{ switch(WeylLetter)
  { case 'B': break;
    default:
    assert(false);
    break;
  }
  this->theAlgebra.ComputeChevalleyConstants('B', theRank, theGlobalVariables);
  int numGenerators=this->theAlgebra.GetNumGenerators();
  int numPosRoots=this->theAlgebra.GetNumPosRoots();
  this->ActionsChevalleyGenerators.SetSize(numGenerators);
  root weightCurrentRootSpace;
  root weightInEpsCoords;
  List<Matrix<CoefficientType> > correspondingOperators;
  correspondingOperators.SetSize(numGenerators);
//  for (int sign=0; sign<2; sign++)
  for (int k=0; k<numPosRoots*2; k++)
  { weightCurrentRootSpace=this->theAlgebra.theWeyl.RootSystem[k];
//   if (sign==1)
//      weightCurrentRootSpace.MinusRoot();
    this->theAlgebra.theWeyl.GetEpsilonCoords(weightCurrentRootSpace, weightInEpsCoords, theGlobalVariables);
    int GeneratorIndex= this->theAlgebra.RootToIndexInUE(weightCurrentRootSpace);
    Matrix<CoefficientType>& currentMat=this->ActionsChevalleyGenerators[GeneratorIndex];
    currentMat.init(theRank*2+1, theRank*2+1);
    currentMat.NullifyAll((Qsqrt2) 0);
    Rational rootLength=this->theAlgebra.theWeyl.RootScalarCartanRoot(weightCurrentRootSpace, weightCurrentRootSpace);
    if (rootLength==2)
    { bool found=false;
      int first, firstDual, second, secondDual;
      for (int i=0; i<theRank; i++)
      { if (weightInEpsCoords[i]!=0)
        { if (!found)
          { if (weightInEpsCoords[i]==1)
            { first=i;
              secondDual=i+theRank+1;
            } else
            { first=theRank+i+1;
              secondDual=i;
            }
          } else
          { if (weightInEpsCoords[i]==1)
            { second=i;
              firstDual=i+theRank+1;
            } else
            { second=theRank+i+1;
              firstDual=i;
            }
          }
          found=true;
        }
      }
      currentMat.elements[first][firstDual]=1;
      currentMat.elements[second][secondDual]=-1;
    } else
    { int firstIndex= weightInEpsCoords.getIndexFirstNonZeroCoordinate();
      int firstIndexDual=firstIndex+theRank+1;
      if (weightInEpsCoords[firstIndex]==-1)
        MathRoutines::swap(firstIndex, firstIndexDual);
      currentMat.elements[firstIndex][theRank].MakeAplusSqrt2B(0,1);
      currentMat.elements[theRank][firstIndexDual].MakeAplusSqrt2B(0,-1);
    }
    std::cout << "generator " << this->theAlgebra.ChevalleyGeneratorIndexToDisplayIndex(GeneratorIndex)
    << currentMat.ElementToString(true, false);
    correspondingOperators[GeneratorIndex]=currentMat;
  }
  for (int i=0; i<theRank; i++)
    Matrix<CoefficientType>::LieBracket
    (correspondingOperators[numPosRoots+theRank+i],
    correspondingOperators[numPosRoots-1-i], correspondingOperators[numPosRoots+i]);
  Vectors<CoefficientType> rootForm;
  rootForm.SetSize(numGenerators);
  Vector<CoefficientType> tempRoot, tempRoot2;
  for (int i=0; i<numGenerators; i++)
  { correspondingOperators[i].GetNSquaredVectorForm(rootForm[i]);
    std::cout << "<hr>Generator " << i+1 << correspondingOperators[i].ElementToString(true, false)
    << " and in root form " << rootForm[i].ElementToString();
  }
  Matrix<CoefficientType> tempMat;
  PolynomialOutputFormat theFormat;
  theFormat.MakeAlphabetArbitraryWithIndex("g", "h");
  Qsqrt2 temp1, temp2;
//  temp1.MakeAplusSqrt2B
  for (int i=0; i<numGenerators; i++)
  { for (int j=0; j<numGenerators; j++)
    { tempMat.LieBracket(correspondingOperators[i],correspondingOperators[j], tempMat);
      tempMat.GetNSquaredVectorForm(tempRoot);
      if (j==7)
        std::cout << "oh no!";
      bool tempBool=
      tempRoot.GetCoordsInBasiS(rootForm, tempRoot2);
      std::cout << "<hr>Lie bracket of gen. index "
      << this->theAlgebra.GetLetterFromGeneratorIndex(i, false, false, theFormat, theGlobalVariables) << " and "
      << this->theAlgebra.GetLetterFromGeneratorIndex(j, false, false, theFormat, theGlobalVariables)
      << " equals in root form "
      << tempRoot.ElementToString() << "and in chev. coords:" << tempRoot2.ElementToString() << " and in non root form: " << tempMat.ElementToString(true, false);
      if (! tempBool)
        std::cout << "<hr>The weakest link is here!";
      assert(tempBool);
    }
  }
  /*
  Matrix<CoefficientType> tempMat, tempMat2;
  Selection tempSel;
  Vector<CoefficientType> tempRoot;
  for (int lowestNonExplored=0;
  rootFormOfTheOperators.GetRankOfSpanOfElements(tempMat, tempSel) <numGenerators;
  lowestNonExplored++)
    for (int i=0; i<theRank*2; i++)
    { tempMat2.LieBracket(correspondingOperators[lowestNonExplored], correspondingOperators[i], tempMat2);
      tempMat2.GetNSquaredVectorForm(tempRoot);
      if (!rootFormOfTheOperators.LinSpanContainsRoot(tempRoot))
      { rootFormOfTheOperators.AddOnTop(tempRoot);
        correspondingOperators.AddOnTop(tempMat2);
        std::cout << "<hr>new matrix found: " << tempMat2.ElementToString(true, false);
      }
    }

*/
}
