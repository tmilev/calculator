#include "vpf.h"
#include "vpfCharacters.h"
#include "vpfGraph.h"

#include "vpfHeader1_2.h"
static ProjectInformationInstance ProjectInfoVpfCharacters
(__FILE__, "Experimental code by Thomas: finite group characters sandbox. Not fully implemented yet.");


template<>
typename List<CoxeterElement>::OrderLeftGreaterThanRight FormatExpressions::GetMonOrder<CoxeterElement>()
{ return 0;
}

bool CommandList::innerCoxeterGroup(CommandList& theCommands, const Expression& input, Expression& output)
{ SemisimpleLieAlgebra* thePointer;
  std::string errorString;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerSSLieAlgebra, input, thePointer, &errorString))
    return output.SetError(errorString, theCommands);
  CoxeterGroup theGroup;
  theGroup.MakeFrom(thePointer->theWeyl.theDynkinType);
  return output.AssignValue(theGroup, theCommands);
}

bool CommandList::innerCoxeterElement(CommandList& theCommands, const Expression& input, Expression& output)
{ //if (!input.IsSequenceNElementS(2))
  //return output.SetError("Function Coxeter element takes two arguments.", theCommands);
  if(input.children.size<2){
    return output.SetError("Function CoxeterElement needs to know what group the element belongs to", theCommands);
  }
  //note that if input is list of 2 elements then input[0] is sequence atom, and your two elements are in fact
  //input[1] and input[2];
  SemisimpleLieAlgebra* thePointer;
  std::string errorString;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerSSLieAlgebra, input[1], thePointer, &errorString))
    return output.SetError(errorString, theCommands);
  List<int> theReflections;
  for(int i=2; i<input.children.size; i++){
    int tmp;
    if (!input[i].IsSmallInteger(& tmp))
    { std::cout << "something is rotten here! " << input[i].ToString() << " is not a small integer??";
      return false;
    }
    theReflections.AddOnTop(tmp);
  }
  CoxeterGroup theGroup;
  theGroup.MakeFrom(thePointer->theWeyl.theDynkinType);
  CoxeterElement theElt;
  int indexOfOwnerGroupInObjectContainer=
  theCommands.theObjectContainer.theCoxeterGroups.AddNoRepetitionOrReturnIndexFirst(theGroup);
  theElt.owner=&theCommands.theObjectContainer.theCoxeterGroups[indexOfOwnerGroupInObjectContainer];
    //I have no idea
//  std::cout << "<b>Not implemented!!!!!</b> You requested reflection indexed by " << theReflection;
  for(int i=0; i<theReflections.size; i++){
    if (theReflections[i] > thePointer->GetRank() || theReflections[i] < 0)
      return output.SetError("Bad reflection index", theCommands);
  }
  std::cout << "\n" << theGroup.rho << " " << theElt.owner->rho << std::endl;
  theElt.reflections.AddListOnTop(theReflections);
  theElt.canonicalize();
  return output.AssignValue(theElt, theCommands);
}

bool CommandList::innerCharacter(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerCharacter");
  SemisimpleLieAlgebra* thePointer;
  std::string errorString;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerSSLieAlgebra, input[1], thePointer, &errorString))
    return output.SetError(errorString, theCommands);
  CoxeterGroup theGroup;
  theGroup.MakeFrom(thePointer->theWeyl.theDynkinType);
  CoxeterElement theElt;
  int indexOfOwnerGroupInObjectContainer=
  theCommands.theObjectContainer.theCoxeterGroups.AddNoRepetitionOrReturnIndexFirst(theGroup);
  theElt.owner=&theCommands.theObjectContainer.theCoxeterGroups[indexOfOwnerGroupInObjectContainer];

  theGroup.ComputeInitialCharacters();
  if(input.IsListNElements(3))
  { int theIndex;
    if(!input[2].IsSmallInteger(&theIndex))
      return output.SetError("Character index must be an integer", theCommands);
    if(theIndex < 0 || theIndex > theGroup.ccCount)
      return output.SetError("Character index must be between 0 and the number of conjugacy classes", theCommands);
    if(theIndex >= theGroup.characterTable.size)
      return output.SetError("Unfortunately, tom doesn't know how to calculate that one.  sorry.", theCommands);
    return output.AssignValue(theGroup.characterTable[theIndex], theCommands);
  }

  if(input.IsListNElements(theGroup.ccCount+1+1))
  { int n = theGroup.ccCount;
    List<int> X;
    for(int i=0; i<n; i++)
    { int tmp;
      if (!input[i].IsSmallInteger(& tmp))
      { std::cout << "something is rotten here! " << input[i].ToString() << " is not a small integer??";
        return false;
      }
      X.AddOnTop(tmp);
    }
    Character<Rational> theChar;
    theChar.G = &theGroup;
    theChar.data = X;
    return output.AssignValue(theChar, theCommands);
  }
  return output.SetError("Characters may be selected by index or entered by hand.", theCommands);
}

