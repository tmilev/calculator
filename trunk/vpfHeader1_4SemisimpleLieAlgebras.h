//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1_4_h_already_included
#define vpfHeader1_4_h_already_included

#include "vpfHeader1_2.h"
static ProjectInformationInstance ProjectInfoVpfHeader1_4
(__FILE__, "Header, math routines concerning semisimple Lie algebras. ");

class slTwoSubalgebra
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
  Vectors<Rational> RootsWithScalar2WithH;
  DynkinDiagramRootSubalgebra DiagramM;
  DynkinDiagramRootSubalgebra CentralizerDiagram;
  PolynomialSubstitution<Rational> theSystemToBeSolved;
  Matrix<Rational> theSystemMatrixForm;
  Matrix<Rational> theSystemColumnVector;
  bool DifferenceTwoHsimpleRootsIsARoot;
  int DynkinsEpsilon;
  slTwoSubalgebra(): owners(0), indexOwnerAlgebra(-1), container(0), indexInContainer(-1){}
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
  void GetInvolvedPosGenerators(List<ChevalleyGenerator>& output);
  void GetInvolvedNegGenerators(List<ChevalleyGenerator>& output);
  void ElementToStringModuleDecomposition(bool useLatex, bool useHtml, std::string& output);
  void ElementToStringModuleDecompositionMinimalContainingRegularSAs(bool useLatex, bool useHtml, SltwoSubalgebras& owner, std::string& output);
  void ComputeModuleDecomposition(Vectors<Rational>& positiveRootsContainingRegularSA, int dimensionContainingRegularSA, List<int>& outputHighestWeights, List<int>& outputMultiplicitiesHighestWeights, List<int>& outputWeightSpaceDimensions, GlobalVariables& theGlobalVariables);
  void ComputeModuleDecompositionAmbientLieAlgebra(GlobalVariables& theGlobalVariables);
  void ComputeModuleDecompositionOfMinimalContainingRegularSAs(SltwoSubalgebras& owner, int IndexInOwner, GlobalVariables& theGlobalVariables);
  bool ModuleDecompositionFitsInto(const slTwoSubalgebra& other);
  static bool ModuleDecompositionFitsInto(const List<int>& highestWeightsLeft, const List<int>& multiplicitiesHighestWeightsLeft, const List<int>& highestWeightsRight, const List<int>& multiplicitiesHighestWeightsRight);
  void MakeReportPrecomputations(GlobalVariables& theGlobalVariables, SltwoSubalgebras& container, int indexInContainer, int indexMinimalContainingRegularSA, rootSubalgebra& MinimalContainingRegularSubalgebra);
  //the below is outdated, must be deleted as soon as equivalent code is written.
  void ComputeDynkinsEpsilon(WeylGroup& theWeyl);
  void ElementToHtml(std::string& filePath);
  void ElementToHtmlCreateFormulaOutputReference(const std::string& formulaTex, std::stringstream& output, bool usePNG, bool useHtml, SltwoSubalgebras& container, std::string* physicalPath, std::string* htmlPathServer);
  void operator=(const slTwoSubalgebra& right)
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
    this->RootsWithScalar2WithH=right.RootsWithScalar2WithH;
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
  bool operator==(const slTwoSubalgebra& right)const;
  unsigned int HashFunction() const
  { int tempI=MathRoutines::Minimum(SomeRandomPrimesSize, this->hCharacteristic.size);
    int result=0;
    for (int i=0; i<tempI; i++)
      result+= this->hCharacteristic[i].NumShort*SomeRandomPrimes[i];
    return result;
  }
  static inline unsigned int HashFunction(const slTwoSubalgebra& input)
  { return input.HashFunction();
  }
};

class SltwoSubalgebras : public HashedList<slTwoSubalgebra>
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

class SemisimpleSubalgebras;

