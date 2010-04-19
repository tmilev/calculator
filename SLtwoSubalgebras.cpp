#include "polyhedra.h"


template < > int HashedListBasicObjects<SltwoDecomposition>::PreferredHashSize=1000;
template < > int ListBasicObjects<SltwoDecomposition>::ListBasicObjectsActualSizeIncrement=1000;
template < > int HashedListBasicObjects<ElementSimpleLieAlgebra>::PreferredHashSize=100;
template < > int ListBasicObjects<ElementSimpleLieAlgebra>::ListBasicObjectsActualSizeIncrement=100;

extern ::IndicatorWindowVariables IndicatorWindowGlobalVariables;
void SltwoSubalgebras::Compute(GlobalVariables& theGlobalVariables)
{	this->theWeylGroup.MakeEn(8);
	int theDimension= this->theWeylGroup.KillingFormMatrix.NumRows;
	this->theWeylGroup.ComputeRho();
	this->ClearHashes();
  SelectionWithMaxMultiplicity theHselection;
	theHselection.initMe2(theDimension,2);
	int NumCycles= ::MathRoutines::KToTheNth(3,theDimension)-1;
	root hCandidate;
	roots rootsHavingScalarProd2WithH;
	roots hCommutingRoots;
	roots RootSpacesThatCommuteWithAllRootsInTheSlTwo;
	Rational tempRat;
  this->IndexZeroWeight=this->theWeylGroup.RootsOfBorel.size;
	::SltwoDecomposition BufferDecomposition;
	hCommutingRoots.MakeActualSizeAtLeastExpandOnTop(this->theWeylGroup.RootsOfBorel.size);
	for (int i=0; i<NumCycles;i++)
	{ this->MakeProgressReport(i,NumCycles,theGlobalVariables);
		theHselection.IncrementSubset();
		hCommutingRoots.size=0;
		rootsHavingScalarProd2WithH.size=0;
		RootSpacesThatCommuteWithAllRootsInTheSlTwo.size=0;
		//theHselection.ComputeDebugString();
		hCandidate.MakeZero(theDimension);
		this->MultiplicitiesFixedHweight.initFillInObject
			(this->theWeylGroup.RootSystem.size+1,0);
		this->MultiplicitiesFixedHweight.TheObjects[this->IndexZeroWeight]
			=theDimension;
		for (int j=0;j<theDimension;j++)
			hCandidate.TheObjects[j]=theHselection.Multiplicities.TheObjects[j];
		//hCandidate.ComputeDebugString();
		bool possible=true;
		for (int k=0; k<this->theWeylGroup.RootSystem.size;k++)
		{ //this->theWeylGroup.RootScalarKillingFormMatrixRoot
			root::RootScalarEuclideanRoot
				(hCandidate, this->theWeylGroup.RootSystem.TheObjects[k], tempRat);
			assert(tempRat.DenShort==1);
			if (tempRat.NumShort>theWeylGroup.RootSystem.size)
			{	possible=false;
				break;
			}
			this->MultiplicitiesFixedHweight.TheObjects[this->IndexZeroWeight+tempRat.NumShort]++;
			if (tempRat.NumShort==2)
        rootsHavingScalarProd2WithH.AddObjectOnTop(theWeylGroup.RootSystem.TheObjects[k]);
      if (tempRat.IsEqualToZero())
        hCommutingRoots.AddObjectOnTop(this->theWeylGroup.RootSystem.TheObjects[k]);
		}
		//this->ComputeDebugStringCurrent();
		for (int j=this->MultiplicitiesFixedHweight.size-1; j>=this->IndexZeroWeight;j--)
		{ int topMult=this->MultiplicitiesFixedHweight.TheObjects[j];
			if (topMult<0)
			{ possible=false;
				break;
			}
			if (topMult>0)
			{	if (j!=this->IndexZeroWeight)
					this->MultiplicitiesFixedHweight.TheObjects[this->IndexZeroWeight*2-j]-=topMult;
				for (int k=j-2;k>=this->IndexZeroWeight;k-=2)
				{	this->MultiplicitiesFixedHweight.TheObjects[k]-=topMult;
					if (k!=this->IndexZeroWeight)
						this->MultiplicitiesFixedHweight
							.TheObjects[this->IndexZeroWeight*2-k]-=topMult;
					assert(	this->MultiplicitiesFixedHweight.TheObjects[k]==
									this->MultiplicitiesFixedHweight
										.TheObjects[this->IndexZeroWeight*2-k]);
					if(this->MultiplicitiesFixedHweight.TheObjects[k]<0)
					{ possible=false;
						break;
					}
				}
			}
		}
		//this->ComputeDebugStringCurrent();
		if (possible)
		{ for (int k=0;k<this->theWeylGroup.RootSystem.size;k++)
      { root::RootScalarEuclideanRoot
          (hCandidate,this->theWeylGroup.RootSystem.TheObjects[k],tempRat);
        if (tempRat.IsEqualToZero())
          if (!rootsHavingScalarProd2WithH.ContainsARootConnectedTo
                (this->theWeylGroup.RootSystem.TheObjects[k],this->theWeylGroup))
            RootSpacesThatCommuteWithAllRootsInTheSlTwo.AddObjectOnTop
              (this->theWeylGroup.RootSystem.TheObjects[k]);
      }
      BufferDecomposition.DifferenceTwoHsimpleRootsIsARoot=false;
      for (int k=0;k<rootsHavingScalarProd2WithH.size;k++)
        for(int j=k+1;j<rootsHavingScalarProd2WithH.size;j++)
          if(this->theWeylGroup.IsARoot
              ( rootsHavingScalarProd2WithH.TheObjects[j]-
                rootsHavingScalarProd2WithH.TheObjects[k]))
            BufferDecomposition.DifferenceTwoHsimpleRootsIsARoot=true;
      BufferDecomposition.theModulesHighestWeights.size=0;
			BufferDecomposition.theModulesMultiplicities.size=0;
			for (int j=this->MultiplicitiesFixedHweight.size-1; j>=0;j--)
				if (this->MultiplicitiesFixedHweight.TheObjects[j]>0)
				{ BufferDecomposition.theModulesHighestWeights.AddObjectOnTop(j-this->IndexZeroWeight);
					BufferDecomposition.theModulesMultiplicities.AddObjectOnTop
						(this->MultiplicitiesFixedHweight.TheObjects[j]);
				}
      BufferDecomposition.hCharacteristic.Assign(hCandidate);
			BufferDecomposition.hCommutingRootSpaces.CopyFromBase(hCommutingRoots);
      BufferDecomposition.DiagramM.ComputeDiagramType(hCommutingRoots,this->theWeylGroup);
      BufferDecomposition.CentralizerDiagram.ComputeDiagramType
        (RootSpacesThatCommuteWithAllRootsInTheSlTwo,this->theWeylGroup);
      BufferDecomposition.RootsHavingScalarProduct2WithH.CopyFromBase
        (rootsHavingScalarProd2WithH);
      BufferDecomposition.ComputeDynkinsEpsilon(this->theWeylGroup);
			if (this->AddObjectOnTopNoRepetitionOfObjectHash(BufferDecomposition))
			{
			}
			//this->AddObjectOnTopHash(BufferDecomposition);
			//this->theHelements.AddObjectOnTop(hCandidate);
		}
	}
	this->ComputeDebugString(theGlobalVariables,this->theWeylGroup,false,false);
}

