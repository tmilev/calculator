//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1_4_h_already_included
#define vpfHeader1_4_h_already_included

#include "vpfHeader1_2.h"
static ProjectInformationInstance ProjectInfoVpfHeader1_4
(__FILE__, "Header, math routines concerning semisimple Lie algebras. ");

class SltwoSubalgebras : public HashedList<slTwo>
{
  friend class SemisimpleSubalgebras;
public:
//  List< int > hSingularWeights;
  List<int> MultiplicitiesFixedHweight;
  List<List<int> > IndicesSl2sContainedInRootSA;
  List<int> IndicesSl2decompositionFlas;
  Vectors<Rational> BadHCharacteristics;
  int IndexZeroWeight;
  List<SemisimpleLieAlgebra>* owners;
  int IndexInOwners;
  rootSubalgebras theRootSAs;
  List<std::string> texFileNamesForPNG;
  List<std::string> texStringsEachFile;
  List<std::string> listSystemCommandsLatex;
  List<std::string> listSystemCommandsDVIPNG;
  void CheckForCorrectInitializationCrashIfNot()
  { if (this->owners==0 || this->IndexInOwners<0)
    { std::cout << "This is a programming error. "
      << " Object SltwoSubalgebras is not initialized, although it is supposed to be. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
  }
  WeylGroup& GetOwnerWeyl()
  { return this->GetOwner().theWeyl;
  }
  SemisimpleLieAlgebra& GetOwner()
  { this->CheckForCorrectInitializationCrashIfNot();
    return (*this->owners)[this->IndexInOwners];
  }
  void ComputeModuleDecompositionsOfAmbientLieAlgebra(GlobalVariables& theGlobalVariables)
  { for(int i=0; i<this->size; i++)
      this->TheObjects[i].ComputeModuleDecompositionAmbientLieAlgebra(theGlobalVariables);
  }
  void init(List<SemisimpleLieAlgebra>* inputOwners, int inputIndexInOwner);
  SltwoSubalgebras(): owners(0), IndexInOwners(-1)
  {}
  SltwoSubalgebras(List<SemisimpleLieAlgebra>* inputOwners, int inputIndexInOwner)
  : owners(inputOwners), IndexInOwners(inputIndexInOwner)
  {}
  void ComputeModuleDecompositionsOfMinimalContainingRegularSAs(GlobalVariables& theGlobalVariables)
  { std::cout << "This is a programming error. This function used to work in an older"
    << " version of the program, but, as the requirements have changed, now needs a rewrite. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
    /*for(int i=0; i<this->size; i++)
      (*this)[i].ComputeModuleDecompositionOfMinimalContainingRegularSAs(*this, i, theGlobalVariables);
      */
  }
  void getZuckermansArrayE8(Vectors<Rational>& output);
  void MakeProgressReport(int index, int outOf, GlobalVariables& theGlobalVariables);
  void ComputeDebugStringCurrent();
  bool ContainsSl2WithGivenH(Vector<Rational>& theH, int* outputIndex);
  bool ContainsSl2WithGivenHCharacteristic(Vector<Rational>& theHCharacteristic, int* outputIndex);
  void ElementToHtml
  (FormatExpressions* theFormat=0, GlobalVariables* theGlobalVariables=0)
   ;
  std::string ElementToStringNoGenerators(FormatExpressions* theFormat=0);
  void ElementToStringModuleDecompositionMinimalContainingRegularSAs(std::string& output, bool useLatex, bool useHtml);
  std::string ToString(FormatExpressions* theFormat=0);
};

class DynkinSimpleType
{ friend std::ostream& operator << (std::ostream& output, const DynkinSimpleType& theMon)
  { output << theMon.ToString();
    return output;
  }
  public:
  SltwoSubalgebras* owner;
  char theLetter;
  int theRank;
  int longRootOrbitIndex;
  DynkinSimpleType(): owner(0), theLetter('X'), theRank(-1), longRootOrbitIndex(-1){}
  void MakeAone(SltwoSubalgebras& inputOwner)
  { this->owner=&inputOwner;
    this->theLetter='A';
    this->theRank=1;
    this->longRootOrbitIndex=0;
  }
  void operator=(const DynkinSimpleType& other)
  { this->theLetter=other.theLetter;
    this->theRank=other.theRank;
    this->longRootOrbitIndex=other.longRootOrbitIndex;
  }
  void assertIAmInitialized()const
  { if (this->owner==0)
    { std::cout << "This is a programming error:  "
      << " using a non-initialized Dynkin simple type. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
  }
  bool operator==(const DynkinSimpleType& other)const
  { this->assertIAmInitialized();
    if (this->owner!=other.owner)
    { std::cout << "This is a programming error: attempting to compare "
      << " Dynkin types of subalgebras with different owners. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    return
    this->theLetter==other.theLetter && this->theRank==other.theRank &&
    this->longRootOrbitIndex==other.longRootOrbitIndex;
  }
  static unsigned int HashFunction(const DynkinSimpleType& input)
  { return ((unsigned int) input.theLetter)*2+input.theRank;
  }
  unsigned int HashFunction()const
  { return this->HashFunction(*this);
  }
  inline bool IsEqualToZero()const
  { return false;
  }
  std::string ToString()const
  { std::stringstream out;
    if (theRank>=10)
      out << theLetter << "_{" << theRank << "}";
    else
      out << theLetter << "_" << theRank;
    return out.str();
  }
  void operator++(int);
  bool operator>(const DynkinSimpleType& other)const
  { if (this->theRank>other.theRank)
      return true;
    if (this->theRank<other.theRank)
      return false;
    if (this->longRootOrbitIndex>other.longRootOrbitIndex)
      return true;
    if (this->longRootOrbitIndex<other.longRootOrbitIndex)
      return false;
    if (this->theLetter=='B' && other.theLetter=='D')
      return true;
    if (this->theLetter=='D' && other.theLetter=='B')
      return false;
    return this->theLetter>other.theLetter;
  }
  inline bool operator<(const DynkinSimpleType& other)const
  { return other>*this;
  }
  bool operator<(int otherRank)const;
};

class DynkinType : public MonomialCollection<DynkinSimpleType, int>
{
public:
  void GetLettersTypesMults (List<char>& outputLetters, List<int>& outputRanks, List<int>& outputMults)
  { outputLetters.SetSize(0);
    outputRanks.SetSize(0);
    outputMults.SetSize(0);
    for (int i=0; i<this->size; i++)
    { outputLetters.AddOnTop((*this)[i].theLetter);
      outputRanks.AddOnTop((*this)[i].theRank);
      outputMults.AddOnTop(this->theCoeffs[i]);
    }
  }
};

class CandidateSSSubalgebra
{
public:
  List<Vectors<Rational> > CartanSAsByComponent;
  List<DynkinSimpleType> theTypes;
  void operator=(const CandidateSSSubalgebra& other)
  { this->CartanSAsByComponent=other.CartanSAsByComponent;
  }
};

class SemisimpleSubalgebras
{
public:
  List<SemisimpleLieAlgebra>* owners;
  int indexInOwners;
  SltwoSubalgebras theSl2s;
  List<CandidateSSSubalgebra> Hcandidates;
  int theRecursionCounter;
  SemisimpleLieAlgebra& GetSSowner()
  { if (this->owners==0 || this->indexInOwners<0)
    { std::cout << "This is a programming error: attempted to access non-initialized "
      << " semisimple Lie subalgerbas. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    return (*this->owners)[this->indexInOwners];
  }
  SemisimpleSubalgebras(List<SemisimpleLieAlgebra>* inputOwners, int inputIndexInOwner):
  owners(inputOwners), indexInOwners(inputIndexInOwner)
  { this->theSl2s.owners=inputOwners;
    this->theSl2s.IndexInOwners=inputIndexInOwner;
    this->theRecursionCounter=0;
  }
  std::string ToString(FormatExpressions* theFormat=0);
  void FindTheSSSubalgebras
  (List<SemisimpleLieAlgebra>* newOwner, int newIndexInOwner, GlobalVariables* theGlobalVariables)
  ;
  void FindTheSSSubalgebrasPart2
  (GlobalVariables* theGlobalVariables)
  ;
  void ExtendCandidatesRecursive
  (CandidateSSSubalgebra& theCandidate, GlobalVariables* theGlobalVariables)
  ;
};


#endif