class CandidateSSSubalgebra
{
public:
  WeylGroup theWeylNonEmbeddeD;
  WeylGroup theWeylNonEmbeddeDdefaultScale;
  List<Vectors<Rational> > CartanSAsByComponent;
  Vectors<Rational> theCoRoots;
  Vectors<Rational> theHs;
  List<DynkinSimpleType> theTypes;
  List<ElementSemisimpleLieAlgebra> thePosGens;
  List<ElementSemisimpleLieAlgebra> theNegGens;
//  List<List<int> > theHorbitIndices;
//  List<List<ElementWeylGroup> > theHWeylGroupElts;
  List<List<ChevalleyGenerator> > theInvolvedPosGenerators;
  List<List<ChevalleyGenerator> > theInvolvedNegGenerators;
  DynkinType theTypeTotal;
  charSSAlgMod<Rational> theCharFundamentalCoordsRelativeToCartan;
  charSSAlgMod<Rational> theCharFundCoords;
  Vectors<Rational> PosRootsPerpendicularPrecedingWeights;
  List<Polynomial<Rational> > theSystemToSolve;
  SemisimpleSubalgebras* owner;
  int indexInOwnersOfNonEmbeddedMe;
  CandidateSSSubalgebra(): owner(0), indexInOwnersOfNonEmbeddedMe(-1){}
  void operator=(const CandidateSSSubalgebra& other)
  { this->CartanSAsByComponent=other.CartanSAsByComponent;
    this->theTypes=other.theTypes;
    this->theCharFundamentalCoordsRelativeToCartan=other.theCharFundamentalCoordsRelativeToCartan;
    this->theCharFundCoords=other.theCharFundCoords;
    this->theWeylNonEmbeddeD=other.theWeylNonEmbeddeD;
    this->theWeylNonEmbeddeDdefaultScale=other.theWeylNonEmbeddeDdefaultScale;
    this->PosRootsPerpendicularPrecedingWeights=other.PosRootsPerpendicularPrecedingWeights;
    this->indexInOwnersOfNonEmbeddedMe=other.indexInOwnersOfNonEmbeddedMe;
    this->theTypeTotal=other.theTypeTotal;
    this->thePosGens=other.thePosGens;
    this->theNegGens=other.theNegGens;
//    this->theHorbitIndices=other.theHorbitIndices;
//    this->theHWeylGroupElts=other.theHWeylGroupElts;
    this->theInvolvedPosGenerators=other.theInvolvedPosGenerators;
    this->theInvolvedNegGenerators=other.theInvolvedNegGenerators;
    this->owner=other.owner;
    this->theSystemToSolve=other.theSystemToSolve;
    this->theCoRoots=other.theCoRoots;
    this->theHs=other.theHs;
  }
  bool IsWeightSystemSpaceIndex
(int theIndex, const Vector<Rational>& AmbientRootTestedForWeightSpace);
  void AddTypeIncomplete(const DynkinSimpleType& theNewType);
  void AddHincomplete
  (const Vector<Rational>& theH, const ElementWeylGroup& theWE, int indexOfOrbit)
  ;
  bool CheckInitialization()const;
  SemisimpleLieAlgebra& GetAmbientSS();
  WeylGroup& GetAmbientWeyl();
  bool ComputeSystem
(SemisimpleLieAlgebra& ownerSS, List<SemisimpleLieAlgebra>& ownersSubalgebra,
 int indexInOwners, GlobalVariables* theGlobalVariables)
 ;
  bool ComputeChar
(GlobalVariables* theGlobalVariables)

  ;
  bool isGoodForTheTop
  (WeylGroup& ownerWeyl, const Vector<Rational>& HneW)const
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
  HashedList<SemisimpleLieAlgebra> theSubalgebrasNonEmbedded;
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
  void AddCandidatesSubalgebra
  (CandidateSSSubalgebra& theCandidate, GlobalVariables* theGlobalVariables)
  ;
  void FindAllEmbeddings
  (DynkinSimpleType& theType, List<SemisimpleLieAlgebra>* newOwner, int newIndexInOwner,
   GlobalVariables* theGlobalVariables)
  ;
  void FindTheSSSubalgebras
  (List<SemisimpleLieAlgebra>* newOwner, int newIndexInOwner, GlobalVariables* theGlobalVariables)
  ;
  void FindTheSSSubalgebrasPart2
  (GlobalVariables* theGlobalVariables)
  ;
  void ExtendCandidatesRecursive
  (const CandidateSSSubalgebra& baseCandidate, bool propagateRecursion,
   GlobalVariables* theGlobalVariables)
  ;
  void ExtendOneComponentOneTypeAllLengthsRecursive
  (const CandidateSSSubalgebra& baseCandidate, DynkinSimpleType& theType,
   bool propagateRecursion, GlobalVariables* theGlobalVariables)
;
  void ExtendOneComponentRecursive
  (const CandidateSSSubalgebra& baseCandidate, bool propagateRecursion,
   GlobalVariables* theGlobalVariables)
  ;
};


#endif