//--------------Implementing some features of some finite Coxeter groups
void CoxeterGroup::MakeFrom(const DynkinType& D){
    std::cout << "MakeFrom called" << std::endl;
    Matrix<Rational> M;
    D.GetCartanSymmetric(M);
    this->CartanSymmetric = M;
    this->nGens = M.NumCols;
    int n = M.NumCols;
    HashedList<Vector<Rational> > roots;

    Vector<Rational> v;
    for(int i=0;i<nGens;i++){
        v.MakeEi(n,i);
        roots.AddOnTopNoRepetition(this->GetOrbit(v));
    }

    this->rootSystem = roots;
    Vector<Rational> derp;
    derp.MakeZero(n);
    for(int i=0;i<roots.size;i++){
        bool usethis = true;
        for(int j=0; j<n; j++)
            if(roots[i][j] < 0){
                usethis = false;
                break;
            }
        if(usethis == true){
            std::cout << roots[i] << std::endl;
            derp += roots[i];
        }
    }
    this->rho = derp/2;
    std::cout << this->rho << std::endl;
}

HashedList<Vector<Rational> > CoxeterGroup::GetOrbit(const Vector<Rational> &vv) const{
    List<Vector<Rational> > newones;
    HashedList<Vector<Rational> > orbit;
    newones.AddOnTop(vv);
    orbit.AddOnTop(vv);

    Vector<Rational> v,w;
    while(newones.size > 0){
        v = newones.PopLastObject();
        for(int i=0;i<nGens;i++){
            w = SimpleReflection(i,v);
            if(!orbit.Contains(w)){
                newones.AddOnTop(w);
                orbit.AddOnTop(w);
            }
        }
    }
    return orbit;
}

void CoxeterGroup::ComputeRhoOrbit(){
    rhoOrbit = GetOrbit(rho);
    N = rhoOrbit.size;
}

List<int> CoxeterGroup::DecomposeTodorsVector(const Vector<Rational> &vv) const{
    List<int> l;
    Vector<Rational> v=vv,w;
    while(v != rho){
        for(int i=0; i<nGens; i++){
            w = SimpleReflection(i,v);
            if(w>v){
                v = w;
                l.AddOnTop(i);
                break;
            }
        }
    }
    return l;
}

Vector<Rational> CoxeterGroup::ApplyList(const List<int> &l, const Vector<Rational> &vv) const{
    Vector<Rational> v = vv;
    for(int i = l.size-1; i>=0; i--){
        Rational x=0;
        for(int j=0; j<nGens; j++)
            x += v[j] * CartanSymmetric.elements[l[i]][j];
        v[l[i]] -= x * 2 / CartanSymmetric.elements[l[i]][l[i]];
    }
    return v;
}

Vector<Rational> CoxeterGroup::ComposeTodorsVector(const List<int> &l) const{
    return CoxeterGroup::ApplyList(l,rho);
}

Vector<Rational> CoxeterGroup::SimpleConjugation(int i, const Vector<Rational> &vv) const{
    Vector<Rational> v = SimpleReflection(i,rho);
    v = ApplyList(DecomposeTodorsVector(vv),v);
    return SimpleReflection(i,v); // one extra copy here
}

Matrix<Rational> CoxeterGroup::TodorsVectorToInvMatrix(const Vector<Rational> &vv) const{
    Matrix<Rational> M;
    M.MakeIdMatrix(nGens);

    Vector<Rational> v,w;
    v.MakeZero(nGens);
    for(int i=0;i<nGens;i++){
        v[i] = vv[i];
    }

    while(v != rho){
        for(int i=0; i<nGens; i++){
            w = SimpleReflection(i,v);
            if(w>v){
                v = w;
                for(int j=0; j<nGens; j++){
                    Rational x=0;
                    for(int k=0; k<nGens; k++)
                        x += M.elements[k][j] * CartanSymmetric.elements[i][k];
                    M.elements[i][j] -= x*2 / CartanSymmetric.elements[i][i];
                }
                break;
            }
        }
    }
    return M;
}

Matrix<Rational> CoxeterGroup::TodorsVectorToMatrix(const Vector<Rational> &v) const{
    Matrix<Rational> M;
    M.MakeIdMatrix(nGens);

    List<int> l = DecomposeTodorsVector(v);

    // there's probably a better way to do this and i just suck at linear algebra


    for(int i = l.size-1;i>=0;i--){
        for(int j=0; j<nGens; j++){
            Rational x=0;
            for(int k=0; k<nGens; k++)
                x += M.elements[k][j] * CartanSymmetric.elements[l[i]][k];
            M.elements[l[i]][j] -= x*2 / CartanSymmetric.elements[l[i]][l[i]];
        }
    }

    return M;
}

