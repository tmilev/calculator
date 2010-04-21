#include "polyhedra.h"


template < > int HashedListBasicObjects<SltwoDecomposition>::PreferredHashSize=1000;
template < > int ListBasicObjects<SltwoDecomposition>::ListBasicObjectsActualSizeIncrement=1000;
template < > int HashedListBasicObjects<ElementSimpleLieAlgebra>::PreferredHashSize=100;
template < > int ListBasicObjects<ElementSimpleLieAlgebra>::ListBasicObjectsActualSizeIncrement=100;

extern ::IndicatorWindowVariables IndicatorWindowGlobalVariables;
extern void getZuckermansArrayE8(roots& output);

void SltwoSubalgebras::Compute(GlobalVariables& theGlobalVariables, bool flagUsingDynkinHardCodedTables)
{	this->theWeylGroup.MakeEn(8);
	int theDimension= this->theWeylGroup.KillingFormMatrix.NumRows;
	this->theWeylGroup.ComputeRho(true);
	this->ClearHashes();
  SelectionWithMaxMultiplicity theHselection;
	theHselection.initMe2(theDimension,2);
	int NumCycles= ::MathRoutines::KToTheNth(3,theDimension)-1;
	//NumCycles=5;
	root hCandidate;
	roots rootsHavingScalarProd2WithH;
	roots hCommutingRoots;
	roots RootSpacesThatCommuteWithAllRootsInTheSlTwo;
	Rational tempRat;
  this->IndexZeroWeight=this->theWeylGroup.RootsOfBorel.size;
	::SltwoDecomposition BufferDecomposition;
	hCommutingRoots.MakeActualSizeAtLeastExpandOnTop(this->theWeylGroup.RootsOfBorel.size);
	roots preComputedDynkinHs;
	if (flagUsingDynkinHardCodedTables)
  { getZuckermansArrayE8(preComputedDynkinHs);
    NumCycles=preComputedDynkinHs.size;
  }
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
    if (flagUsingDynkinHardCodedTables)
      hCandidate.Assign(preComputedDynkinHs.TheObjects[i]);
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
  //  if (!this->TheObjects[i].DifferenceTwoHsimpleRootsIsARoot)//)==tempBool)
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
      if (dimM<=DimensionCentralizer)
      { out <<"\n\n~ {This subalgebra satisfies $\\mathrm{dim} m< \\mathrm{dim} V_0 $}\\\\~\\\\~\\\\~ ";
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

void ElementSimpleLieAlgebra::operator+=(const ElementSimpleLieAlgebra& other)
{ for(int i=0;i<other.NonZeroElements.CardinalitySelection;i++)
    this->coeffsRootSpaces.TheObjects[other.NonZeroElements.elements[i]].Add
      (other.coeffsRootSpaces.TheObjects[other.NonZeroElements.elements[i]]);
  this->Hcomponent.Add(other.Hcomponent);
}

bool ElementSimpleLieAlgebra::IsEqualToZero()const
{ for (int i=0;i<this->NonZeroElements.CardinalitySelection;i++)
    if(!this->coeffsRootSpaces.TheObjects[this->NonZeroElements.elements[i]].IsEqualToZero())
      return false;
  if (!this->Hcomponent.IsEqualToZero())
    return false;
  return true;
}

void ElementSimpleLieAlgebra::init (SimpleLieAlgebra& owner)
{ this->Hcomponent.SetSizeExpandOnTopLight(owner.theWeyl.KillingFormMatrix.NumRows);
  this->coeffsRootSpaces.SetSizeExpandOnTopNoObjectInit(owner.theWeyl.RootSystem.size);
  this->NonZeroElements.init(owner.theWeyl.RootSystem.size);
}

void ElementSimpleLieAlgebra::Nullify(SimpleLieAlgebra& owner)
{ this->init(owner);
  this->Hcomponent.MakeZero(owner.theWeyl.KillingFormMatrix.NumRows);
  for(int j=0; j<this->coeffsRootSpaces.size;j++)
    this->coeffsRootSpaces.TheObjects[j].MakeZero();
  this->NonZeroElements.init(this->coeffsRootSpaces.size);
}

void SimpleLieAlgebra::LieBracket
  ( const ElementSimpleLieAlgebra& g1, const ElementSimpleLieAlgebra& g2,
    ElementSimpleLieAlgebra& output)
{ assert(&output!=&g1 && &output!=&g2);
  output.Nullify(*this);
  root tempRoot, root1plusRoot2;
  Rational tempRat;
  int i1, i2;
  int g1NumZero=g1.NonZeroElements.CardinalitySelection;
  int g2NumZero=g2.NonZeroElements.CardinalitySelection;
  for (int counter1=0; counter1<g1NumZero; counter1++)
    for (int counter2=0; counter2<g2NumZero; counter2++)
    {	i1=g1.NonZeroElements.elements[counter1];
			i2=g2.NonZeroElements.elements[counter2];
			assert
				(	!g1.coeffsRootSpaces.TheObjects[i1].IsEqualToZero() &&
          !g2.coeffsRootSpaces.TheObjects[i2].IsEqualToZero());
			root& root1= this->theWeyl.RootSystem.TheObjects[i1];
      root& root2= this->theWeyl.RootSystem.TheObjects[i2];
      root1plusRoot2=root1+root2;
      if (root1plusRoot2.IsEqualToZero())
      { tempRat.Assign(g1.coeffsRootSpaces.TheObjects[i1]);
        tempRat.MultiplyBy(g2.coeffsRootSpaces.TheObjects[i2]);
        tempRoot.Assign(root1);
        tempRoot.DivByLargeRational
          ((this->theWeyl.RootScalarKillingFormMatrixRoot(root1, root1)/tempRat)/2);
        output.Hcomponent.Add(tempRoot);
      }
      else
      { if (!this->ChevalleyConstants.elements[i1][i2].IsEqualToZero())
        { int theIndex=this->theWeyl.RootSystem.IndexOfObjectHash(root1plusRoot2);
          tempRat.Assign(g1.coeffsRootSpaces.TheObjects[i1]);
          tempRat.MultiplyBy(g2.coeffsRootSpaces.TheObjects[i2]);
          output.coeffsRootSpaces.TheObjects[theIndex].Add
            (tempRat*this->ChevalleyConstants.elements[i1][i2]);
        }
      }
    }
  ElementSimpleLieAlgebra const* element1;
  ElementSimpleLieAlgebra const* element2;
  Rational order; order.MakeOne();
  element1=&g1; element2=&g2;
  for (int l=0;l<2;l++)
	{	for (int j=0;j<element2->NonZeroElements.CardinalitySelection;j++)
		{ this->theWeyl.RootScalarKillingFormMatrixRoot
				( this->theWeyl.RootSystem.TheObjects[element2->NonZeroElements.elements[j]],
					element1->Hcomponent,tempRat);
			tempRat.MultiplyBy
				(element2->coeffsRootSpaces.TheObjects[element2->NonZeroElements.elements[j]]);
			tempRat.MultiplyBy(order);
			output.coeffsRootSpaces.TheObjects[element2->NonZeroElements.elements[j]].Add(tempRat);
		}
		order.MakeMOne();
		element1=&g2;
		element2=&g1;
	}
	output.ComputeNonZeroElements();
}

void ElementSimpleLieAlgebra::ComputeNonZeroElements()
{ this->NonZeroElements.init(this->coeffsRootSpaces.size);
  for (int i=0;i<this->coeffsRootSpaces.size;i++)
    if (!this->coeffsRootSpaces.TheObjects[i].IsEqualToZero())
      this->NonZeroElements.AddSelectionAppendNewIndex(i);
}

bool SimpleLieAlgebra::FindComplementaryNilpotent
  ( root* h, ElementSimpleLieAlgebra& e, ElementSimpleLieAlgebra& output,
    GlobalVariables& theGlobalVariables)
{ assert(e.Hcomponent.IsEqualToZero());
  e.ComputeNonZeroElements();
  root Difference;
  //format of the system
  //let an element of the simple lie algebra be written so that the first theWeyl.RootSystem.size
  // coordinates correspond to the root spaces g^\alpha,
  // and the last rank(g) coordinates correspond to the elements of the cartan.
  //then ad(e) is a linear operator which has theWeyl.RootSystem.size+theDimension rows and columns.
  //Then ad(e)ad(e)(f)=-2e, so this gives us a linear system for f.
  roots theBasis;
  int theDimension = this->theWeyl.KillingFormMatrix.NumRows;
  theBasis.SetSizeExpandOnTopNoObjectInit(theDimension);
  for(int i=0;i<theDimension;i++)
  { theBasis.TheObjects[i].MakeZero(theDimension);
    theBasis.TheObjects[i].TheObjects[i].MakeOne();
  }
  MatrixLargeRational theSystem, adESquaredadE, targetElt;
  int NumRoots=this->theWeyl.RootSystem.size;
  int NumRows=NumRoots+theDimension;
  theSystem.init( NumRows,NumRows);
  targetElt.init(NumRoots*2+theDimension,1);
  targetElt.NullifyAll();
  theSystem.NullifyAll();
  assert(e.Hcomponent.IsEqualToZero());
  for (int i=0; i<this->theWeyl.RootSystem.size; i++)
    for (int j=0; j<e.NonZeroElements.CardinalitySelection; j++)
    { int indexE = e.NonZeroElements.elements[j];
      root& rootE = this->theWeyl.RootSystem.TheObjects[indexE];
      root& rootF= this->theWeyl.RootSystem.TheObjects[i];
      root relation = rootE+rootF;
      if (!relation.IsEqualToZero())
      { int indexRel = this->theWeyl.RootSystem.IndexOfObjectHash(relation);
        if (indexRel!=-1)
          theSystem.elements[indexRel][i].Assign
            (	e.coeffsRootSpaces.TheObjects[indexE]*
							this->ChevalleyConstants.elements[indexE][i]);
      }else
      {	//if (h!=0)
				for (int k=0; k<theDimension; k++)
					theSystem.elements[NumRoots+k][i].Assign
						(	e.coeffsRootSpaces.TheObjects[indexE]*rootE.TheObjects[k]*2/
							this->theWeyl.RootScalarKillingFormMatrixRoot(rootE,rootE));
      }
      for(int j=0;j<theDimension;j++)
      { theSystem.elements[indexE][j+NumRoots].Assign
          ( this->theWeyl.RootScalarKillingFormMatrixRoot(theBasis.TheObjects[j],rootE)*
            e.coeffsRootSpaces.TheObjects[indexE]*(-1));
      }
    }
  theSystem.MultiplyOnTheLeft(theSystem,adESquaredadE);
  for (int i=0;i<e.NonZeroElements.CardinalitySelection;i++)
    targetElt.elements[e.NonZeroElements.elements[i]][0].Assign
      (e.coeffsRootSpaces.TheObjects[e.NonZeroElements.elements[i]]*(-2));
  adESquaredadE.Resize(NumRows+NumRoots, NumRows,true);
  for (int i= 0; i<NumRoots;i++)
    for(int j=0;j<NumRows;j++)
      adESquaredadE.elements[i+NumRows][j].Assign(theSystem.elements[i][j]);
  //targetH.ComputeDebugString();
	//theSystem.ComputeDebugString();
  MatrixLargeRational result;
  bool hasSolution =
    theSystem.Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(adESquaredadE,targetElt,result);
	//result.ComputeDebugString();
  if (hasSolution)
  { output.Nullify(*this);
		output.Hcomponent.MakeZero(theDimension);
    for (int i=0;i<this->theWeyl.RootSystem.size;i++)
      output.coeffsRootSpaces.TheObjects[i].Assign(result.elements[i][0]);
    output.ComputeNonZeroElements();
  }
  return hasSolution;
}

void ElementSimpleLieAlgebra::MultiplyByRational
	(SimpleLieAlgebra& owner, const Rational& theNumber)
{ if (theNumber.IsEqualToZero())
	{	this->Nullify(owner);
		return;
	}
	this->Hcomponent.MultiplyByLargeRational(theNumber);
	for(int i=0;i<this->NonZeroElements.CardinalitySelection;i++)
		this->coeffsRootSpaces.TheObjects[this->NonZeroElements.elements[i]].MultiplyBy(theNumber);
}

void ElementSimpleLieAlgebra::ElementToString
	(std::string& output, SimpleLieAlgebra& owner, bool useHtml, bool useLatex)
{ std::stringstream out; std::string tempS;
	if (useLatex)
		out <<"$";
  for (int i=0;i<this->NonZeroElements.CardinalitySelection;i++)
  { this->coeffsRootSpaces.TheObjects[this->NonZeroElements.elements[i]]
			.ElementToString(tempS);
    if (tempS=="1")
			tempS="";
		if (tempS=="-1")
			tempS="-";
		if (i!=0)
		{	if (tempS!="")
			{	if (tempS[0]!='-')
					out <<"+";
			} else
				out<<"+";
		}
		out <<tempS<<"g^{\\alpha_{" << this->NonZeroElements.elements[i]+1<<"}}";
  }
	for (int i=0;i<this->Hcomponent.size;i++)
		if (!this->Hcomponent.TheObjects[i].IsEqualToZero())
		{ this->Hcomponent.TheObjects[i].ElementToString(tempS);
			if (tempS=="1")
				tempS="";
			if (tempS=="-1")
				tempS="-";
			if (tempS!="")
			{	if (tempS[0]!='-')
					out <<"+";
			} else
				out<<"+";
			out <<tempS<<"h_{\\alpha_{"<<i+1<<"}}";
		}
	if(useLatex)
		out <<"$";
  output= out.str();
}

void ElementSimpleLieAlgebra::SetCoefficient
  ( root& indexingRoot, Rational& theCoeff, SimpleLieAlgebra& owner )
{ int index= owner.theWeyl.RootSystem.IndexOfObjectHash(indexingRoot);
  if (index!=-1)
    this->coeffsRootSpaces.TheObjects[index].Assign(theCoeff);
  this->ComputeNonZeroElements();
}
void ElementSimpleLieAlgebra::SetCoefficient
  ( root& indexingRoot, int theCoeff, SimpleLieAlgebra& owner )
{ Rational tempRat=theCoeff; this->SetCoefficient(indexingRoot,tempRat, owner);
}


void ::SimpleLieAlgebra::ElementToString
	(std::string &output, bool useHtml, bool useLatex, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
	std::string tempS;
	int numRoots=this->theWeyl.RootSystem.size;
	int theDimension= this->theWeyl.KillingFormMatrix.NumRows;
	roots theBasis;
	theBasis.SetSizeExpandOnTopNoObjectInit(theDimension);
	for (int i=0;i<theDimension;i++)
	{	out <<"$\\alpha_{"<< i+1<<"} := $ ";
		theBasis.TheObjects[i].MakeZero(theDimension);
		theBasis.TheObjects[i].TheObjects[i].MakeOne();
		out <<theBasis.TheObjects[i].ElementToString()<<"\n\n";
	}
	root tempRoot;
	for (int i=theDimension;i<numRoots;i++)
	{	this->theWeyl.RootSystem.TheObjects[i].GetCoordsInBasis
			(theBasis,tempRoot,theGlobalVariables);
		//this->theWeyl.RootSystem.TheObjects[i].ComputeDebugString();
		//tempRoot.ComputeDebugString();
		out <<"$\\alpha_"<<i+1<<" := ";
		for (int j=0;j<theDimension;j++)
		{ tempRoot.TheObjects[j].ElementToString(tempS);
			if (tempS!="0")
			{	if (tempS=="1")
					tempS="";
				if (tempS=="-1")
					tempS="-";
				if (j!=0)
				{	if (tempS!="")
					{	if (tempS[0]!='-')
							out <<"+";
					} else
						out <<"+";
				}
				out<<tempS<< "\\alpha_{" <<j+1<<"}";
			}
		}
		out <<"$";
		this->theWeyl.RootSystem.TheObjects[i].ElementToString(tempS);
		out <<"="<<tempS<< "\n\n";
	}
	for (int i=0;i<theDimension;i++)
	{	out <<"$h_{\\alpha_"<< i+1<<"} (g^{\\gamma}) := \\langle\\gamma,\\alpha_{"
				<<i+1<<" } \\rangle g^{\\gamma}$, for any $\\gamma$.\n\n";
	}
	out <<"\n\n";
	if (useLatex)
	{	out <<"\\begin{tabular}{c";
		for(int i=0;i<numRoots;i++)
			out <<"c";
		out <<"}";
		out <<"$[\\bullet,\\bullet]$&";
	}
	for (int i=0;i<numRoots;i++)
	{ out <<"$g^{\\alpha_{"<<i+1<<"}}$";
		if (i!=numRoots-1)
			out<<"&";
	}
	out <<"\\\\\n";
	Rational tempRat;
	for (int i=0;i<this->ChevalleyConstants.NumRows;i++)
	{	out <<"$g^{\\alpha_{"<<i+1<<"}}$&";
		for (int j=0; j<this->ChevalleyConstants.NumCols;j++)
		{	if (this->Computed.elements[i][j])
			{	this->ChevalleyConstants.elements[i][j].ElementToString(tempS);
				if (tempS=="1")
					tempS="";
				if (tempS=="-1")
					tempS="-";
				if (tempS=="0")
					out <<"0,";
				else
				{ tempRoot=
						this->theWeyl.RootSystem.TheObjects[i]+
						this->theWeyl.RootSystem.TheObjects[j];
					int index=this->theWeyl.RootSystem.IndexOfObjectHash(tempRoot);
					out <<"$"<<tempS<<"g^{\\alpha_{" << index+1<<"}}$, ";
				}
			}
			else
			{ root& theRoot= this->theWeyl.RootSystem.TheObjects[i];
				tempRat=2;
				tempRat.DivideBy(this->theWeyl.RootScalarKillingFormMatrixRoot(theRoot,theRoot));
				if (theRoot.IsNegativeOrZero())
					tempRat.Minus();
				tempRat.ElementToString(tempS);
				if (tempS=="1")
					tempS="";
				if (tempS=="-1")
					tempS="-";
				out << "$"<<tempS<< "h_{\\alpha_{" <<i+1<<"}}$";
			}
			if (useLatex && j!=this->ChevalleyConstants.NumCols-1)
				out<<" & ";
			else
				if(!useLatex)
					out<<"\t";
		}
		if (useLatex)
			out<<"\\\\";
		out <<"\n";
	}
	if (useLatex)
		out <<"\\end{tabular}";
	//this->ChevalleyConstants.ElementToSting(tempS);
	//out <<"\n"<< tempS<<"\n";
	output=out.str();
}



void SimpleLieAlgebra::MakeSl2ProgressReport
  (int progress, int found, int foundGood, int DifferentHs, int outOf, GlobalVariables& theGlobalVariables)
{ std::stringstream out2,out3;
  out2<< "found "<<found<< " out of "<<progress+1<<" processed out of total " << outOf<<" candidates";
  out3<< foundGood<<" good subalgebras realizing " <<DifferentHs <<" different h's";
  IndicatorWindowGlobalVariables.ProgressReportString2=out2.str();
  IndicatorWindowGlobalVariables.ProgressReportString3=out3.str();
  IndicatorWindowGlobalVariables.String2NeedsRefresh=true;
  IndicatorWindowGlobalVariables.String3NeedsRefresh=true;
  if (theGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
    theGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
}

void SimpleLieAlgebra::MakeChevalleyTestReport
  (int i, int j, int k, int Total, GlobalVariables& theGlobalVariables)
{ std::stringstream out2,out3;
  int x=(i*Total*Total+j*Total+k+1);
  out2<< "i: "<<i+1<<" of "<< Total << " j: "<<j+1<<" of "<< Total << " k: "<<k+1<<" of "<< Total;
  out3<< "Total progress: " << x<<" out of "<< (Total*Total*Total);
  IndicatorWindowGlobalVariables.ProgressReportString2=out2.str();
  IndicatorWindowGlobalVariables.ProgressReportString3=out3.str();
  //if (x%100==0)
  { IndicatorWindowGlobalVariables.String2NeedsRefresh=true;
    IndicatorWindowGlobalVariables.String3NeedsRefresh=true;
  } //else
  //{ IndicatorWindowGlobalVariables.String2NeedsRefresh=false;
   // IndicatorWindowGlobalVariables.String3NeedsRefresh=false;
  //}
  if (theGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
    theGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
}
