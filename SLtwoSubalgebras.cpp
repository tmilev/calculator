#include "polyhedra.h"


template < > int HashedListBasicObjects<SltwoDecomposition>::PreferredHashSize=1000;
template < > int ListBasicObjects<SltwoDecomposition>::ListBasicObjectsActualSizeIncrement=1000;
extern ::IndicatorWindowVariables IndicatorWindowGlobalVariables;
void SltwoSubalgebras::Compute(GlobalVariables& theGlobalVariables)
{	this->theWeylGroup.MakeEn(8);
	int theDimension= this->theWeylGroup.KillingFormMatrix.NumRows;
	this->theWeylGroup.ComputeRho();
	this->ClearHashes();
	this->theHelements.size=0;
	::SelectionWithMaxMultiplicity theHselection;
	theHselection.initMe2(theDimension,2);
	int NumCycles= ::MathRoutines::KToTheNth(3,theDimension)-1;
	root hCandidate;
	roots rootsHavingScalarProd2WithH;
	roots CommutingRootSpaces;
	roots hCommutingRoots;
	Rational tempRat;
	this->theHelements.MakeActualSizeAtLeastExpandOnTop(1000);
	this->IndexZeroWeight=this->theWeylGroup.RootsOfBorel.size;
	::SltwoDecomposition BufferDecomposition;
	CommutingRootSpaces.MakeActualSizeAtLeastExpandOnTop(this->theWeylGroup.RootsOfBorel.size);
	for (int i=0; i<NumCycles;i++)
	{ this->MakeProgressReport(i,NumCycles,theGlobalVariables);
		theHselection.IncrementSubset();
		CommutingRootSpaces.size=0;
		hCommutingRoots.size=0;
		rootsHavingScalarProd2WithH.size=0;
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
            CommutingRootSpaces.AddObjectOnTop
              (this->theWeylGroup.RootSystem.TheObjects[k]);
      }
      BufferDecomposition.theModulesHighestWeights.size=0;
			BufferDecomposition.theModulesMultiplicities.size=0;
			for (int j=this->MultiplicitiesFixedHweight.size-1; j>=0;j--)
				if (this->MultiplicitiesFixedHweight.TheObjects[j]>0)
				{ BufferDecomposition.theModulesHighestWeights.AddObjectOnTop(j-this->IndexZeroWeight);
					BufferDecomposition.theModulesMultiplicities.AddObjectOnTop
						(this->MultiplicitiesFixedHweight.TheObjects[j]);
          BufferDecomposition.hCommutingRootSpaces.CopyFromBase(CommutingRootSpaces);
          BufferDecomposition.DiagramM.ComputeDiagramType(hCommutingRoots,this->theWeylGroup);
          BufferDecomposition.CentralizerDiagram.ComputeDiagramType
            (CommutingRootSpaces,this->theWeylGroup);
          BufferDecomposition.hCommutingRootSpaces.CopyFromBase(CommutingRootSpaces);
				}
			if (this->AddObjectOnTopNoRepetitionOfObjectHash(BufferDecomposition))
			{	this->theHelements.AddObjectOnTop(hCandidate);
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

	out <<"Number sl(2)s: "<< this->theHelements.size<<"\n\n";
	for (int i=0;i<this->size;i++)
	{ this->theHelements.TheObjects[i].ElementToString(tempS);
		out << "h: " << tempS;
		root& r=this->theHelements.TheObjects[i];
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
    { out <<"\n\n~ \Large{This subalgebra satisfies your inequality}\\\\~\\\\~\\\\~ ";
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
	out << "\n\n Num subalgebras satisfying your inequality= "<< Zcounter;
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
