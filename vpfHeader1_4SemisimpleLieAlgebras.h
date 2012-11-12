//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1_4_h_already_included
#define vpfHeader1_4_h_already_included

#include "vpfHeader1_2.h"
static ProjectInformationInstance ProjectInfoVpfHeader1_4
(__FILE__, "Header, math routines concerning semisimple Lie algebras. ");

class slTwo
{
public:
  List<int> highestWeights;
  List<int> multiplicitiesHighestWeights;
  List<int> weightSpaceDimensions;
  ElementSemisimpleLieAlgebra theH, theE, theF;
  ElementSemisimpleLieAlgebra bufferHbracketE, bufferHbracketF, bufferEbracketF;
  List<SemisimpleLieAlgebra>* owners;
  int indexOwnerAlgebra;
  SltwoSubalgebras* container;
  Rational LengthHsquared;
  int indexInContainer;
  List<int> IndicesContainingRootSAs;
  List<int> IndicesMinimalContainingRootSA;
  List<List<int> > HighestWeightsDecompositionMinimalContainingRootSA;
  List<List<int> > MultiplicitiesDecompositionMinimalContainingRootSA;
  Vectors<Rational> preferredAmbientSimpleBasis;
  Vector<Rational> hCharacteristic;
  Vector<Rational> hElementCorrespondingToCharacteristic;
  Vectors<Rational> hCommutingRootSpaces;
  Vectors<Rational> RootsHavingScalarProduct2WithH;
  DynkinDiagramRootSubalgebra DiagramM;
  DynkinDiagramRootSubalgebra CentralizerDiagram;
  PolynomialSubstitution<Rational> theSystemToBeSolved;
  Matrix<Rational> theSystemMatrixForm;
  Matrix<Rational> theSystemColumnVector;
  bool DifferenceTwoHsimpleRootsIsARoot;
  int DynkinsEpsilon;
  slTwo(): owners(0), indexOwnerAlgebra(-1), container(0), indexInContainer(-1){}
  SltwoSubalgebras& GetContainerSl2s()
  { if (this->container==0)
    { std::cout << "This is a programming error: attempting to access the container "
      << " list of a non-initialized sl(2)-subalgebra. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    return *this->container;
  }
  WeylGroup& GetOwnerWeyl();
  SemisimpleLieAlgebra& GetOwnerSSAlgebra()
  { if (this->owners==0)
    { std::cout << "This is a programming error: attempting to access the ambient "
      << " Lie algebra of a non-initialized sl(2)-subalgebra. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    return (*this->owners)[this->indexOwnerAlgebra];
  }
  std::string ToString(FormatExpressions* theFormat=0);
  void ElementToStringModuleDecomposition(bool useLatex, bool useHtml, std::string& output);
  void ElementToStringModuleDecompositionMinimalContainingRegularSAs(bool useLatex, bool useHtml, SltwoSubalgebras& owner, std::string& output);
  void ComputeModuleDecomposition(Vectors<Rational>& positiveRootsContainingRegularSA, int dimensionContainingRegularSA, List<int>& outputHighestWeights, List<int>& outputMultiplicitiesHighestWeights, List<int>& outputWeightSpaceDimensions, GlobalVariables& theGlobalVariables);
  void ComputeModuleDecompositionAmbientLieAlgebra(GlobalVariables& theGlobalVariables);
  void ComputeModuleDecompositionOfMinimalContainingRegularSAs(SltwoSubalgebras& owner, int IndexInOwner, GlobalVariables& theGlobalVariables);
  bool ModuleDecompositionFitsInto(const slTwo& other);
  static bool ModuleDecompositionFitsInto(const List<int>& highestWeightsLeft, const List<int>& multiplicitiesHighestWeightsLeft, const List<int>& highestWeightsRight, const List<int>& multiplicitiesHighestWeightsRight);
  void MakeReportPrecomputations(GlobalVariables& theGlobalVariables, SltwoSubalgebras& container, int indexInContainer, int indexMinimalContainingRegularSA, rootSubalgebra& MinimalContainingRegularSubalgebra);
  //the below is outdated, must be deleted as soon as equivalent code is written.
  void ComputeDynkinsEpsilon(WeylGroup& theWeyl);
  void ElementToHtml(std::string& filePath);
  void ElementToHtmlCreateFormulaOutputReference(const std::string& formulaTex, std::stringstream& output, bool usePNG, bool useHtml, SltwoSubalgebras& container, std::string* physicalPath, std::string* htmlPathServer);
  void operator=(const slTwo& right)
  { this->highestWeights=right.highestWeights;
    this->multiplicitiesHighestWeights=right.multiplicitiesHighestWeights;
    this->weightSpaceDimensions=right.weightSpaceDimensions;
    this->HighestWeightsDecompositionMinimalContainingRootSA=
    right.HighestWeightsDecompositionMinimalContainingRootSA;
    this->MultiplicitiesDecompositionMinimalContainingRootSA=
    right.MultiplicitiesDecompositionMinimalContainingRootSA;
    this->hCommutingRootSpaces=right.hCommutingRootSpaces;
    this->CentralizerDiagram=right.CentralizerDiagram;
    this->DiagramM=right.DiagramM;
    this->hCommutingRootSpaces=right.hCommutingRootSpaces;
    this->DifferenceTwoHsimpleRootsIsARoot=right.DifferenceTwoHsimpleRootsIsARoot;
    this->RootsHavingScalarProduct2WithH=right.RootsHavingScalarProduct2WithH;
    this->DynkinsEpsilon=right.DynkinsEpsilon;
    this->hCharacteristic=right.hCharacteristic;
    this->hElementCorrespondingToCharacteristic=right.hElementCorrespondingToCharacteristic;
    this->owners=right.owners;
    this->theE=right.theE;
    this->theH=right.theH;
    this->theF=right.theF;
    this->bufferEbracketF=right.bufferEbracketF;
    this->bufferHbracketE=right.bufferHbracketE;
    this->bufferHbracketF=right.bufferHbracketF;
    this->theSystemToBeSolved=right.theSystemToBeSolved;
    this->theSystemMatrixForm=right.theSystemMatrixForm;
    this->theSystemColumnVector=right.theSystemColumnVector;
    this->IndicesContainingRootSAs=right.IndicesContainingRootSAs;
    this->preferredAmbientSimpleBasis=right.preferredAmbientSimpleBasis;
    this->container=right.container;
    this->indexInContainer=right.indexInContainer;
    this->indexOwnerAlgebra=right.indexOwnerAlgebra;
    this->LengthHsquared=right.LengthHsquared;
  }
  bool operator==(const slTwo& right)const;
  unsigned int HashFunction() const
  { int tempI=MathRoutines::Minimum(SomeRandomPrimesSize, this->hCharacteristic.size);
    int result=0;
    for (int i=0; i<tempI; i++)
      result+= this->hCharacteristic[i].NumShort*SomeRandomPrimes[i];
    return result;
  }
  static inline unsigned int HashFunction(const slTwo& input)
  { return input.HashFunction();
  }
};

class SltwoSubalgebras : public HashedList<slTwo>
{
  friend class SemisimpleSubalgebras;
public:
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
  void operator=(const SltwoSubalgebras& other)
  { this->MultiplicitiesFixedHweight=other.MultiplicitiesFixedHweight;
    this->IndicesSl2sContainedInRootSA=other.IndicesSl2sContainedInRootSA;
    this->IndicesSl2decompositionFlas=other.IndicesSl2decompositionFlas;
    this->BadHCharacteristics =other.BadHCharacteristics;
    this->IndexZeroWeight =other.IndexZeroWeight;
    this->owners =other.owners;
    this->IndexInOwners =other.IndexInOwners;
    this->theRootSAs =other.theRootSAs;
    this->texFileNamesForPNG =other.texFileNamesForPNG;
    this->texFileNamesForPNG =other.texFileNamesForPNG;
    this->listSystemCommandsLatex =other.listSystemCommandsLatex;
    this->listSystemCommandsDVIPNG =other.listSystemCommandsDVIPNG;
  }
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
  int firstSimpleRootOrbitIndex;
  DynkinSimpleType(): owner(0), theLetter('X'), theRank(-1), firstSimpleRootOrbitIndex(-1){}
  void MakeAone(SltwoSubalgebras& inputOwner)
  { this->owner=&inputOwner;
    this->theLetter='A';
    this->theRank=1;
    this->firstSimpleRootOrbitIndex=0;
  }
  void operator=(const DynkinSimpleType& other)
  { this->owner=other.owner;
    this->theLetter=other.theLetter;
    this->theRank=other.theRank;
    this->firstSimpleRootOrbitIndex=other.firstSimpleRootOrbitIndex;
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
    this->firstSimpleRootOrbitIndex==other.firstSimpleRootOrbitIndex;
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
  std::string ToString()const;
  void operator++(int);
  bool operator>(const DynkinSimpleType& other)const
  { if (this->theRank>other.theRank)
      return true;
    if (this->theRank<other.theRank)
      return false;
    if (this->firstSimpleRootOrbitIndex>other.firstSimpleRootOrbitIndex)
      return true;
    if (this->firstSimpleRootOrbitIndex<other.firstSimpleRootOrbitIndex)
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
  ReflectionSubgroupWeylGroup theWeylSubgroup;
  List<Vectors<Rational> > CartanSAsByComponent;
  List<DynkinSimpleType> theTypes;
  charSSAlgMod<Rational> theChar;
  void operator=(const CandidateSSSubalgebra& other)
  { this->CartanSAsByComponent=other.CartanSAsByComponent;
    this->theTypes=other.theTypes;
    this->theChar=other.theChar;
    this->theWeylSubgroup=other.theWeylSubgroup;
  }
  bool ComputeChar
  ( WeylGroup& ownerWeyl
  )
  ;
  std::string ToString(FormatExpressions* theFormat=0)const;
};

class SemisimpleSubalgebras
{
public:
  List<SemisimpleLieAlgebra>* owners;
  int indexInOwners;
  SltwoSubalgebras theSl2s;
  List<SemisimpleLieAlgebra> SimpleComponentsSubalgebras;
  List<SltwoSubalgebras> theSl2sOfSubalgebras;

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
  (const CandidateSSSubalgebra& baseCandidate, GlobalVariables* theGlobalVariables)
  ;
  void ExtendOneComponentRecursive
  (const CandidateSSSubalgebra& baseCandidate, const DynkinSimpleType& theNewType,
   GlobalVariables* theGlobalVariables)
  ;
};


#endif