void SltwoSubalgebras::ElementToString
	(	std::string &output, GlobalVariables &theGlobalVariables, WeylGroup &theWeyl,
		bool useLatex, bool UseHtml)
{	std::string tempS; std::stringstream out;
  int Zcounter=0;
  int numGood=0;
	out <<"Number sl(2)s: "<< this->size<<"\n\n";
	//for (int k=0;k<2;k++)
	//{	//bool tempBool=(k==0);
	for (int i=0;i<this->size;i++)
    if (!this->TheObjects[i].DifferenceTwoHsimpleRootsIsARoot)//)==tempBool)
    { //if(this->TheObjects[i].DynkinsEpsilon==0)
      numGood++;
      this->TheObjects[i].hCharacteristic.ElementToString(tempS);
      out << "h: " << tempS;
      root& r=this->TheObjects[i].hCharacteristic;
      out <<"$\\begin{array}{ccccccc}"
          << r.TheObjects[7].ElementToString() <<" & "
          << r.TheObjects[6].ElementToString() <<" & "
          << r.TheObjects[5].ElementToString() <<" & "
          << r.TheObjects[4].ElementToString() <<" & "
          << r.TheObjects[3].ElementToString() <<" & "
          << r.TheObjects[2].ElementToString() <<" & "
          << r.TheObjects[0].ElementToString()
          <<"\\\\&&&&"
          << r.TheObjects[1].ElementToString()
          <<"\\end{array}$\n\n";
      out << "\n sl(2) - module decomposition: ";
      int DimensionCentralizer;
      for (int j=0;j<this->TheObjects[i].theModulesHighestWeights.size;j++)
      {	if ( this->TheObjects[i].theModulesMultiplicities.TheObjects[j]!=1)
          out << this->TheObjects[i].theModulesMultiplicities.TheObjects[j];
        out	<<"$V_{" << this->TheObjects[i].theModulesHighestWeights.TheObjects[j]
            <<"}$;   ";
        if (this->TheObjects[i].theModulesHighestWeights.TheObjects[j]==0)
          DimensionCentralizer=
            this->TheObjects[i].theModulesMultiplicities.TheObjects[j];
      }
      int easyCentralizerDim=this->TheObjects[i].CentralizerDiagram.NumRootsGeneratedByDiagram()+
        this->TheObjects[i].CentralizerDiagram.RankTotal();
      int dimM=this->TheObjects[i].DiagramM.NumRootsGeneratedByDiagram()+
        this->TheObjects[i].DiagramM.RankTotal();
      out <<"\n\nm is of type: " << this->TheObjects[i].DiagramM.DebugString<<" Dimension of m: "<< dimM<< "\n\n";
      if (dimM<=DimensionCentralizer+3)
      { out <<"\n\n~ {This subalgebra satisfies your inequality}\\\\~\\\\~\\\\~ ";
        Zcounter++;
      }
      else
        out <<"\n\nDimension of m is smaller than the total dim of  $V_0$";

      if (  DimensionCentralizer==easyCentralizerDim  )
        out <<"\n\n\n   Dynkin Diagram centralizer: "
            << this->TheObjects[i].CentralizerDiagram.DebugString;
      else
        out <<"\n\n\n   Dynkin Diagram generated by root spaces commuting with e: "
            << this->TheObjects[i].CentralizerDiagram.DebugString;
      if (DimensionCentralizer<easyCentralizerDim)
        out <<"aaaaaaaarrrrrrrrrrrrrggggggggggggggggghhhhhhhhh";
      out <<"\n\n\\rule{\\textwidth}{0.4pt}";
    }
	out <<  "\n\n Num subalgebras satisfying your inequality= "<< Zcounter;
	out <<  "\n\nNum good subalgebras: "<< numGood;
	output=out.str();
}