int CoxeterGroup::MultiplyElements(int i, int j) const
{ return rhoOrbit.GetIndex(ApplyList(DecomposeTodorsVector(rhoOrbit[i]),rhoOrbit[j]));
}

int CoxeterGroup::operator()(int i, int j) const
{ return MultiplyElements(i,j);
}



void CoxeterGroup::ComputeConjugacyClasses(){
    if(rhoOrbit.size == 0)
        ComputeRhoOrbit();
    Graph G = Graph(rhoOrbit.size,nGens);
    for(int i=0;i<rhoOrbit.size;i++){
        for(int j=0;j<nGens;j++){
            G.AddEdge(i,rhoOrbit.GetIndex(SimpleConjugation(j,rhoOrbit[i])));
        }
    }
    conjugacyClasses = G.DestructivelyGetConnectedComponents();
    ccCount = conjugacyClasses.size;
    ccSizes.SetExpectedSize(ccCount);
    for(int i=0;i<ccCount;i++)
        ccSizes.AddOnTop(conjugacyClasses[i].size);
}

void CoxeterGroup::ComputeSquares(){
    if(conjugacyClasses.size == 0)
        ComputeConjugacyClasses();

    List<int> l;
    l.SetExpectedSize(ccCount);

    for(int i=0;i<ccCount;i++){
        int n,m;
        n = conjugacyClasses[i][0];
        Vector<Rational> v = ApplyList(DecomposeTodorsVector(rhoOrbit[n]),rhoOrbit[n]);
        int vi = rhoOrbit.GetIndex(v);
        for(int i=0; i<ccCount; i++){
            if(conjugacyClasses[i].BSContains(vi)){
                m = i;
                break;
            }
        }
        l.AddOnTop(m);
    }
    squares = l;
}

/*
List<Character<Rational> >& ClearCharsList(const List<Character> cchars)
{
  List<Character> chars;
  for(int i=0;i<cchars.size;i++)
    chars.AddOnTop(cchars[i]);
  List<Character> newChars;


  bool outerChanged = false;
  bool innerChanged = false;
  Character X,X2;
  for(int charindex = 0; charindex<chars.size; charindex++)
  {
  X = chars[charindex];
  do
  { for(int i=0; i<chars.size; i++)
    { if(i == charindex)
        continue;
      do
      { innerChanged = false;
        X2 = X - chars[i];
        if(X2[0]>=0 && X2.norm() < X.norm())
        { X = X2;
//          if(X.norm() == 0)
//            return X;
          innerChanged = true;
//          if(X[0]<0) // negative virtual character alert
//            for(int i=0; i<X.data.size; i++)
//              X.data[i] = -X.data[i];
        }
      } while(innerChanged);
    }
  } while(outerChanged);
  chars[charindex] = X;
  }
}
*/

class ElementGroupRing: public MonomialCollection<CoxeterElement, Rational>
{
  public:
};

