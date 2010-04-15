#include "polyhedra.h"


int HashedListBasicObjects< SltwoDecomposition>::PreferredHashSize=1000;
int ListBasicObjects<SltwoDecomposition>::ListBasicObjectsActualSizeIncrement=1000;
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
	roots theCentralizerRoots;
	Rational tempRat;
	this->theHelements.MakeActualSizeAtLeastExpandOnTop(1000);
	this->IndexZeroWeight=this->theWeylGroup.RootsOfBorel.size;
	::SltwoDecomposition BufferDecomposition;
	for (int i=0; i<NumCycles;i++)
	{ this->MakeProgressReport(i,NumCycles,theGlobalVariables);
		theHselection.IncrementSubset();
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
		{ BufferDecomposition.theModulesHighestWeights.size=0;
			BufferDecomposition.theModulesMultiplicities.size=0;
			for (int j=this->MultiplicitiesFixedHweight.size-1; j>=0;j--)
				if (this->MultiplicitiesFixedHweight.TheObjects[j]>0)
				{ BufferDecomposition.theModulesHighestWeights.AddObjectOnTop(j-this->IndexZeroWeight);
					BufferDecomposition.theModulesMultiplicities.AddObjectOnTop
						(this->MultiplicitiesFixedHweight.TheObjects[j]);
				}	
			//if (this->AddObjectOnTopNoRepetitionOfObjectHash(BufferDecomposition))
			//	this->theHelements.AddObjectOnTop(hCandidate);
			this->AddObjectOnTopHash(BufferDecomposition);
			this->theHelements.AddObjectOnTop(hCandidate);
		}
	}
	this->ComputeDebugString(theGlobalVariables,this->theWeylGroup,false,false);
}

void SltwoSubalgebras::ElementToString
	(	std::string &output, GlobalVariables &theGlobalVariables, WeylGroup &theWeyl, 
		bool useLatex, bool UseHtml)
{	std::string tempS; std::stringstream out;
	out <<"Number sl(2)s: "<< this->theHelements.size<<"\n";
	for (int i=0;i<this->size;i++)
	{ this->theHelements.TheObjects[i].ElementToString(tempS);
		out << "h: " << tempS;
		out << "\n sl(2) - module decomposition: ";
		for (int j=0;j<this->TheObjects[i].theModulesHighestWeights.size;j++)
		{	if ( this->TheObjects[i].theModulesMultiplicities.TheObjects[j]!=1)
				out << this->TheObjects[i].theModulesMultiplicities.TheObjects[j];
			out	<<"$V_{" << this->TheObjects[i].theModulesHighestWeights.TheObjects[j] 
					<<"}$;   ";
		}
		out <<"\n\n";
	}
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