void SltwoSubalgebras::ComputeDebugStringCurrent()
{	std::string tempS; std::stringstream out;
	for (int i=0;i<this->MultiplicitiesFixedHweight.size;i++)
		out << i-this->IndexZeroWeight<<": "<<this->MultiplicitiesFixedHweight.TheObjects[i] <<"\n";
	this->DebugString=out.str();
}

void SltwoSubalgebras::MakeProgressReport
	(int index, int outOf, GlobalVariables &theGlobalVariables)
{ std::stringstream out;
	out <<index <<" out of "<< outOf <<" =3^8-1 computed";
	::IndicatorWindowGlobalVariables.ProgressReportString1=out.str();
	::IndicatorWindowGlobalVariables.String1NeedsRefresh=true;
	if (theGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		theGlobalVariables.FeedDataToIndicatorWindowDefault(::IndicatorWindowGlobalVariables);
}

void SltwoDecomposition::ComputeDynkinsEpsilon(WeylGroup& theWeyl)
{ this->DynkinsEpsilon =
    this->DiagramM.NumRootsGeneratedByDiagram()+this->DiagramM.RankTotal();
  int r=0;
  for (int i=0;i<this->hCharacteristic.size;i++)
    if (!this->hCharacteristic.TheObjects[i].IsEqualToZero())
      r++;
  this->DynkinsEpsilon+= r;
  for (int i=0;i<theWeyl.RootSystem.size;i++)
  { int NumNonZeroFound=0;
    for (int j=0;j<this->hCharacteristic.size;j++)
    { if (theWeyl.RootSystem.TheObjects[i].TheObjects[j]==1)
        NumNonZeroFound++;
      if(NumNonZeroFound>1)
        break;
    }
    if (NumNonZeroFound==1)
      this->DynkinsEpsilon--;
  }
  this->DynkinsEpsilon=0;
  for (int i=0;i<this->hCharacteristic.size;i++)
    if (this->hCharacteristic.TheObjects[i]==1)
      this->DynkinsEpsilon=-1;
  this->DynkinsEpsilon=0;

}

void ElementSimpleLieAlgebra::init (SimpleLieAlgebra& owner)
{ this->Hcomponent.SetSizeExpandOnTopLight(owner.theWeyl.KillingFormMatrix.NumRows);
  this->coeffsRootSpaces.SetSizeExpandOnTopNoObjectInit(owner.theWeyl.RootSystem.size);
}

void ElementSimpleLieAlgebra::Nullify(SimpleLieAlgebra& owner)
{ this->init(owner);
  this->Hcomponent.MakeZero(owner.theWeyl.KillingFormMatrix.NumRows);
  for(int j=0; j<this->coeffsRootSpaces.size;j++)
    this->coeffsRootSpaces.TheObjects[j].MakeZero();
}

void SimpleLieAlgebra::LieBracket
  ( const ElementSimpleLieAlgebra& g1, const ElementSimpleLieAlgebra& g2,
    ElementSimpleLieAlgebra& output)
{ assert(&output!=&g1 && &output!=&g2);
  output.Nullify(*this);
  root tempRoot, rootIplusRootJ;
  Rational tempRat;
  for (int i=0;i<g1.coeffsRootSpaces.size;i++)
    for (int j=0; j<g2.coeffsRootSpaces.size;j++)
      if (!g1.coeffsRootSpaces.TheObjects[i].IsEqualToZero() &&
          !g2.coeffsRootSpaces.TheObjects[j].IsEqualToZero())
      { root& rootI= this->theWeyl.RootSystem.TheObjects[i];
        root& rootJ= this->theWeyl.RootSystem.TheObjects[j];
        rootIplusRootJ=rootI+rootJ;
        if (rootIplusRootJ.IsEqualToZero())
        { tempRat.Assign(g1.coeffsRootSpaces.TheObjects[i]);
          tempRat.MultiplyBy(g2.coeffsRootSpaces.TheObjects[j]);
          tempRoot.Assign(rootI);
          tempRoot.DivByLargeRational
            ((this->theWeyl.RootScalarKillingFormMatrixRoot(rootI, rootI)/tempRat)/2);
          output.Hcomponent.Add(tempRoot);
        }
        else
        { if (!this->ChevalleyConstants.elements[i][j].IsEqualToZero())
          { int theIndex=this->theWeyl.RootSystem.IndexOfObjectHash(rootIplusRootJ);
            tempRat.Assign(g1.coeffsRootSpaces.TheObjects[i]);
            tempRat.MultiplyBy(g2.coeffsRootSpaces.TheObjects[j]);
            output.coeffsRootSpaces.TheObjects[theIndex].Add
              (tempRat*this->ChevalleyConstants.elements[i][j]);
          }
        }
      }
}

void ElementSimpleLieAlgebra::ComputeNonZeroElements()
{ this->NonZeroElements.init(this->coeffsRootSpaces.size);
  for (int i=0;i<this->coeffsRootSpaces.size;i++)
    if (!this->coeffsRootSpaces.TheObjects[i].IsEqualToZero())
      this->NonZeroElements.AddSelectionAppendNewIndex(i);
}

bool SimpleLieAlgebra::FindComplementaryNilpotent
  ( root& h, ElementSimpleLieAlgebra& e, ElementSimpleLieAlgebra& output,
    GlobalVariables& theGlobalVariables)
{ assert(e.Hcomponent.IsEqualToZero());
  e.ComputeNonZeroElements();
  root Difference;
  //format of the system
  //Let the roots of the Weyl be r_1, ..., r_k where k=this->theWeyl.RootSystem.size
  //Let the coefficient in front of r_i in f be x_i, let x be the column vector of the x_i's.
  //Let b be the column vector parametrizing [e,f] (<- Lie bracket). We write b as a column vector
  //in the following format: first come the coefficients in front of p_i, where p_i is the i^th
  //vector of this->theWeyl.RootSystem, and then comes the element of the Cartan.
  //Note that the column vector b is ordered differently from the column vector x!!!
  //Then define theSystem to be the matrix A such that  A x=b.
  MatrixLargeRational theSystem, targetH;
  theSystem.init
    ( this->theWeyl.RootSystem.size+this->theWeyl.KillingFormMatrix.NumRows,
      this->theWeyl.RootSystem.size);
  targetH.init(this->theWeyl.RootSystem.size+this->theWeyl.KillingFormMatrix.NumRows,1);
  targetH.NullifyAll();
  theSystem.NullifyAll();
  for (int indexF=0; indexF<this->theWeyl.RootSystem.size; indexF++)
    for (int j=0; j<e.NonZeroElements.CardinalitySelection; j++)
    { int indexE = e.NonZeroElements.elements[j];
      root& rootE = this->theWeyl.RootSystem.TheObjects[indexE];
      root PotentialF = -this->theWeyl.RootSystem.TheObjects[indexF];
      root relation = rootE-PotentialF;
      if (relation.IsEqualToZero())
      { for (int k=0; k<this->theWeyl.KillingFormMatrix.NumRows; k++)
          theSystem.elements[this->theWeyl.RootSystem.size+k][indexE].Assign
            (rootE.TheObjects[k]*2/this->theWeyl.RootScalarKillingFormMatrixRoot(rootE,rootE));
      } else
      { int indexRel = this->theWeyl.RootSystem.IndexOfObjectHash(relation);
        if (indexRel!=-1)
          theSystem.elements[indexRel][indexE].Assign
            (e.coeffsRootSpaces.TheObjects[indexE]*this->ChevalleyConstants.elements[indexE][indexF]);
      }
      theSystem.ComputeDebugString();
    }
  for (int i=0;i<this->theWeyl.KillingFormMatrix.NumRows;i++)
    targetH.elements[i+this->theWeyl.RootSystem.size][0].Assign(h.TheObjects[i]);
  MatrixLargeRational result;
  return theSystem.Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(theSystem,targetH,result);
}