void CoxeterGroup::ComputeInitialCharacters(){
  if(squares.size == 0)
    ComputeSquares();

  Character<Rational> Xe;
  Xe.G = this;
  Xe.data.SetExpectedSize(ccCount);
  for(int i=0; i<ccCount; i++)
    Xe.data.AddOnTop(1);
  characterTable.AddOnTop(Xe);

  Character<Rational> Xsgn;
  Xsgn.G = this;
  Xsgn.data.SetExpectedSize(ccCount);
  for(int i=0; i<ccCount; i++)
    Xsgn.data.AddOnTop(TodorsVectorToMatrix(rhoOrbit[conjugacyClasses[i][0]]).GetDeterminant().floorIfSmall());
  characterTable.AddOnTop(Xsgn);

    // may not actually belong in the character table
  Character<Rational> Xstd;
  Xstd.G = this;
  Xstd.data.SetExpectedSize(ccCount);
  for(int i=0; i<ccCount; i++)
    Xstd.data.AddOnTop(TodorsVectorToMatrix(rhoOrbit[conjugacyClasses[i][0]]).GetTrace().floorIfSmall());
//  Xstd = Xstd.ReducedWithChars();
  characterTable.AddOnTop(Xstd);

/*
  List<Character> allChars;
  allChars.AddListOnTop(characterTable);
  Character X,Y;
  int n;

  List<Character> products;
  for(int loopvar=1;loopvar<50;loopvar++)
  { for(int i=0; i<allChars.size; i++)
    { for(int j=0; j<allChars.size; j++)
      { products.AddOnTop(allChars[i]*allChars[j]);
      }
    }
    for(int i=0; i<allChars.size; i++)
    { for(int j=0; j<allChars.size; j++)
      { for(int k=0; k<allChars.size; k++)
        { products.AddOnTop(allChars[i]*allChars[j]);
        }
      }
    }
    while(products.size > 0)
    { X = products.PopLastObject();
      Y = X.ReducedWithChars(allChars);
      n = Y.norm();
      if(n>=1)
      { std::cout << X << Y << std::endl;
        if(n==1)
        { characterTable.AddOnTop(Y);
        }
        allChars.AddOnTop(Y);
      }
    }

    ClearCharsList(allChars);

  }*/

/*  List<Character> powers;
  powers.AddOnTop(Xstd);
  for(int loopvar=1;loopvar<50;loopvar++)
  {

    powers.AddOnTop(powers[loopvar-1]*Xstd);
  }

  for(int i=0; i<powers.size;i++){
    X = powers[i].ReducedWithChars(allChars);
    n = X.norm();
    if(n>=1){
      if(n==1)
        characterTable.AddOnTop(X);
      allChars.AddOnTop(X);
    }
  }
*/
  /*for(int i=0;i<allChars.size;i++)
  {
    X = allChars[i].Sym2();
    X = X.ReducedWithChars(allChars);
    n = X.norm();
    if(n>=1)
    { std::cout << allChars[i] << X << std::endl;
    }
    if(n==1){
      characterTable.AddOnTop(X);
      allChars.AddOnTop(X);
    }
    else if (n>1){
      allChars.AddOnTop(X);
    }

    X = allChars[i].Alt2();
    X = X.ReducedWithChars(allChars);
    n = X.norm();
    if(n>=1)
    { std::cout << allChars[i] << X << std::endl;
    }
    if(n==1){
      characterTable.AddOnTop(X);
      allChars.AddOnTop(X);
    }
    else if (n>1){
      allChars.AddOnTop(X);
    }
  }*/
  /*
  std::cout << "allchars " << allChars.size << '\n';
  for(int i=0;i<allChars.size;i++){
    std::cout << allChars[i] << '\n';
  }
  std::cout << "\n\n";
*/
}

Vector<Rational> CoxeterGroup::SimpleReflection(int i, const Vector<Rational> &v) const{
    Rational x=0;
    for(int j=0;j<this->nGens;j++)
        x += this->CartanSymmetric.elements[i][j] * v[j];
    Vector<Rational> w = v;
    w[i] -= x * 2 / CartanSymmetric.elements[i][i];
    return w;
}

CoxeterElement CoxeterGroup::GetCoxeterElement(int i)
{ return CoxeterElement(this, this->DecomposeTodorsVector(rhoOrbit[i]));
}

//-------------------------------CoxeterElement--------------------------

void CoxeterElement::canonicalize()
{ if(reflections.size < 2)
    return;
  reflections = owner->DecomposeTodorsVector(owner->ComposeTodorsVector(reflections));
}

void CoxeterElement::operator*=(const CoxeterElement& other)
{ reflections = owner->DecomposeTodorsVector(owner->ApplyList(reflections, owner->ComposeTodorsVector(other.reflections)));
}

void CoxeterElement::operator=(const CoxeterElement& other)
{ this->reflections=other.reflections;
  this->owner=other.owner;
}

bool CoxeterElement::operator==(const CoxeterElement& other)const
{ // this is all stuffed on one line because the auto keyword is only in cxx11
return owner->ComposeTodorsVector(reflections) == owner->ComposeTodorsVector(other.reflections);
}

unsigned int CoxeterElement::HashFunction(const CoxeterElement& input)
{// return input.reflections.HashFunction(); <- doesn't actually work
  unsigned int acc = 0;
  int N = (input.reflections.size < SomeRandomPrimesSize) ? input.reflections.size : SomeRandomPrimesSize;
  for(int i=0; i<N; i++){
    acc += input.reflections[i]*SomeRandomPrimes[i];
  }
  return acc;
}


std::string CoxeterElement::ToString(FormatExpressions* theFormat)const
{ if (this->owner==0)
    return "(not initialized)";
//  std::cout << "<b>CoxeterElement::toString not implemented</b>";
  std::stringstream out;
  out << "(";
  for(int i=0;i<reflections.size;i++){
    out << reflections[i];
    if(i<reflections.size-1)
      out << ", ";
  }
  out << ")";
//  out << "coxeter element owner: ";
//  out << owner->ToString(theFormat);
  return out.str();
}


