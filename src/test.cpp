//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#include "calculator.h"
#include <assert.h>


#include "vpfHeader4SystemFunctionsGlobalObjects.h"

//#include "testlib.h"
#include <iostream>
#include <algorithm>

#include "vpfJson.h"
#include "vpfFiniteFields.h"
#include "math_extra_packed_vector_implementation.h"
#include "math_extra_finite_groups_implementation.h"

#include "math_extra_symmetric_groups_and_generalizations.h"
#include "general_test.h"
#include "math_general_implementation.h"

/*
// The Plan:
// * HomomorphismSemidirectElement class takes a GroupHomomorphism pointer, a bool, and a group element
// * That group element is permitted to be of any type, including GenericElement

class GlobalVariablesStdoutClass: public StdoutClass
{ GlobalVariables* theGlobalVariables;
};


class GlobalAtomicStringVoidPointerTableClass;

class GlobalAtomicStringVoidPointerTableClass
{ public:
  HashedList<AtomicString> theStrings;
  List<void*> theVoidPointers;

  void SetPointer(const char* s, void* p, int ssz= - 1) {
    AtomicString as;
    if (ssz == - 1) {
      ssz = 0;
      for (; ; ssz++)
        if (s[ssz] == 0)
          break;
    }
    char* aString = (char*) malloc(ssz);
    for (int i = 0; i <ssz; i ++)
      aString[i] = s[i];
    as.theString = aString;
    this->theStrings.AddOnTop(as);
    this->theVoidPointers.AddOnTop(p);
  }

  void SetPointer(std::string s, void* p) {
    SetPointer(s.c_str(), p, s.size());
  }

  void* GetPointer(const AtomicString& as) {
    int i = this->theStrings.GetIndex(as);
    if (i == - 1)
      return 0;
    return this->theVoidPointers[i];
  }

  void* GetPointer(const std::string& s) {
    AtomicString as;
    as.theString = s.c_str();
    return this->GetPointer(as);
  }
};

GlobalAtomicStringVoidPointerTableClass VPT;

template <typename somestream>
somestream AtomicString::IntoStream(somestream& out, GlobalVariables* unused) {
  if (this->vpti == - 1)
    this->vpti = VPT.theStrings.GetIndex(*this);
  out << "AtomicString[" << this->vpti << "]: " << this->theString;
  return out;
}
*/

//TODO: Semidirect product groups have a product of elements, whereas we only need a product of pointers and a sum of
// elements in memory.  Fortunately HomoSemidirectElements only carry twice as much elements.
template <typename kelt>
class HomomorphismSemidirectElement: public GroupConjugacyImplementation<HomomorphismSemidirectElement<kelt> > {
  bool h;
  kelt k;
  GroupHomomorphism<kelt, kelt>* oa;

  HomomorphismSemidirectElement operator*(const HomomorphismSemidirectElement& right) const {
    HomomorphismSemidirectElement out;
    out.h = this->h ^ right.h;
    kelt tmp = this->h ? this->oa(this->k) : this->k;
    out.k = tmp * right.k;
    return out;
  }

  void Invert() {
    crash << "Code commented out due to compilation error with new compiler. " << crash;
    //this->h.Invert();
    //this->k = this->oa(this->h,this->k);
    //this->k.Invert();
  }

  void MakeID(const HomomorphismSemidirectElement& prototype) {
    this->h = false;
    this->k.MakeID(prototype.k);
  }

  void operator=(const kelt& theKelt) {
    this->h = false;
    this->k = theKelt;
  }

  bool operator==(const HomomorphismSemidirectElement& right) const {
    if (!(this->h == right.h)) {
      return false;
    }
    if (!(this->k == right.k)) {
      return false;
    }
    return true;
  }

  bool operator>(const HomomorphismSemidirectElement& right) const {
    if (!this->h && right.h) {
      return false;
    }
    if (this->h && !right.h) {
      return true;
    }
    return this->k > right.k;
  }
};

template <typename kelt>
class HomomorphismSemidrectGroupData {
  GroupHomomorphism<kelt, kelt>* oa;
  FiniteGroup<kelt>* theSubgroup;
  FiniteGroup<HomomorphismSemidirectElement<kelt> >* theGroup;
  FiniteGroup<HomomorphismSemidirectElement<kelt> > theGroupMayBeHereNameIsLongToDiscourageUse;

  void MakeHomoSemidirectGroupData(FiniteGroup<kelt>* subgroup, GroupHomomorphism<kelt, kelt>* outer) {
    this->theSubgroup = subgroup;
    this->oa = outer;
    this->theGroup = &theGroupMayBeHereNameIsLongToDiscourageUse;
    this->theGroup->generators = this->theSubgroup->generators;
    this->theGroup->generators.SetSize(this->theGroup->generators.size + 1);
    this->theGroup->generators.LastObject()->MakeID(this->theGroup->generators[0]);
    this->theGroup->generators.LastObject()->h = true;
  }
};


/*class GeneralizedElement: public GroupConjugacyImplementation
{ char specifictype;
  union data
  { ElementZ2N b;
    ElementZmodP c;
    ElementFiniteGroup f;
    Matrix<Rational> m;
    ElementHyperoctahedralGroupR2 o;
    PermutationR2 p;
    UserSemidirectElement u;
    ElementWeylGroup w;
  };

  GeneralizedElement operator*(const GeneralizedElement right) const {
    if (this->specifictype != right.specifictype)
      crash << "Illegal operation " << __FILE__ << ":" << __LINE__ << crash;
    GeneralizedElement out;
    out.specifictype = this->specifictype;
    switch(this->specifictype) {
      case 'b':
        out.data.b = data.b * right.data.b;
        return out;
      case 'c':
        out.data.c = data.c * right.data.c;
        return out;
      case 'f':
        out.data.f = data.f * right.data.f;
        return out;
      case 'm':
        out.data.m = data.m * right.data.m;
        return out;
      case 'o':
        out.data.o = data.o * right.data.o;
        return out;
      case 'p':
        out.data.p = data.p * right.data.p;
        return out;
      case 'u':
        out.data.u = data.u * right.data.u;
        return out;
      case 'w':
        out.data.w = data.w * right.data.w;
        return out;
    }
  }

  void Invert() {
    switch(this->specifictype) {
      case 'b':
        return data.b.Invert();
      case 'c':
        return data.c.Invert();
      case 'f':
        return data.f.Invert();
      case 'm':
        return data.m.Invert();
      case 'o':
        return data.o.Invert();
      case 'p':
        return data.p.Invert();
      case 'u':
        return data.u.Invert();
      case 'w':
        return data.w.Invert();
    }
  }

};*/

class CharacterTable {
public:
  List<List<int> > representatives;
  List<int> sizes;
  List<Vector<Rational> > characters;

  void write(std::string filename);
  void read(std::string filename);
};


/*{ std::ofstream out;
  out.open(filename.c_str());
  // representatives
  out << '[';
  for (int i = 0; i < this->representatives.size; i ++) {
    out << '[';
    for (int j = 0; j < this->representatives[i].size; j ++) {
      out << this->representatives[i][j];
      if (j != this->representatives[i].size- 1)
        out << ',';
    }
    out << ']';
    if (i != this->representatives.size- 1)
      out << ',';
  }
  out << ']';
  out << '\n';

  // sizes
  out << '[';
  for (int i = 0; i < this->sizes.size; i ++) {
    out << this->sizes[i];
    if (i != this->sizes.size- 1)
      out << ',';
  }
  out << ']';
  out << '\n';

  // characters
  out << '[';
  for (int i = 0; i < this->characters.size; i ++) {
    out << '[';
    for (int j = 0; j < this->characters[i].size; j ++) {
      out << '"' << this->characters[i][j] << '"';
      if (j != this->characters[i].size- 1)
        out << ',';
    }
    out << ']' << '\n';
    if (i != this->characters.size- 1)
      out << ',';
  }
  out << ']';
}*/

bool CharacterComparator(const Vector<Rational>& left, const Vector<Rational>& right) {
  if (left[0] > right[0]) {
    return true;
  }
  if (left[0] < right[0]) {
    return false;
  }
  for (int i = 1; i < left.size; i ++) {
    if (left[i] > right[i]) {
      return false;
    }
    if (left[i] < right[i]) {
      return true;
    }
  }
  return false;
}

template <>
unsigned int Vector<int>::HashFunction() const {
  unsigned int result = 0;
  int theSize = MathRoutines::Minimum(this->size, SomeRandomPrimesSize);
  for (int i = 0; i < theSize; i ++) {
    result += (unsigned int) this->TheObjects[i] * SomeRandomPrimes[i];
  }
  return result;
}


// A ListListInt is a size and a List containing a bunch of pointers followed by
// pointed to sublists.  This might be a good idea because valgrind says the non-local
// data structure List[List[int]] leads to lots and lots of memory accesses
class SubListListInt;

class ListListInt {
public:
  int size;
  List<int> things;

  SubListListInt operator[](int i);
  void SetSize(int s);
};

class SubListListInt {
public:
  ListListInt* viewed;
  int sublist;

  int& operator[](int i);
  void SetSize(int s);
};

SubListListInt ListListInt::operator[](int in) {
  if (in >= this->size) {
    crash << "Attempt to access invalid sublist " << in << " from list " << this->things << crash;
  }
  SubListListInt out;
  out.viewed = this;
  out.sublist = in;
  return out;
}

void ListListInt::SetSize(int newsz) {
  int deltas = newsz - size;
  if (deltas == 0) {
    return;
  }
  if (deltas < 0) {
    this->things.SetSize(things[newsz + 1]);
    size = newsz;
  } else {
    if (newsz < this->things[0]) {
      for (int i = size; i < newsz; i ++) {
        this->things[i] = this->things.size;
      }
    } else {
      int oldos = this->things.size;
      int newos = this->things.size + deltas;
      this->things.SetSize(this->things.size + deltas);
      memmove(this->things.TheObjects + newsz, this->things.TheObjects + size, oldos - size);
      for (int i = 0; i < size; i ++) {
        this->things[i] += deltas;
      }
      for (int i = size; i < newsz; i ++) {
        this->things[i] = newos;
      }
    }
  }
}

int& SubListListInt::operator[](int in) {
  int slp = viewed->things[sublist];
  if ((viewed->size - 1) == sublist) {
    if (slp + in >= viewed->things.size) {
      crash << "Attempt to access out of bounds index " << in << " from sublist " << sublist << crash;
    }
  } else {
    int nslp = viewed->things[sublist + 1];
    if (slp + in >= nslp) {
      crash << "Attempt to access out of bounds index " << in << " from sublist " << sublist << crash;
    }
  }
  return viewed->things[slp + in];
}

void SubListListInt::SetSize(int newsz) {
  int slp = viewed->things[sublist];
  if ((viewed->size - 1) == sublist) {
    viewed->things.SetSize(slp+newsz);
    return;
  }
  int nslp = viewed->things[sublist + 1];
  int sz = (nslp - slp);
  int deltas = newsz - sz;
  if (deltas == 0) {
    return;
  }
  if (deltas < 0) {
    memmove(viewed->things.TheObjects + nslp + deltas, viewed->things.TheObjects + nslp, viewed->things.size - nslp);
    viewed->things.SetSize(viewed->things.size + deltas);
  } else {
    viewed->things.SetSize(viewed->things.size + deltas);
    memmove(viewed->things.TheObjects + nslp + deltas, viewed->things.TheObjects + nslp, viewed->things.size - nslp);
  }
}

void WeylElementPermutesRootSystem(const ElementWeylGroup& g, PermutationR2& p) {
  int rss = g.owner->RootSystem.size;
  List<bool> accountedFor;
  accountedFor.SetSize(rss);
  for (int i = 0; i < rss; i ++) {
    accountedFor[i] = false;
  }
  for (int i = 0; i < rss; i ++) {
    if (accountedFor[i]) {
      continue;
    }
    p.cycles.SetSize(p.cycles.size + 1);
    p.cycles[p.cycles.size - 1].size = 0;
    int j = i;
    do {
      p.cycles[p.cycles.size- 1].AddOnTop(j);
      accountedFor[j] = true;
      j = g.owner->RootSystem.GetIndex(g * g.owner->RootSystem[j]);
    } while (j != i);
    if (p.cycles[p.cycles.size - 1].size == 1) {
      p.cycles.SetSize(p.cycles.size - 1);
    }
  }
}

//std::ostream& operator<<(std::ostream& out, const PermutationGroup& p)
//{ for (int i = 0; i <p.cycles.size; i ++)
//  { out << '(';
//    for (int j = 0; j<p.cycles[i].size; j ++)
//    { out << p.cycles[i][j];
//      if (j != p.cycles[i].size- 1)
//        out << ' ';
//    }
//    out << ')';
//  }
//  return out;
//}




//List<List<Vector<Rational> > > eigenspaces(const Matrix<Rational> &M, int checkDivisorsOf= 0);

template <typename somegroup>
void PrintCharTable(const somegroup& G, const char* filename) {
  JSData data;
  data.theType = JSData::token::tokenObject;
  JSData& representatives = data.objects.GetValueCreate("representatives");
  JSData& sizes = data.objects.GetValueCreate("sizes");
  JSData& characters = data.objects.GetValueCreate("characters");
  representatives.theType = JSData::token::tokenArray;
  representatives.theList.SetSize(G.conjugacyClasses.size);
  for (int i = 0; i < G.conjugacyClasses.size; i ++) {
    List<int> reprefs;
    G.GetWord(G.conjugacyClasses[i][0], reprefs);
    representatives.theList[i].theType = JSData::token::tokenArray;
    representatives.theList[i].theList.SetSize(reprefs.size);
    for (int j = 0; j < reprefs.size; j ++) {
      representatives.theList[i].theList[j].theType = JSData::token::tokenLargeInteger;
      int fixme;
      //representatives.theList[i].theList[j].theInteger.GetElement() = reprefs[j];
    }
  }
  sizes.theType = JSData::token::tokenArray;
  sizes.theList.SetSize(G.conjugacyClasses.size);
  for (int i = 0; i < G.conjugacyClasses.size; i ++) {
    sizes.theList[i].theType = JSData::token::tokenLargeInteger;
    int fixme;
    //sizes.theList[i].theInteger.GetElement() = G.conjugacyClasses[i].size;
  }
  characters.theType = JSData::token::tokenArray;
  characters.theList.SetSize(G.characterTable.size);
  for (int i = 0; i < G.characterTable.size; i ++) {
    for (int j = 0; j < G.characterTable[i].size; j ++) {
      characters[i][j] = G.characterTable[i][j].GetDoubleValue();
    }
  }
  if (filename) {
    std::ofstream out;
    out.open(filename);
    data.IntoStream(out, false, true);
  } else {
    data.IntoStream(stOutput, false, true);
  }
}

template <typename somegroup>
void AddCharTable(JSData& chartable, somegroup& G) {
  std::string sizes = "sizes";

  // check sizes
  for (int i = 0; i < G.conjugacyClasses.size; i ++) {
    int fixme;
    //if (chartable[sizes][i].theInteger.GetElement() != G.conjugacyClasses[i].size) {
    //  stOutput << "Size mismatch in conjugacy class " << i;
    //}
  }
  // check representatives... well, not yet.
  // load characters

  G.characterTable.SetSize(chartable["characters"].theList.size);
  for (int i = 0; i < chartable["characters"].theList.size; i ++) {
    G.characterTable[i].SetSize(G.conjugacyClasses.size);
    for (int j = 0; j < G.conjugacyClasses.size; j ++) {
      int fixme;
      //G.characterTable[i][j] = chartable["characters"][i][j].theInteger.GetElement();
    }
  }
}

template <typename coefficient>
List<VectorSpace<coefficient> > GetLeftEigenspaces(Matrix<coefficient>& M) {
  M.Transpose();
  return GetEigenspaces(M);
}

template <typename weylgroup>
List<int> FindConjugacyClassRepresentatives(weylgroup& G, int ncc) {
  if (G.theElements.size == 0) {
    G.ComputeAllElements();
  }
  stOutput << "Computing conjugacy class representatives... ";
  // hashing these might be nice
  // need to figure out how to pass MathRoutines::IntUnsignIdentity in
  List<List<int> > cycleTypes;
  List<int> representatives;
  for (int gi = 0; gi < G.theElements.size; gi ++) {
    PermutationR2 pi;
    WeylElementPermutesRootSystem(G.theElements[gi], pi);
    List<int> cycleType;
    pi.GetCycleStructure(cycleType);
    stOutput << pi;
    for (int i = 1; i < cycleType.size; i ++) {
      stOutput << ' ' << cycleType[i];
    }
    stOutput << "\n";
    if (cycleTypes.GetIndex(cycleType) == - 1) {
      cycleTypes.AddOnTop(cycleType);
      representatives.AddOnTop(gi);
      //stOutput << representatives.size << ' ';
      stOutput << "^ New cycle type, now have " << representatives.size << "\n";
      if (representatives.size == ncc) {
        break;
      }
    }
  }
  return representatives;
}

//template <typename somegroup>
//template <typename grouprep>
template <typename elementSomeGroup>
void ComputeIrreps(FiniteGroup<elementSomeGroup>& G) {
  List<int> charactersWithIrreps;
  List<int> charactersWithoutIrreps;
  for (int i = 0; i < G.irreps.size; i ++) {
    charactersWithIrreps.AddOnTop(G.characterTable.BSGetIndex(G.irreps[i].GetCharacter()));
    stOutput << G.irreps[i].GetCharacter() << ": "
    << G.characterTable.BSIndexFirstGreaterThan(G.irreps[i].GetCharacter())
    << G.characterTable.BSGetIndex(G.irreps[i].GetCharacter()) << "\n";
  }
  stOutput << "characters with irreps " << charactersWithIrreps << "\n";
  for (int i = 0; i < G.characterTable.size; i ++) {
    if (!charactersWithIrreps.BSContains(i)) {
      charactersWithoutIrreps.AddOnTop(i);
    }
  }
  stOutput << "need reps for chars" << charactersWithoutIrreps << "\n";
  Matrix<Rational> M;
  M.init(G.characterTable.size, G.characterTable.size);
  Rational SizeG = G.GetSize();
  for (int i = 0; i < G.characterTable.size; i ++) {
    for (int j = 0; j < G.characterTable.size; j ++) {
      M(j, i) = G.characterTable[j][i] * G.conjugacyClasseS[i].size / SizeG;
    }
  }
  for (int i = 0; i < G.characterTable.size; i ++) {
    for (int j = i; j < G.characterTable.size; j ++) {
      stOutput << i << "⊗" << j << ": " << M * (G.characterTable[i] * G.characterTable[j]).data << "\n";
    }
  }
}

/*
// this is incorrect.  Do not use it.
void PseudoParabolicSubgroupNope(WeylGroup* G, const Selection& sel, SubgroupRootReflections& out) {
  out.init();
  out.parent = G;
  int d = sel.CardinalitySelection + 1;
  out.generatorPreimages.SetSize(d);
  // not sure if this is actually how this works
  for (int i = 0; i <d- 1; i ++)
    out.generatorPreimages[i] = i + 1;

  out.SubCartanSymmetric.init(d,d);
  for (int ii = 0; ii <d- 1; ii ++)
    for (int jj = 0; jj<d- 1; jj ++)
      out.SubCartanSymmetric.elements[ii][jj] = G->CartanSymmetric.elements[sel.elements[ii]][sel.elements[jj]];

  // the highest root was the last one in the RootSystem
  Vector<Rational> hr = G->RootSystem[G->RootSystem.size- 1];

  for (int i = 0; i <d- 1; i ++) {
    Vector<Rational> v;
    G->CartanSymmetric.GetVectorFromRow(sel.elements[i],v);
    Rational x = v.ScalarEuclidean(hr);
    out.SubCartanSymmetric.elements[i][d- 1] = x;
    out.SubCartanSymmetric.elements[d- 1][i] = x;
  }
  out.SubCartanSymmetric.elements[d- 1][d- 1] = hr.ScalarProduct(hr,G->CartanSymmetric);
  out.generatorPreimages[d- 1] = G->theElements.GetIndex(G->GetRootReflection(G->RootSystem.size- 1));
}
*/

template <typename weylgroup>
void PrettyPrintTauSignatures(weylgroup& G, JSData& data, bool pseudo = false) {
  if (G.characterTable.size == 0) {
    ComputeCharacterTable(G);
  }
  /* this doesn't belong here
     Selection sel;
     sel.MakeFullSelection(G.CartanSymmetric.NumCols);
     sel.RemoveSelection(0);
     SubgroupWeylGroup H;
     ParabolicSubgroup(&G,sel,H);
     stOutput << G.CartanSymmetric.ToString(&consoleFormat) << "\n";
     H.ComputeIrreducibleRepresentations();
     stOutput << "subgroup" << "\n";
     stOutput << H.ToString(&consoleFormat) << "\n";
     stOutput << "sigs" << "\n";
     H.ComputeTauSignature();
     stOutput << H.tauSignature << "\n";
  */
  List<List<bool> > ts;
  AllTauSignatures(&G, ts, true);
  for (int i = 0; i < ts.size; i ++) {
    for (int j = 0; j < ts[i].size; j ++) {
      data[i][j] = ts[i][j];
    }
  }
  /* this prints out everything for some reason
  for (int i = 0; i <G.conjugacyClasses.size; i ++) {
   stOutput << "conjugacy class " << i << "\n";
   for (int j = 0; j<G.conjugacyClasses[i].size; j ++) {
    ElementWeylGroup<WeylGroup> g = G.theElements[G.conjugacyClasses[i][j]];
     for (int k = 0; k<g.size; k++)
      stOutput << g[k] << ' ';
     stOutput << "\n";
   }
   stOutput << "\n";
  }*/

// lets print out the character table
  stOutput << "\\[ \\begin{array}{";
  for (int i = 0; i < G.conjugacyClasses.size + 1; i ++) {
    stOutput << 'c';
  }
  stOutput << "}\n";

  for (int i = 0; i < G.conjugacyClasses.size; i ++) {
    List<int> g;
    G.GetWord(G.conjugacyClasses[i][0], g);
    stOutput << "&\\rotatebox{90}{";
    for (int k = 0; k < g.size; k ++) {
      stOutput << g[k] << ' ';
    }
    stOutput << "}\t";
  }
  stOutput << "\\\\" << "\n";

  for (int i = 0; i < G.conjugacyClasses.size; i ++) {
    stOutput << '&' << G.conjugacyClasses[i].size << '\t';
  }
  stOutput << "\\\\\n\\hline\n";

  for (int i = 0; i < G.characterTable.size; i ++) {
    for (int j = 0; j < G.conjugacyClasses.size; j ++) {
      stOutput << '&' << G.characterTable[i][j] << '\t';
    }
    stOutput << "\\\\\n";
  }
  stOutput << "\\end{array} \\]" << "\n";

// okay now the regular tau signatures
  Selection sel;
  sel.init(G.CartanSymmetric.NumCols);
  int numCycles =MathRoutines::TwoToTheNth(sel.MaxSize);
  stOutput << "\\[ \\begin{array}{";
  for (int i = 0; i < numCycles + 1; i ++) {
    stOutput << 'c';
  }
  stOutput << "}\n";
  for (int i = 0; i <numCycles; i ++) {
    stOutput << "&\\rotatebox{90}{(";
    for (int j = 0; j < sel.CardinalitySelection; j ++) {
      stOutput << sel.elements[j];
      if (j + 1 < sel.elements.size) {
        stOutput << ' ';
      }
    }
    stOutput << ")}\t";
    sel.incrementSelection();
  }

  stOutput << "\\\\" << "\n";
  for (int i = 0; i < ts.size; i ++) {
    for (int j = 0; j<numCycles; j ++) {
      stOutput << '&';
      stOutput << ts[i][j] << '\t';
    }
    stOutput << "\\\\\n";
  }
  stOutput << "\\end{array} \\]" << "\n";

// now the pseudo tau signatures
  if (pseudo) {


    if (ts[0].size > numCycles) {
      stOutput << "hr is ";
// WeylGroup is alittle different from AnotherWeylGroup<derp>
//    ElementWeylGroup<WeylGroup> hr = G.theElements[G.GetRootReflection(G.RootSystem.size- 1)];
      List<int> hr;
      G.GetWord(G.GetRootReflection(G.RootSystem.size - 1), hr);
      for (int i = 0; i < hr.size; i ++) {
        stOutput << hr[i] << ' ';
      }
      stOutput << "\n";

      // make enough columns
      stOutput << "\\[ \\begin{array}{";
      for (int i = 0; i < numCycles; i ++) {
        stOutput << 'c';
      }
      stOutput << "}\n";

      // labels at the top
      sel.init(G.CartanSymmetric.NumCols);
      for (int i = 0; i < numCycles - 1; i ++) {
        stOutput << "&\\rotatebox{90}{(";
        for (int j = 0; j<sel.CardinalitySelection; j ++) {
          stOutput << sel.elements[j] << ' ';
        }
        stOutput << "hr";
        stOutput << ")}\t";
        sel.incrementSelection();
      }

      // data goes out here
      stOutput << "\\\\" << "\n";
      for (int i = 0; i < ts.size; i ++) {
        for (int j = numCycles; j < numCycles + numCycles - 1; j ++) {
          stOutput << '&';
          stOutput << ts[i][j] << '\t';
        }
        stOutput << "\\\\\n";
      }
      stOutput << "\\end{array} \\]" << "\n";
    }
  }
}

/*
  for (int j = 0; j<PSGs[i].ccCount; j ++) {
    CoxeterElement h = PSGs[i].GetCoxeterElement(PSGs[i].conjugacyClasses[j][0]);
    g.reflections.SetSize(h.reflections.size);
    for (int k = 0; k<h.reflections.size; k++)
      g.reflections[k] = sel.elements[h.reflections[k]];
    int gi = this->GetIndexOfElement(g);
    for (int k = 0; k< this->ccCount; k++)
      for (int l = 0; l< this->conjugacyClasses[k].size; l ++)
        if (this->conjugacyClasses[k][l] == gi) {
          ccBackMaps[i][j] = k;
          goto endloop;
        }
    endloop:;
  }
*/


/*
template <typename coefficient>
List<CoxeterRepresentation<coefficient> > CoxeterRepresentation<coefficient>::Decomposition(List<ClassFunction<coefficient> >& ct, List<CoxeterRepresentation<coefficient> > &gr) {
   List<CoxeterRepresentation<coefficient> > out;
   if (GetNumberOfComponents() == 1) {
     if (ct.GetIndex(character) == - 1) {
       stOutput << "new irrep found, have " << ct.size << "\n";
       ct.AddOnTop(character);
       gr.AddOnTop(*this);
     }
     out.AddOnTop(*this);
     return out;
   }
   List<Vector<coefficient> > Vb = basis;
   for (int i = 0; i <ct.size; i ++) {
     if (character.IP(ct[i]) != 0) {
       stOutput << "contains irrep " << i << "\n";
       Matrix<coefficient> M = ClassFunctionMatrix(ct[i]);
       Vb = intersection(Vb,DestructiveKernel(M));
     }
   }
   if ((Vb.size < basis.size) && (Vb.size > 0)) {
     CoxeterRepresentation<coefficient> V;
     V.G = G;
     V.gens = gens;
     V.classFunctionMatrices = classFunctionMatrices;
     V.basis = Vb;
     V = V.Reduced();
     stOutput << "Decomposing remaining subrep " << V.GetCharacter() << "\n";
     return V.Decomposition(ct,gr);
   }
   if (Vb.size == 0)
    return out;
   SpaceTree<Rational> st;
   st.space = basis;
   for (int cfi = 0; cfi <G->ccCount; cfi ++) {
      ClassFunction<coefficient> cf;
      cf.G = G;
      cf.MakeZero();
      cf[cfi] = 1;
      stOutput << "getting matrix " << cf << "\n";
      Matrix<coefficient> A = ClassFunctionMatrix(cf);
      List<List<Vector<coefficient> > > es = eigenspaces(A);
      stOutput << "eigenspaces were ";
      for (int i = 0; i <es.size; i ++)
         stOutput << es[i].size << " ";
      stOutput << "\n";
      for (int i = 0; i <es.size; i ++)
         st.PlaceInTree(es[i]);
      stOutput << "tree is ";
      st.DisplayTree();
      stOutput << "\n";
   }
   List<List<Vector<coefficient> > > leaves;
   st.GetLeaves(leaves);
   stOutput << "leaves are ";
   for (int i = 0; i <leaves.size; i ++)
      stOutput << leaves[i].size << " ";
   stOutput << "\n";

   for (int i = 0; i <leaves.size; i ++) {
      CoxeterRepresentation<coefficient> outeme;
      outeme.G = G;
      outeme.gens = gens;
      outeme.classFunctionMatrices = classFunctionMatrices;
      outeme.basis = leaves[i];
      out.AddOnTop(outeme.Reduced());
   }
   return out;
}
*/

/*template <typename coefficient>
List<CoxeterRepresentation<coefficient> > CoxeterRepresentation<coefficient>::Decomposition() const {
  List<CoxeterRepresentation<coefficient> > out;
  if (GetNumberOfComponents() == 1) {
    out.AddOnTop(*this);
    return out;
  }
  List<List<Vector<coefficient> > > spaces;
  spaces.AddOnTop(basis);
  List<bool> is_component;
  is_component.AddOnTop(false);
  int dim_components = 0;
  for (int cfi = 0; cfi <G->ccCount; cfi ++) {
    if (dim_components == basis.size)
      break;
    ClassFunction<coefficient> cf;
    cf.G = G;
    cf.MakeZero();
    cf[cfi] = 1;
    stOutput << "getting matrix" << cf << "\n";
    Matrix<coefficient> A = ClassFunctionMatrix(cf);
    stOutput << A.ToString(&consoleFormat) << "\n";
    stOutput << "getting eigenspaces" << "\n";
    List<List<Vector<coefficient> > > es = eigenspaces(A);
    for (int i = 0; i <es.size; i ++) {
      if (dim_components == basis.size)
        break;
      int spaces_to_check = spaces.size;
      for (int j = 0; j<spaces_to_check; j ++) {
        if (is_component[j])
          continue;
        List<Vector<coefficient> > middle = intersection(es[i],spaces[j]);
        if (!((0<middle.size)&&(middle.size<spaces[j].size)))
          continue;
        stOutput << "eigenspace " << i << " intersects with space " << j << "\n";
        List<Vector<coefficient> > right = relative_complement(spaces[j],middle);
        spaces[j] = middle;
        spaces.AddOnTop(right);
        CoxeterRepresentation<coefficient> midr;
        midr.G = G;
        midr.gens = gens;
        midr.basis = middle;
        if (midr.GetNumberOfComponents() == 1) {
          is_component[j] = true;
          dim_components += middle.size;
        }
        CoxeterRepresentation<coefficient> rightr;
        rightr.G = G;
        rightr.gens = gens;
        rightr.basis = right;
        if (rightr.GetNumberOfComponents() == 1) {
          is_component.AddOnTop(true);
          dim_components += right.size;
        } else {
          is_component.AddOnTop(false);
        }
      }
    }
  }
  stOutput << "spaces.size " << spaces.size << "\n";
  for (int i = 0; i <spaces.size; i ++) {
    CoxeterRepresentation<coefficient> s;
    s.G = this->G;
    s.gens = this->gens;
    s.basis = spaces[i];
    out.AddOnTop(s);
  }
  stOutput << "decomposition might be complete, found " << out.size << " components" << "\n";
  return out;
}*/


/*
template <typename coefficient>
class polynom1al
{
  public:
  List<coefficient> data;


  void AddMonomial(coefficient c, int i) {
    while (this->data.size < i + 1) {
      this->data.AddOnTop(0);
    }
    this->data[i] += c;
  }


  // if only cxx had lisp macros...
  polynom1al<coefficient> operator+(const polynom1al &right) const {
    polynom1al<coefficient> out;
    out.data.SetSize((right.data.size >this->data.size)?right.data.size:this->data.size);
    int i = 0;
    for (; i < this->data.size && i <right.data.size; i ++)
      out.data[i] = this->data[i] + right.data[i];
    for (; i < this->data.size; i ++)
      out.data[i] = this->data[i];
    for (; i <right.data.size; i ++)
      out.data[i] = right->data[i];
    return out;
  }

  polynom1al<coefficient> operator-(const polynom1al &right) const {
    polynom1al<coefficient> out;
    out.data.SetSize((right.data.size >this->data.size)?right.data.size:this->data.size);
    int i = 0;
    for (; i < this->data.size && i <right.data.size; i ++)
      out.data[i] = this->data[i] - right.data[i];
    for (; i < this->data.size; i ++)
      out.data[i] = this->data[i];
    for (; i <right.data.size; i ++)
      out.data[i] = -right->data[i];
    return out;
  }

  polynom1al<coefficient> operator*(const polynom1al &right) const {
    polynom1al<coefficient> out;
    out.data.SetSize(tmp.data.size * right.data.size);
    for (int i = 0; i < this->data.size; i ++)
      for (int j = 0; j<right.data.size; j ++)
        out.data[i*j] = this->data[i]*right.data[j];
    return out;
  }
  void operator*=(const polynom1al &right) {
    polynom1al<coefficient> tmp = *this;
    this->data.SetSize(tmp.data.size * right.data.size)
    for (int i = 0; i < tmp.data.size; i ++)
      for (int j = 0; j<right.data.size; j ++)
        this->data[i*j] = tmp.data[i]*right.data[j];
  }

  void Invert() {
    for (int i = 0; i < this->data.size; i ++)
      this->data[i].Invert();
  }

  bool IsEqualToZero() {
    return this->data.size == 0;
  }


  static List<polynom1al<coefficient> > LagrangeInterpolants(List<int> points) {


  }

  polynom1al(){}
  polynom1al(int c){this->data.AddOnTop(c);}


  void operator=(int right) {
    this->data.SetSize(1);
    this->data[0] = right;

  }
  void operator=(coefficient right) {
    this->data.SetSize(1);
    this->data[0] = right;
  }

};

template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const polynom1al<coefficient>& p) {
  out << p.data;
  return out;
}
*/
template <typename coefficient>
bool space_contains(const List<Vector<coefficient> >& V, Vector<coefficient>& v) {
  if (v.IsEqualToZero()) {
    if (V.size == 0) {
      return false;
    }
    return true;
  }
  int i = 0;
  while (true) {
    int vi = v.GetIndexFirstNonZeroCoordinate();
    while (true) {
      if (i == V.size) {
        return false;
      }
      int vii = V[i].GetIndexFirstNonZeroCoordinate();
      if (vii > vi) {
        return false;
      }
      if (vii == vi) {
        break;
      }
      i ++;
    }
    v -= V[i] * v[vi] / V[i][vi];
    if (v.IsEqualToZero()) {
      return true;
    }
  }
}

template<typename coefficient>
List<Vector<coefficient> > getunion(const List<Vector<coefficient> >& V, const List<Vector<coefficient> >& W) {
  if (V.size == 0) {
    return W;
  }
  int d = V[0].size;
  Matrix<coefficient> M;
  M.init(V.size + W.size,d);
  for (int i = 0; i < V.size; i ++) {
    for (int j = 0; j < d; j ++) {
      M.elements[i][j] = V[i][j];
    }
  }
  for (int i = 0; i < W.size; i ++) {
    for (int j = 0; j < d; j ++) {
      M.elements[V.size + i][j] = W[i][j];
    }
  }
  M.GaussianEliminationByRows();
  List<Vector<coefficient> > out;
  for (int i = 0; i < M.NumRows; i ++) {
    Vector<coefficient> v;
    v.SetSize(d);
    for (int j = 0; j < d; j ++) {
      v[j] = M.elements[i][j];
    }
    if (v.IsEqualToZero()) {
      break;
    }
    out.AddOnTop(v);
  }
  return out;
}

template<typename coefficient>
List<Vector<coefficient> > intersection(const List<Vector<coefficient> >& V, const List<Vector<coefficient> >& W) {
  if ((V.size == 0) or (W.size == 0)) {
    List<Vector<coefficient> > out;
    return out;
  }
  int d = V[0].size;

  Matrix<coefficient> MV;
  MV.init(V.size, d);
  for (int i = 0; i < V.size; i ++) {
    for (int j = 0; j < d; j ++) {
      MV.elements[i][j] = V[i][j];
    }
  }
  List<Vector<coefficient> > Vperp = DestructiveKernel(MV);

  Matrix<coefficient> MW;
  MW.init(W.size, d);
  for (int i = 0; i < W.size; i ++) {
    for (int j = 0; j < d; j ++) {
      MW.elements[i][j] = W[i][j];
    }
  }
  List<Vector<coefficient> > Wperp = DestructiveKernel(MW);
  Matrix<coefficient> M;
  M.init(Vperp.size + Wperp.size, d);
  int i = 0;
  for (; i < Vperp.size; i ++) {
    for (int j = 0; j < d; j ++) {
      M.elements[i][j] = Vperp[i][j];
    }
  }
  for (; i < Vperp.size + Wperp.size; i ++) {
    for (int j = 0; j < d; j ++) {
      M.elements[i][j] = Wperp[i - Vperp.size][j];
    }
  }
  return DestructiveKernel(M);
}

template <typename coefficient>
List<Vector<coefficient> > orthogonal_complement(const List<Vector<coefficient> >& V, const List<Vector<coefficient> >& WW) {
  List<Vector<coefficient> > W = intersection(V,WW);
  if (W.size == 0) {
    return V;
  }
  int d = W[0].size;
  Matrix<coefficient> GM;
  GM.init(W.size, W.size);
  for (int i = 0; i < W.size; i ++) {
    for (int j = 0; j < W.size; j ++) {
      GM.elements[i][j] = W[i].ScalarEuclidean(W[j]);
    }
  }
  GM.Invert();
  Matrix<coefficient> VM;
  VM.init(W.size, V.size);
  for (int i = 0; i < W.size; i ++) {
    for (int j = 0; j < V.size; j ++) {
      VM.elements[i][j] = W[i].ScalarEuclidean(V[j]);
    }
  }
  VM.MultiplyOnTheLeft(GM);
  Matrix<coefficient> outm;
  outm.init(V.size, d);
  for (int i = 0; i < V.size; i ++) {
    for (int j = 0; j < d; j ++) {
      coefficient r = V[i][j];
      outm.elements[i][j] = r;
      for (int k = 0; k < W.size; k ++) {
        outm.elements[i][j] -= VM.elements[k][i] * W[k][j];
      }
    }
  }
  outm.GaussianEliminationByRows();
  List<Vector<coefficient> > out;
  for (int i = 0; i < outm.NumRows; i ++) {
    Vector<coefficient> v;
    v.SetSize(d);
    for (int j = 0; j < d; j ++) {
      v[j] = outm.elements[i][j];
    }
    if (v.IsEqualToZero()) {
      return out;
    }
    out.AddOnTop(v);
  }
  return out;
}

// we're going to guess at integers
template <typename coefficient>
bool pdiv(List<coefficient>& p, int a) {
  List<coefficient> q;
  coefficient lastround = p[0];
  for (int i = 1; i < p.size; i ++) {
    q.AddOnTop(lastround);
    lastround = p[i] - lastround*a;
  }
  if (lastround == 0) {
    p = q;
    return true;
  }
  return false;
}

template <typename coefficient>
List<int> factorpoly(List<coefficient> p, int maxfac) {
  List<int> factors;
  for (int i1 = 0; i1 < maxfac; i1 ++) {
    for (int i2 = 1; i2 >= - 1; i2 -= 2) {
      int i = i1 * i2;
      while (pdiv(p, i)) {
        if (!factors.Contains(i)) {
          factors.AddOnTop(i);
        }
        if (p.size == 1) {
          return factors;
        }
      }
    }
  }
  return factors;
}

template <typename coefficient>
List<Vector<coefficient> > DestructiveKernel(Matrix<coefficient>& M) {
  Selection s;
//  stOutput << "DestructiveKernel: GaussianEliminationByRows" << "\n";
  M.GaussianEliminationByRows(0, 0, s);
//  stOutput << "DestructiveKernel: calculating kernel space" << "\n";
  List<Vector<coefficient> > V;
  for (int i = 0; i < s.selected.size; i ++) {
    if (!s.selected[i]) {
      continue;
    }
    Vector<coefficient> v;
    v.MakeEi(M.NumCols, i);
    int rowmap = 0;
    for (int j = 0; j < M.NumCols; j ++) {
      if (!s.selected[j]) {
        v[j] = - M.elements[rowmap][i];
        rowmap ++;
      }
    }
    V.AddOnTop(v);
  }
//  stOutput << "DestructiveKernel: done" << "\n";
  return V;
}

template <typename coefficient>
List<Vector<coefficient> > DestructiveEigenspace(Matrix<coefficient>& M, const coefficient& l) {
  for (int i = 0; i < M.NumCols; i ++) {
    M.elements[i][i] -= l;
  }
  return DestructiveKernel(M);
}

template <typename coefficient>
List<Vector<coefficient> > DestructiveColumnSpace(Matrix<coefficient>& M) {
  M.Transpose();
  M.GaussianEliminationByRows();
  List<Vector<coefficient> > out;
  bool zerov;
  for (int i = 0; i < M.NumRows; i ++) {
    Vector<coefficient> v;
    v.MakeZero(M.NumCols); // initializing is not necessary
    zerov = true;
    for (int j = 0; j < M.NumCols; j ++) {
      v[j] = M.elements[i][j];
      if (zerov && M.elements[i][j] != 0) {
        zerov = false;
      }
    }
    if (zerov) {
      return out;
    }
    out.AddOnTop(v);
  }
}

template <typename coefficient>
Vector<coefficient> PutInBasis(const Vector<coefficient>& v, const List<Vector<coefficient> >& B) {
  Vector<coefficient> w;
  w.MakeZero(B.size);
  Matrix<coefficient> M;
  M.MakeZeroMatrix(B.size);
  for (int i = 0; i < B.size; i ++) {
    for (int j = 0; j < v.size; j ++) {
      w[i] += B[i][j] * v[j];
    }
    for (int j = 0; j < B.size; j ++) {
      for (int k = 0; k < v.size; k ++) {
        M.elements[i][j] += B[i][k] * B[j][k];
      }
    }
  }
  M.Invert();
  Vector<coefficient> v2 = M * w;
  Vector<coefficient> v3;
  v3.MakeZero(v.size);
  for (int i = 0; i < B.size; i ++) {
    v3 += B[i] * v2[i];
  }
  if (!(v3 == v)) {
    Vector<coefficient> out;
    return out;
  }
  return v2;
}

template <typename elementSomeGroup, typename coefficient>
Vector<coefficient> ActOnGroupRing(const FiniteGroup<elementSomeGroup>& G, int g, const Vector<coefficient>& v) {
  Vector<coefficient> out;
  out.MakeZero(G.theElements.size);
  for (int i = 0; i < G.theElements.size; i ++) {
    if (v[i] != 0) {
      out[G.MultiplyElements(g, i)] = v[i];
    }
  }
  return out;
}

// this function name is a lie
template <typename elementSomeGroup, typename coefficient>
bool is_isotypic_component(FiniteGroup<elementSomeGroup>& G, const List<Vector<coefficient> >& V) {
  // pre-initial test: V isn't empty
  if (V.size == 0) {
    return false;
  }
  // initial test: dimension of V is a square
  int n = sqrt(V.size);
  if (n * n != V.size) {
    return false;
  }
  // more expensive test: character of V has unit norm
  ClassFunction<FiniteGroup<elementSomeGroup>, coefficient> X;
  X.G = &G;
  X.data.SetSize(G.conjugacyClasseS.size);
  for (int i = 0; i < G.conjugacyClasseS.size; i ++) {
    int g = G.conjugacyClasseS[i].representative;
    coefficient tr = 0;
    for (int j = 0; j < V.size; j ++) {
      Vector<coefficient> v = ActOnGroupRing(G, g, V[j]);
      v = PutInBasis(v, V);
      tr += v[j];
    }
    X.data[i] = tr;
  }
  // (4, - 1, - 1/2, 1/2, - 1, 1/4, 1/4, 1, 1/2, -7/2)[1] haha yeah right
  if (X.norm() != n) {
    return false;
  }
  // okay now do the really hard test
  Matrix<coefficient> M = GetMatrix(X);
  List<List<Vector<coefficient> > > spaces = eigenspaces(M);
  if (spaces.size != 2) {
    return false;
  }
  if (spaces[1].size != V.size) {
    return false;
  }
  if (intersection(spaces[1],V).size != V.size) {
    return false;
  }
  return true;
}


template <typename elementSomeGroup, typename coefficient>
Matrix<coefficient> GetMatrix(const ClassFunction<FiniteGroup<elementSomeGroup>, coefficient>& X) {
  Matrix<coefficient> M;
  M.MakeZeroMatrix(X.G->N);
  for (int i1 = 0; i1 < X.G->ccCount; i1 ++) {
    for (int i2 = 0; i2 < X.G->ccSizes[i1]; i2 ++) {
      int i = X.G->conjugacyClasses[i1][i2];
      for (int j = 0; j < X.G->N; j ++) {
        M.elements[X.G->MultiplyElements(i, j)][j] = X.data[i1];
      }
    }
  }
  return M;
}

int skipcount(int n, const List<int>& l) {
  int o = 0;
  for (int i = 0; i < l.size; i ++) {
    if (l[i] == n) {
      return - 1;
    }
    if (l[i] < n) {
      o ++;
    }
  }
  return n - o;
}

int popcnt(int i) {
  // from the internet
  i = i - ((i >> 1) & 0x55555555);
  i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
  return ((((i + (i >> 4)) & 0xF0F0F0F) * 0x1010101) & 0xffffffff) >> 24;
}

List<List<int> > powersetgrade(int G, const List<int>& l) {
  List<List<int> > out;
  for (int i = 0; i < 1 << l.size; i ++) {
    if (popcnt(i) == G) {
      List<int> outi;
      for (int j = 0; j < l.size; j ++) {
        if (i & (1 << j)) {
          outi.AddOnTop(j);
        }
      }
      out.AddOnTop(outi);
    }
  }
  return out;
}

template<typename Element>
Element minor_det(const Matrix<Element>& M, const List<int>& l) {
  Matrix<Element> MM;
  MM.init(M.NumRows - l.size, M.NumCols - l.size);
  for (int i = 0; i < M.NumRows; i ++) {
    int ii = skipcount(i, l);
    if (ii == - 1) {
      continue;
    }
    for (int j = 0; j < M.NumCols; j ++) {
      int jj = skipcount(j, l);
      if (jj == - 1) {
        continue;
      }
      MM.elements[ii][jj] = M.elements[i][j];
    }
  }
//  Element x;
//  MM.ComputeDeterminantOverwriteMatrix(&x);
//  return x;
  return MM.GetDeterminant();
}

template<typename Element>
List<Element> charpoly(const Matrix<Element>& M) {
  List<Element> p;
  p.SetSize(M.NumRows + 1);
  List<int> rowids;
  for (int i = 0; i < M.NumRows; i ++) {
    rowids.AddOnTop(i);
  }
  for (int i = 0; i < M.NumRows; i ++) {
    Element acc = 0;
    List<List<int> > ps = powersetgrade(i, rowids);
    for (int si = 0; si < ps.size; si ++) {
      acc += minor_det(M, ps[si]);
    }
    if (i % 2 == 0) {
      acc = -acc;
    }
    p[M.NumRows-i] = acc;
  }
  p[0] = ((M.NumRows % 2) == 0) ? - 1 : 1;
  return p;
}

void BSTest() {
  List<int> l;
  stOutput << "I am inserting without duplication 1, 1, 5, 5, 1, 3, 5 to";
  stOutput << "an empty list.  They are going in positions ";
  stOutput << l.BSInsertDontDup(1) << ' ';
  stOutput << l.BSInsertDontDup(1) << ' ';
  stOutput << l.BSInsertDontDup(5) << ' ';
  stOutput << l.BSInsertDontDup(5) << ' ';
  stOutput << l.BSInsertDontDup(1) << ' ';
  stOutput << l.BSInsertDontDup(3) << ' ';
  stOutput << l.BSInsertDontDup(5) << ' ';
  stOutput << "The list now contains " << l.ToStringCommaDelimited();
  stOutput << "\nI am inserting 1, 5, and 6 into the list, in positions ";
  stOutput << l.BSInsert(1) << ", " << l.BSInsert(5) << ", and ";
  stOutput << l.BSInsert(6) << ".\n";
  stOutput << "Now checking first not less than and first greater than:\n";
  stOutput << "list:                ";
  for (int i = 0; i < l.size; i ++) {
    stOutput << l[i] << " ";
  }
  stOutput << "\n";
  stOutput << "items:               ";
  for (int i = 0; i < 7; i ++) {
    stOutput << i << " ";
  }
  stOutput << "\n";
  stOutput << "first not less than: ";
  for (int i = 0; i < 7; i ++) {
    stOutput << l.BSIndexFirstNotLessThan(i) << " ";
  }
  stOutput << "\n";
  stOutput << "first greater than:  ";
  for (int i = 0; i < 7; i ++) {
    stOutput << l.BSIndexFirstGreaterThan(i) << " ";
  }
  stOutput << "\n";
}

int chartable[10][10] =
{ {1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
  {1, - 1, - 1,  1,  1, - 1,  1,  1, - 1, - 1},
  {3,  1,  1, - 1,  1,  0,  0, - 1, - 1, -3},
  {3, - 1, - 1, - 1,  1,  0,  0, - 1,  1,  3},
  {2,  0, -2,  0,  0,  1, - 1,  2,  0, -2},
  {2,  0,  2,  0,  0, - 1, - 1,  2,  0,  2},
  {1,  1, - 1, - 1, - 1, - 1,  1,  1,  1, - 1},
  {1, - 1,  1, - 1, - 1,  1,  1,  1, - 1,  1},
  {3, - 1,  1,  1, - 1,  0,  0, - 1,  1, -3},
  {3,  1, - 1,  1, - 1,  0,  0, - 1, - 1,  3}
};

void make_macdonald_polynomial(
  const WeylGroupData& G, const List<Vector<Rational> > roots, Polynomial<Rational>& macdonaldPoly
) {
  List<MonomialP> vars;
  vars.SetSize(G.GetDim());
  for (int i = 0; i < G.GetDim(); i ++) {
    vars[i].MakeEi(i);
  }

  macdonaldPoly.MakeOne();
  for (int i = 0; i <roots.size; i ++) {
    Polynomial<Rational> pj;
    for (int j = 0; j < G.GetDim(); j ++) {
      pj.AddMonomial(vars[j], roots[i][j]);
    }
    macdonaldPoly *= pj;
  }
}

void matrix_acts_on_polynomial(const Matrix<Rational>& m,const Polynomial<Rational>& p, Polynomial<Rational>& q) {
  q = q.GetZero();
  List<MonomialP> vars;
  vars.SetSize(m.NumCols);
  for (int i = 0; i < m.NumCols; i ++) {
    vars[i].MakeEi(i);
  }
  for (int i = 0; i < p.theMonomials.size; i ++) {
    Polynomial<Rational> qi;
    qi = qi.GetOne();
    for (int j = 0; j < p.theMonomials[i].GetMinNumVars(); j ++) {
      if (p.theMonomials[i](j) != 0) {
        Polynomial<Rational> qj;
        qj = qj.GetZero();
        for (int k = 0; k < m.NumCols; k ++) {
          qj.AddMonomial(vars[k], m.elements[k][j]);
        }
        qj.RaiseToPower((p.theMonomials[i](j)).floorIfSmall());
        qi *= qj;
      }
    }
    qi *= p.theCoeffs[i];
    q += qi;
  }
}

template <class templateMonomial, class coefficient>
bool GetCoordsInBasisInputIsGaussianEliminated(
  const List<MonomialCollection<templateMonomial, coefficient> > &inputReducedBasis,
  const MonomialCollection<templateMonomial, coefficient> & input,
  Vector<coefficient>* outputCoordinatesInBasis = 0
) {
  MacroRegisterFunctionWithName("GetCoordsInBasisInputIsGaussianEliminated");
  MonomialCollection<templateMonomial, coefficient>  summand, v = input;
  coefficient currentCoeff;
  if (outputCoordinatesInBasis != 0) {
    outputCoordinatesInBasis->MakeZero(inputReducedBasis.size);
  }
  for (int i = 0; i < inputReducedBasis.size; i ++) {
    currentCoeff=v.GetMonomialCoefficient(inputReducedBasis[i].GetMaxMonomial());
    if (!currentCoeff.IsEqualToZero()) {
      summand = inputReducedBasis[i];
      if (outputCoordinatesInBasis != 0) {
        (*outputCoordinatesInBasis)[i] = currentCoeff;
      }
      currentCoeff *= - 1;
      summand *= currentCoeff;
      v -= summand;
    }
  }
  return v.IsEqualToZero();
}

template <class coefficient>
bool GetCoordsInBasisInputIsGaussianEliminated(
  const List<Polynomial<coefficient> > &inputReducedBasis,
  const Polynomial<coefficient> & input,
  Vector<coefficient>* outputCoordinatesInBasis = 0
) {
  MacroRegisterFunctionWithName("GetCoordsInBasiS");
  Polynomial<coefficient> summand, v = input;
  coefficient currentCoeff;
  if (outputCoordinatesInBasis != 0) {
    outputCoordinatesInBasis->MakeZero(inputReducedBasis.size);
  }
  for (int i = 0; i < inputReducedBasis.size; i ++) {
    currentCoeff = v.GetMonomialCoefficient(inputReducedBasis[i].GetMaxMonomial());
    if (!currentCoeff.IsEqualToZero()) {
      summand = inputReducedBasis[i];
      if (outputCoordinatesInBasis != 0) {
        (*outputCoordinatesInBasis)[i] = currentCoeff;
      }
      currentCoeff *= - 1;
      summand *= currentCoeff;
      v -= summand;
    }
  }
  return v.IsEqualToZero();
}

/*
void get_macdonald_representations_of_weyl_group(SemisimpleLieAlgebra& theSSlieAlg) {
  WeylGroup& W = theSSlieAlg.theWeyl;


  rootSubalgebras theRootSAs;
  theGlobalVariables.SetStandardStringOutput(HtmlRoutines::MakeStdCoutReport);
  theRootSAs.theGlobalVariables = &theGlobalVariables;
  theRootSAs.owner = &theSSlieAlg;
  DynkinSimpleType dt = W.theDynkinType.GetGreatestSimpleType();
  theRootSAs.ComputeAllReductiveRootSubalgebrasUpToIsomorphism();

  List<Vector<Rational> > roots;

  for (int k = 0; k<theRootSAs.theSubalgebras.size; k++) {
    rootSubalgebra& currentRootSA = theRootSAs.theSubalgebras[k];
    roots = currentRootSA.PositiveRootsK;
    Polynomial<Rational> macdonaldPoly;
    make_macdonald_polynomial(W,roots,macdonaldPoly);
    stOutput << macdonaldPoly << "\n";
    Polynomial<Rational> p;
    Matrix<Rational> m;
    List<Polynomial<Rational> > module;
    stOutput << "I am processing root subalgebra of type "
              << currentRootSA.theDynkinDiagram.ToStringRelativeToAmbientType(W.theDynkinType[0]);
    module.AddOnTop(macdonaldPoly);
    int mcs = 0;
    do
    { mcs = module.size;
      for (int i =1; i <W.GetDim() + 1; i ++) {
        W.GetStandardRepresentationMatrix(i,m);
        for (int j = 0; j<mcs; j ++){
          matrix_acts_on_polynomial(m,module[j],p);
          module.AddOnTop(p);
        }
      }
      Polynomial<Rational>::GaussianEliminationByRowsDeleteZeroRows(module);
    } while (module.size > mcs);
    stOutput << "...rank is " << module.size << "\n";
    WeylGroupRepresentation<Rational> rep;
    rep.init(W);
    rep.names.SetSize(1);
    rep.names[0] = currentRootSA.theDynkinDiagram.ToStringRelativeToAmbientType(W.theDynkinType[0]);
    for (int i =1; i <W.GetDim() + 1; i ++) {
      Matrix<Rational> rm;
      rm.init(module.size, module.size);
      W.GetStandardRepresentationMatrix(i,m);
      for (int ji = 0; ji <module.size; ji ++) // would be neat if we
      { Vector<Rational> v;               // didnt need to go through
        matrix_acts_on_polynomial(m,module[ji],p); // stuff we've
        GetCoordsInBasisInputIsGaussianEliminated(module,p,&v); //
        for (int jj = 0; jj<module.size; jj ++) // already looked at, but
          rm(jj,ji) = v[jj]; // new dimensions can be inserted anywhere
      }
      rep.SetElementImage(i,rm);
    }
    stOutput << "has character ";
    stOutput << rep.GetCharacter() << "\n";
    W.AddIrreducibleRepresentation(rep);
    stOutput << "elapsed seconds: " << theGlobalVariables.GetElapsedSeconds() << "\n";
  }
}
*/

class lennum {
public:
  friend std::ostream& operator << (std::ostream& output, const lennum& input) {
    output << "num: " << input.num << " len: " << input.len.ToString();
    return output;
  }
  int num;
  Rational len;

  bool operator<(const lennum& y) const {
    return this->len < y.len;
  }
  bool operator>(const lennum& y) const {
    return this->len > y.len;
  }
};

GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup>, Rational>
get_macdonald_representation(WeylGroupData& W, const List<Vector<Rational> >& roots) {
  stOutput << "starting with roots " << roots << "\n";
  List<Vector<Rational> > monomial;
  List<List<Vector<Rational> > > monomials;
  Matrix<Rational> m;
  for (int i = 0; (Rational) i < W.theGroup.GetSize(); i ++) {
    W.GetStandardRepresentationMatrix(i,m);
    monomial.SetSize(roots.size);
    for (int j = 0; j < roots.size; j ++) {
      monomial[j] = m * roots[j];
      if (monomial[j].IsNegative()) {
        monomial[j] = - monomial[j];
      }
    }
    monomial.QuickSortAscending();
    monomials.BSInsertDontDup(monomial);
  }
  stOutput << "have all " << monomials.size << " monomials";

  List<lennum> lens;
  List<Vector<Rational> > images;
  images.SetSize(monomials.size);
  lens.SetSize(monomials.size);
  for (int i = 0; i < monomials.size; i ++) {
    images[i].SetSize(W.RootSystem.size);
    Rational l = 0;
    for (int j = 0; j < W.RootSystem.size; j ++) {
      Rational p = 1;
      for (int k1= 0; k1 < monomials[i].size; k1 ++) {
        Rational s = 0;
        for (int k2 = 0; k2 < monomials[i][k1].size; k2 ++) {
          s += monomials[i][k1][k2] * W.RootSystem[j][k2];
        }
        p *= s;
      }
      images[i][j] = p;
      l += p * p;
    }
    lens[i].len = l;
    lens[i].num = i;
  }
  lens.QuickSortAscending();
  stOutput << " ... sorted" << "\n";

  List<int> monomials_used;
  VectorSpace<Rational> vs;
  for (int i = 0; i < monomials.size; i ++) {
    if (vs.AddVector(images[lens[i].num])) {
      monomials_used.AddOnTop(lens[i].num);
    }
  }
  Basis<Rational> B;
  for (int i = 0; i < monomials_used.size; i ++) {
    B.AddVector(images[monomials_used[i]]);
  }

  stOutput << "module rank is " << monomials_used.size << "\n";

  GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup>, Rational> rep;
  rep.init(W.theGroup);
  for (int i = 1; i < W.GetDim() + 1; i ++) {
    Matrix<Rational> m;
    W.GetStandardRepresentationMatrix(i,m);
    Matrix<Rational> rm;
    rm.init(monomials_used.size, monomials_used.size);
    for (int j = 0; j < monomials_used.size; j ++) {
      List<Vector<Rational> > monomial;
      monomial.SetSize(roots.size);
      for (int k = 0; k < roots.size; k ++) {
        monomial[k] = m*monomials[monomials_used[j]][k];
      }
      Vector<Rational> evaluated;
      evaluated.SetSize(W.RootSystem.size);
      for (int ei = 0; ei < W.RootSystem.size; ei ++) {
        Rational p = 1;
        for (int ej = 0; ej < monomial.size; ej ++) {
          Rational s = 0;
          for (int ek = 0; ek < monomial[ej].size; ek ++) {
            s += monomial[ej][ek] * W.RootSystem[ei][ek];
          }
          p *= s;
        }
        evaluated[ei] = p;
      }
      Vector<Rational> be = B.PutInBasis(evaluated);
      rm.AssignVectorToColumnKeepOtherColsIntactNoInit(j,be);
    }
    rep.SetElementImage(i,rm);
    stOutput << rm.ToString(&consoleFormat) << "\n";
    if (!(rm*rm).IsIdMatrix()) {
      stOutput << "Error: this matrix is not an involution" << "\n";
    }
  }
  return rep;
}

int maxpoints;

// balanced ternary
Vector<int> pointi(int d, int i) {
  if (maxpoints == 0) {
    int tmp = 1;
    for (int i = 0; i < d; i ++) {
      tmp *= 3;
    }
    int tmp2 = 1;
    for (int i = 0; i < d; i ++) {
      tmp2 *= 5;
    }
    maxpoints = tmp + tmp2;
  }
  Vector<int> out;
  out.SetSize(d);
  int acc = 1;
  for (int i = 0; i < d; i ++) {
    acc *= 3;
  }
  if (i < acc) {
    for (int ii = 0; ii < d; ii ++) {
      int m = i % 3;
      out[ii] = (m != 2) ? m : - 1;
      i /= 3;
    }
    return out;
  }
  for (int ii = 0; ii < d; ii ++) {
    int m = i % 5;
    out[ii] = m - 2;
    i /= 5;
  }
  return out;
}

bool pointgt(const Vector<int>& pointi, const Vector<int>& pointj) {
  int is = 0;
  int im = 0;
  for (int i = 0; i <pointi.size; i ++) {
    is += pointi[i];
    if (im < abs(pointi[i])) {
      im = abs(pointi[i]);
    }
  }
  int js = 0;
  int jm = 0;
  for (int j = 0; j<pointj.size; j ++) {
    js += pointj[j];
    if (jm < abs(pointj[j])) {
      jm = abs(pointj[j]);
    }
  }
  if (abs(is) > abs(js)) {
    return true;
  }
  if (abs(js) < abs(is)) {
    return false;
  }
  if (im > jm) {
    return true;
  }
  if (im < jm) {
    return false;
  }
  return im > jm;
}

bool pointlt(const Vector<int>& pointi, const Vector<int>& pointj) {
  int is = 0;
  int im = 0;
  for (int i = 0; i < pointi.size; i ++) {
    is += pointi[i];
    if (im < abs(pointi[i])) {
      im = abs(pointi[i]);
    }
  }
  int js = 0;
  int jm = 0;
  for (int j = 0; j < pointj.size; j ++) {
    js += pointj[j];
    if (jm < abs(pointj[j])) {
      jm = abs(pointj[j]);
    }
  }
  if (abs(is) > abs(js)) {
    return false;
  }
  if (abs(js) < abs(is)) {
    return true;
  }
  if (im > jm) {
    return false;
  }
  if (im < jm) {
    return true;
  }
  return im < jm;
}

template<>
std::string Vector<int>::ToString(FormatExpressions* theFormat) const {
  (void) theFormat;//avoid unused parameter warning, portable
  std::stringstream out;
  out.precision(5);
  out << "(";
  for (int i = 0; i < this->size; i ++) {
    out << this->TheObjects[i];
    if (i != this->size - 1) {
      out << ", ";
    }
  }
  out << ")";
  return out.str();
}

Vector<int> pointi_slow(int d, int i) {
  static List<Vector<int> > points;
  static int old_d;
  if ((points.size == 0) || (old_d != d)) {
    stOutput << "generating points...";
    old_d = d;
    int n = 10 - d; // values of each coordinate range from -n to n
    Vector<int> point;
    point.SetSize(d);
    for (int j = 0; j < d; j ++) {
      point[j] = - n;
    }
    while (true) {
      for (int j = 0; j < d; j ++) {
        if (point[j] < n) {
          point[j] ++;
          points.AddOnTop(point);
          break;
        } else {
          if (j < d - 1) {
            point[j] = - n;
            continue;
          } else {
            goto done_generating_points;
          }
        }
      }
    }
    done_generating_points:
    //points.QuickSortAscending(&pointgt);
    std::sort(points.TheObjects, points.TheObjects + points.size, pointlt);
    for (int k = 0; k < points.size; k ++) {
      stOutput << points[k] << "\n";
    }
    stOutput << " " << points.size << " generated." << "\n";
    maxpoints = points.size;
  }
  if (i >= points.size) {
    stOutput << "only " << points.size << " points in dimension "
    << d << " available so far (requested point " << i << ")" << "\n";
  }
  return points[i];
}
/*Vector<int> pointi(int d, int i) {
  Vector<int> bars;
  bars.SetSize(d);
  int lastone = n + 1;
  for (int j = 0; j<d- 1; i ++) {
    bars[j] = i%lastone;
    i = i/lastone;
    lastone = bars[j] + 1;
  }
  Vector<int> out;
  out.SetSize(d);
  out[0] = bars[0];
  int sum = out[0];
  for (int j =1; j<d; j ++) {
    out[j] = bars[j] - bars[j]- 1
    sum += out[j];
  }
  out[d- 1] = n-sum;
}*/

/*
int pointis(int d, int n) {
  int acc = 0;
  for (int r =1; r<d; r ++) {
    MathRoutines::NChooseK(n- 1,r) * MathRoutines
  }

}*/

/*Vector<int> pointi_v2(int d, int i) {
  static Vector<int> out;
  out.MakeZero(d);

  // sometimes you have to think aloud lol
  if (i <d) {
    out[i] = 1;
    return out;
  }
  i -= d;
  if (i <d) {
    out[i] = - 1;
    return out;
  }
  i -= d;
  if (i <d*(d- 1)) {
    int j = i/d;
    int k=1%d;
    if (j<k) {
     out[j] += 1;
     out[k] += 1;
     return out;
    }
    out[j] -= 1;
    out[k] -= 1;
    return out;
  }
  i -= d*(d- 1);
  if (i <d*(d- 1)) {
    int j = i/d;
    int k = i%d;
    out[k] += 1;
    if (j<k)
      out[j] -= 1;
    else
      out[j+ 1] -= 1;
  }
  i -= d*(d- 1);
  if (i <d*d*d) {
    int j = i%d;
    i = i/d;
    int k = i%d;
    i = i/d;
    out[k] += 1;
    out[j] += 1;
    out[i] += 1;
    return out;
  }
  i -= d*d*d;
  if (i <d*d*d) {
    int j = i%d;
    i = i/d;
    int k = i%d;
    i = i/d;
    out[k] -= 1;
    out[j] -= 1;
    out[i] -= 1;
    return out;
  }
  i -= d*d*d;

  // static because wasting a few bytes vs allocating datastructures every time
  // idk if its a good idea or not i just like peppering code with keywords
  static List<int> pvec;
  static List<int> npvec;
  for (int n = 0; ; n ++) {
    //def vectors(n):
    //  cnt = 0
    //  for r in range(d):
    //    for partition in partitions(n,r):                   # (n +r- 1 choose r)
    //      pw = ways_of_placing(partition,d)                 # (d choose r)
    //      cnt += sum([2**nonzero_entries(wp) for wp in pw]) # 2**r


    //def vectors(n):
    //  cnt = 0
    //  for m in range(0,n + 1):
    //    for partition in partitions(m):
    //      for negpart in partitions(n-m):
    //        pw = ways_of_placing(partition,d) # (d choose partition.rows)
    //        for wp in pw:
    //          cnt += len(ways_of_placing(negpart, d-nonzero_entries(wp))) # (d choose negpart.rows)
    for (int m= 0; m<n + 1; m ++) {

      // vectors(n = abs sum,m = number of positives)


      pvec.SetSize(m);
      int lastp = d;
      int spacesleft = d;
      for (int j = 0; j<m; j ++) {
        int pplace;
        if (lastp > 0) {
          pplace = i%lastp;
          i /= lastp;
        } else {
          pplace = 0;
        }
        if (out[pplace] == 0){
          pvec.AddOnTop(pplace);
        }
        out[pplace] += 1;
        lastp = pplace;
      }
      // should think about this more
      npvec.SetSize(0);
      for (int ii = 0; ii <d; ii ++) {
        if (pvec.Contains(ii))
          continue;
        npvec.AddOnTop(pvec);
      }

      int lastn = npvec.size;
      int spacesleft = npvec.size;
      for (int j = 0; j < n-m; j ++) {
        int nplace;
        if (lastn > 0) {
          nplace = i%lastn;
          i /= lastn;
        } else {
          nplace = 0;
        }
        out[nplace] -= 1;
        lastn = nplace;
      }
    }
  }

  if (i <d(d- 1)(d-2)...) {
    k[n] = i%d;
    i = i/d;
    k[n- 1] = i%(d- 1);
    i
  }


}
*/


//WeylGroupRepresentation<Rational> get_macdonald_representation_v2(WeylGroup& W, const List<Vector<Rational> >& roots)
//{ stOutput << "starting with roots " << roots << " at time " << theGlobalVariables.GetElapsedSeconds() << "\n";
//  List<Vector<Rational> > monomial;
//  HashedList<List<Vector<Rational> > > monomials;
//  Matrix<Rational> m;
//  for (int i = 0; (Rational) i <W.GetGroupSizeByFormula(); i ++)
//  { W.GetStandardRepresentationMatrix(i,m);
//    monomial.SetSize(roots.size);
//    for (int j = 0; j<roots.size; j ++)
//    { monomial[j] = m*roots[j];
//      if (monomial[j].IsNegative())
//        monomial[j] = -monomial[j];
//    }
//    monomial.QuickSortAscending();
//    monomials.AddNoRepetitionOrReturnIndexFirst(monomial);
//  }
//  stOutput << "have all " << monomials.size << " monomials (" << theGlobalVariables.GetElapsedSeconds() << ")";
//
//  // Σ(n choose k)x<<k
//  int number_of_images = 1;
//  for (int i = 0; i <W.GetDim(); i ++)
//    number_of_images *= 3;
//  number_of_images /= 3;
//
//
//  List<lennum> lens;
//  List<Vector<Rational> > images;
//  images.SetSize(monomials.size);
//  lens.SetSize(monomials.size);
//  for (int i = 0; i <monomials.size; i ++)
//    lens[i].len = 0;
//
//  for (int i = 0; i <monomials.size; i ++)
//  { images[i].SetSize(number_of_images);
//    for (int j = 0; j<number_of_images; j ++)
//    { Vector<int> point = pointi(W.GetDim(),j);
//      Rational p =1;
//      for (int k1= 0; k1<monomials[i].size; k1++)
//      { Rational s = 0;
//        for (int k2= 0; k2<monomials[i][k1].size; k2++)
//          s += monomials[i][k1][k2] * point[k2];
//        p *= s;
//      }
//      images[i][j] = p;
//      lens[i].len += p*p;
//    }
//    lens[i].num = i;
//  }
//  lens.QuickSortAscending();
//  stOutput << " ... sorted (" << theGlobalVariables.GetElapsedSeconds() << ")" << "\n";
//
//  for (int i = 0; i <monomials.size; i ++)
//  { stOutput << lens[i].num << ": ";
//    Rational is = 0;
//    for (int j = 0; j<monomials[lens[i].num].size; j ++)
//    { Rational js = 0;
//      for (int k = 0; k<monomials[lens[i].num][j].size; k++)
//        js += monomials[lens[i].num][j][k];
//      stOutput << monomials[lens[i].num][j] << " [" << js << "], ";
//      is += js;
//    }
//    stOutput << "[[" << is << "]]" << "\n";
//  }
//
//  List<Vector<f65521> > images65521;
//  images65521.SetSize(monomials.size);
//  getting_images_time:
//  stOutput << "using " << number_of_images << " points";
//  if (maxpoints > 0 && number_of_images >= maxpoints)
//  { number_of_images = maxpoints;
//    stOutput << "... max points reached, actually using " << number_of_images;
//  }
//  stOutput << "\n";
//
//  for (int i = 0; i <monomials.size; i ++)
//  { int images_old_size = images65521[i].size;
//    images65521[i].SetSize(number_of_images);
//    for (int j = images_old_size; j<number_of_images; j ++)
//    { Vector<int> point = pointi(W.GetDim(),j);
//      f65521 p =1;
//      for (int k1= 0; k1<monomials[i].size; k1++)
//      { f65521 s = 0;
//        for (int k2= 0; k2<monomials[i][k1].size; k2++)
//          s += monomials[i][k1][k2] * point[k2];
//        p *= s;
//      }
//      images65521[i][j] = p;
//    }
//  }
//
//  /*
//  List<int> monomials_used_rational;
//  VectorSpace<Rational> vsr;
//  for (int i = 0; i <monomials.size; i ++)
//  { if (vsr.AddVectorToBasis(images[lens[i].num]))
//      monomials_used_rational.AddOnTop(lens[i].num);
//  }
//  stOutput << "rational module rank is " << monomials_used_rational.size << "\n";
//  //stOutput << vsr.fastbasis.ToString(&consoleFormat) << "\n";
//  stOutput << "monomials used: ";
//  for (int i = 0; i <monomials_used_rational.size; i ++)
//    stOutput << monomials_used_rational[i] << " ";
//  stOutput << "\n";
//  */
//
//  List<int> monomials_used;
//  VectorSpace<f65521> vs;
//  for (int i = 0; i <monomials.size; i ++)
//  { if (vs.AddVectorToBasis(images65521[lens[i].num]))
//      monomials_used.AddOnTop(lens[i].num);
//  }
//
//  stOutput << "finite module rank is " << monomials_used.size << " (" << theGlobalVariables.GetElapsedSeconds() << ")" << "\n";
//  //stOutput << vs.fastbasis.ToString(&consoleFormat) << "\n";
//  stOutput << "monomials used: ";
//  for (int i = 0; i <monomials_used.size; i ++)
//    stOutput << monomials_used[i] << " ";
//  stOutput << "\n";
//
//  /*
//  if (monomials_used_rational.size != monomials_used.size)
//  { stOutput << "linear combinations over Z and Zp are different" << "\n";
//    for (int i = 0; i <monomials_used_rational.size; i ++)
//      stOutput << monomials_used_rational[i] << " " << images[monomials_used_rational[i]] << "\n";
//    for (int i = 0; i <monomials_used.size; i ++)
//      stOutput << monomials_used[i] << " " << images65521[monomials_used[i]] << "\n";
//    assert(false);
//  }*/
//
//  Basis<f65521> B65521;
//  for (int i = 0; i <monomials_used.size; i ++)
//    B65521.AddVector(images65521[monomials_used[i]]);
//
//  WeylGroupRepresentation<f65521> rep65521;
//  rep65521.init(W);
//  for (int i =1; i <W.GetDim() + 1; i ++)
//  { Matrix<Rational> m;
//    W.GetStandardRepresentationMatrix(i,m);
//    Matrix<f65521> rm;
//    rm.init(monomials_used.size, monomials_used.size);
//    for (int j = 0; j<monomials_used.size; j ++)
//    { List<Vector<Rational> > monomial;
//      monomial.SetSize(roots.size);
//      for (int k = 0; k<roots.size; k++)
//        monomial[k] = m*monomials[monomials_used[j]][k];
//      Vector<f65521> evaluated;
//      evaluated.SetSize(number_of_images);
//      for (int ei = 0; ei <number_of_images; ei ++)
//      { f65521 p = 1;
//        for (int ej = 0; ej<monomial.size; ej ++)
//        { f65521 s = 0;
//          for (int ek = 0; ek<monomial[ej].size; ek++)
//          { s += monomial[ej][ek] * pointi(W.GetDim(),ei)[ek];
//          }
//          p *= s;
//        }
//        evaluated[ei] = p;
//      }
//      Vector<f65521> be;
//      /*Vector<f65521> evbackup = evaluated;
//      if (!vs.GetCoordinatesDestructively(evaluated, be))
//      { stOutput << "error: vector " << evbackup << " not in vector space " << vs.fastbasis.ToString(&consoleFormat) << " made with vectors " << "\n";
//        for (int i = 0; i <monomials_used.size; i ++)
//          stOutput << images65521[monomials_used[i]] << "\n";
//        assert(false);
//      }*/
//      be = B65521.PutInBasis(evaluated);
//      for (int mnop = 0; mnop<be.size; mnop++)
//        rm.elements[j][mnop] = be[mnop];
//    }
//    rep65521.SetElementImage(i,rm);
//    stOutput << rm.ToString(&consoleFormat) << "\n";
//    if ((rm*rm).IsIdMatrix())
//      stOutput << "passed reflection test " <<  " (" << theGlobalVariables.GetElapsedSeconds() << ")" << "\n";
//    else
//    { stOutput << "failed reflection test; retrying with more points " <<  " (" << theGlobalVariables.GetElapsedSeconds() << ")"  << "\n";
//      //stOutput << "failed finite reflection test; trying rational anyway" << "\n";
//      //goto make_rational_matrices;
//      number_of_images *= 2;
//      goto getting_images_time;
//    }
//  }
//
//
//  Matrix<f65521> idr65521;
//  idr65521.MakeIdMatrix(rep65521.GetDim());
//
//  for (int i = 0; i <W.GetDim(); i ++)
//  { for (int j = 0; j<W.GetDim(); j ++)
//    { if (j == i)
//        continue;
//      int w = W.theDynkinType.GetCoxeterEdgeWeight(i, j);
//      if (w <= 2)
//        continue;
//      Matrix<f65521> M1 = rep65521.GetMatrixElement(i + 1);
//      Matrix<f65521> M2 = rep65521.GetMatrixElement(j+ 1);
//      Matrix<f65521> M3 = M1*M2;
//      MathRoutines::RaiseToPower(M3,w,idr65521);
//      if (M3.IsIdMatrix())
//        stOutput << "passed Coxeter presentation test " << i << "," << j << "\n";
//      else
//      { stOutput << "failed Coxeter presentation test " << i << "," << j << ", retrying with more points" << "\n";
//        number_of_images *= 2;
//        goto getting_images_time;
//      }
//    }
//  }
//
//  for (int i = 0; i <monomials.size; i ++)
//  { int images_old_size = images[i].size;
//    images[i].SetSize(number_of_images);
//    for (int j = images_old_size; j<number_of_images; j ++)
//    { Vector<int> point = pointi(W.GetDim(),j);
//      Rational p =1;
//      for (int k1= 0; k1<monomials[i].size; k1++)
//      { Rational s = 0;
//        for (int k2= 0; k2<monomials[i][k1].size; k2++)
//          s += monomials[i][k1][k2] * point[k2];
//        p *= s;
//      }
//      images[i][j] = p;
//    }
//  }
//
//  Basis<Rational> B;
//  for (int i = 0; i <monomials_used.size; i ++)
//    B.AddVector(images[monomials_used[i]]);
//
//  WeylGroupRepresentation<Rational> rep;
//  rep.init(W);
//  for (int i =1; i <W.GetDim() + 1; i ++)
//  { Matrix<Rational> m;
//    W.GetStandardRepresentationMatrix(i,m);
//    Matrix<Rational> rm;
//    rm.init(monomials_used.size, monomials_used.size);
//    for (int j = 0; j<monomials_used.size; j ++)
//    { List<Vector<Rational> > monomial;
//      monomial.SetSize(roots.size);
//      for (int k = 0; k<roots.size; k++)
//        monomial[k] = m*monomials[monomials_used[j]][k];
//      Vector<Rational> evaluated;
//      evaluated.SetSize(number_of_images);
//      for (int ei = 0; ei <number_of_images; ei ++)
//      { Rational p = 1;
//        for (int ej = 0; ej<monomial.size; ej ++)
//        { Rational s = 0;
//          for (int ek = 0; ek<monomial[ej].size; ek++)
//          { s += monomial[ej][ek] * pointi(W.GetDim(),ei)[ek];
//          }
//          p *= s;
//        }
//        evaluated[ei] = p;
//      }
//      Vector<Rational> be = B.PutInBasis(evaluated);
//      for (int mnop = 0; mnop<be.size; mnop++)
//        rm.elements[j][mnop] = be[mnop];
//    }
//    rep.SetElementImage(i,rm);
//    stOutput << rm.ToString(&consoleFormat) << "\n";
//    if ((rm*rm).IsIdMatrix())
//      stOutput << "passed reflection test " <<  " (" << theGlobalVariables.GetElapsedSeconds() << ")" << "\n";
//    else
//    { stOutput << "failed reflection test; retrying with more points " <<  " (" << theGlobalVariables.GetElapsedSeconds() << ")"  << "\n";
//      number_of_images *= 2;
//      goto getting_images_time;
//    }
//  }
//
//
//  Matrix<Rational> idr;
//  idr.MakeIdMatrix(rep.GetDim());
//
//  for (int i = 0; i <W.GetDim(); i ++)
//  { for (int j = 0; j<W.GetDim(); j ++)
//    { if (j == i)
//        continue;
//      int w = W.theDynkinType.GetCoxeterEdgeWeight(i, j);
//      if (w <= 2)
//        continue;
//      Matrix<Rational> M1 = rep.GetMatrixElement(i + 1);
//      Matrix<Rational> M2 = rep.GetMatrixElement(j+ 1);
//      Matrix<Rational> M3 = M1*M2;
//      MathRoutines::RaiseToPower(M3,w,idr);
//      if (M3.IsIdMatrix())
//        stOutput << "passed Coxeter presentation test " << i << "," << j << "\n";
//      else
//      { stOutput << "failed Coxeter presentation test " << i << "," << j << ", retrying with more points" << "\n";
//        number_of_images *= 2;
//        goto getting_images_time;
//      }
//    }
//  }
//  return rep;
//}
//
//void get_macdonald_representations_of_weyl_group_v2(SemisimpleLieAlgebra& theSSlieAlg)
//{ WeylGroup& W = theSSlieAlg.theWeyl;
//  theGlobalVariables.SetStandardStringOutput(HtmlRoutines::MakeStdCoutReport);
//
//  rootSubalgebras theRootSAs;
//  theRootSAs.owner = &theSSlieAlg;
//  DynkinSimpleType dt = W.theDynkinType.GetGreatestSimpleType();
//  theRootSAs.theGlobalVariables = &theGlobalVariables;
//  theRootSAs.ComputeAllReductiveRootSubalgebrasUpToIsomorphism();
//  List<Vector<Rational> > roots;
//
//  for (int k = 0; k<theRootSAs.theSubalgebras.size; k++)
//  { rootSubalgebra& currentRootSA = theRootSAs.theSubalgebras[k];
//    roots = currentRootSA.PositiveRootsK;
//    stOutput << "I am processing root subalgebra of type "
//              << currentRootSA.theDynkinDiagram.ToStringRelativeToAmbientType(W.theDynkinType[0]);
//    WeylGroupRepresentation<Rational> rep = get_macdonald_representation_v2(W,roots);
//    rep.names.SetSize(1);
//    rep.names[0] = currentRootSA.theDynkinDiagram.ToStringRelativeToAmbientType(W.theDynkinType[0]);
//    stOutput << " has character ";
//    stOutput << rep.GetCharacter() << "\n";
//    W.AddIrreducibleRepresentation(rep);
//  }
//}

void lie_bracket_relations(Vector<Polynomial<Rational> >& out, int N) {
  Matrix<Vector<Polynomial<Rational> > > brackets;
  brackets.init(N,N);
  for (int i = 0; i < N; i ++) {
    for (int j = 0; j < N; j ++) {
      brackets.elements[i][j].SetSize(N);
      for (int k = 0; k < N; k ++) {
        brackets.elements[i][j][k].MakeMonomiaL(i * N * N + j * N + k, 1);
      }
    }
  }
  stOutput << brackets << '\n';
  for (int i = 0; i <N; i ++) {
    for (int j = 0; j < i; j ++) {
      for (int k = 0; k < j; k ++) {
        stOutput << i << j << k;
        Vector<Polynomial<Rational> > w;
        w.MakeZero(N);
        for (int l = 0; l < N; l ++) {
          for (int m = 0; m < N; m ++) {
            Polynomial<Rational> p;
            p = brackets.elements[i][j][l];
            p *= brackets.elements[k][m][l];
            w[l] += p;
            p = brackets.elements[j][k][l];
            p *= brackets.elements[i][m][l];
            w[l] += p;
            p = brackets.elements[k][i][l];
            p *= brackets.elements[j][m][l];
            w[l] += p;
          }
        }
        stOutput << w << '\n';
        out.AddListOnTop(w);
      }
    }
  }
  stOutput << "done with Jacobi relations\n";
  for (int i = 0; i < N; i ++) {
    for (int j = 0; j < i; j ++) {
      Vector<Polynomial<Rational> > v;
      v = brackets.elements[i][j];
      v += brackets.elements[j][i];
      stOutput << v << '\n';
      out.AddListOnTop(v);
    }
  }
}


/*
// sparse vector terms
template <typename basisvector, typename scalar>
class SVT
{ basisvector direction;
  scalar coefficient;

  unsigned int HashFunction() const {
   return HashFunction(this->direction)};
  bool operator<(const SVT )
};
unsigned int HashFunction(SVT& in) const {
 return in.HashFunction();}

template <typename basisvector, typename scalar>
class SparseVector
{ List<SVT>
};

template <typename scalar>
class SparseTensor
{ List<List<int> > basisforms;
  List<scalar> coefficients;

  void Canonicalize();

};

void SparseTensor::Canonicalize() {

}
*/

void TestPermutationGenerators() {
  GeneratorPermutationsOfList<int> perms;
  for (int i = 1; i < 8; i ++) {
    for (perms.Initialize(i); !perms.DoneIterating(); ++ perms) {
      stOutput << (*perms).ToStringCommaDelimited() << '\n';
    }
  }

  GeneratorElementsSnxSnOnIndicesAndIndices permssxsxs;
  List<List<int> > indiceses;
  indiceses.SetSize(3);
  indiceses[0].AddOnTop(1); indiceses[0].AddOnTop(2); indiceses[0].AddOnTop(3); indiceses[0].AddOnTop(4);
  indiceses[1].AddOnTop(5); indiceses[1].AddOnTop(6); indiceses[1].AddOnTop(7);
  indiceses[2].AddOnTop(8); indiceses[2].AddOnTop(9);
  for (permssxsxs.Initialize(indiceses); !permssxsxs.DoneIterating(); ++ permssxsxs) {
    stOutput << *permssxsxs << '\n';
  }
}

void TestCountPermutations(int N) {
  long long cnt = 0;
  GeneratorPermutationsOfList<int> perms;
  for (perms.Initialize(N); !perms.DoneIterating(); ++ perms) {
    cnt ++;
  }
  stOutput << "Generated " << cnt << " permutations of " << N << " objects\n";
}

void TestPermutationWords() {
  PermutationGroupData PG;
  PG.MakeSymmetricGroupGeneratorsjjPlus1(5);
  FiniteGroup<PermutationR2>* G = PG.theGroup;
  G->ComputeAllElements(true, - 1);
  G->ComputeCCSizesRepresentativesWords();
  for (int i = 0; i <G->theElements.size; i ++) {
    List<int> word;
    G->GetWord(G->theElements[i],word);
    PermutationR2 p;
    for (int wi = 0; wi <word.size; wi ++)
      p = p*G->generators[word[wi]];
    stOutput << ((p == G->theElements[i]) ? "☑ ":"☒ ");
    stOutput << G->theElements[i] << " has word ("
    << word.ToStringCommaDelimited() << ") which multiplies out to " << p << "\n";
  }
}

void TestPartitionsTableaux() {
  List<Partition> partitions;
  Partition::GetPartitions(partitions, 3);
  for (int i = 0; i <partitions.size; i ++) {
    stOutput << partitions[i] << '\n';
  }
  Partition::GetPartitions(partitions, 4);
  for (int i = 0; i <partitions.size; i ++) {
    stOutput << partitions[i] << '\n';
  }
  Partition::GetPartitions(partitions, 8);
  for (int i = 0; i <partitions.size; i ++) {
    stOutput << partitions[i] << '\n';
  }

  Partition D;
  List<int> Dl;
  Dl.AddOnTop(5);
  Dl.AddOnTop(3);
  Dl.AddOnTop(2);
  D.FromListInt(Dl);
  Tableau T;
  D.FillTableauOrdered(T);
  stOutput << D << ".FillTableauOrdered -> " << T << '\n';
  stOutput << "T.GetColumns() -> [";
  List<List<int> > columns;
  T.GetColumns(columns);
  for (int i = 0; i < columns.size; i ++) {
    stOutput << "[" << columns[i].ToStringCommaDelimited() << "]";
    if (i != columns.size - 1) {
      stOutput << ", ";
    }
  }
  stOutput << "]\n";
  stOutput << "Is T standard? " << T.IsStandard() << '\n';
  List<Matrix<Rational> > Ms;
  stOutput << "D.SpechtModuleMatricesOfTranspositions1j\n";
  D.SpechtModuleMatricesOfTranspositions1j(Ms);
  for (int i = 0; i < Ms.size; i ++) {
    stOutput << Ms[i].ToStringPlainText() << "\n\n";
  }
}

/*void TestHyperoctahedralStuff() {
  ElementHyperoctahedralGroup q1,q2;
  q1.p.AddTransposition(0,2);
  q1.p.AddTransposition(1,3);
  q1.s.SetSize(4);
  for (int i = 0; i <4; i ++)
    q1.s[i] = false;
  q2.s.SetSize(4);
  q2.s[0] = false;
  q2.s[1] = true;
  q2.s[2] = true;
  q2.s[3] = false;
  ElementHyperoctahedralGroup q3,q4;
  stOutput << "Q1=" << q1 << ", Q2=" << q2 << "\n";
  stOutput << "Q1*Q2=" << q1*q2 << " Q2*Q1*Q2=" << q2*q1*q2;
  q4 = q2;
  q4.Invert();
  stOutput << " Q1^Q2=" << (q1^q2) << " Q2*Q1*Q4=" << q2*q1*q4;

  for (int bni =1; bni <6;bni ++) {
    HyperoctahedralGroup Bn;
    Bn.MakeHyperoctahedralGroup(bni);
    Bn.ComputeAllElements();
    Bn.ComputeCCSizesAndRepresentatives(NULL);
    stOutput << Bn << '\n';
    stOutput << Bn.conjugacyClasseS.size << " conjugacy classes\n";
    for (int i = 0; i <Bn.conjugacyClasseS.size; i ++)
      stOutput << Bn.conjugacyClasseS[i] << ", ";
    stOutput << '\n';
    Bn.VerifyCCSizesAndRepresentativesFormula();
    stOutput << "\n\n";
  }
}*/

void TestHyperoctahedralStuff() {
  ElementHyperoctahedralGroupR2 q1, q2;
  q1.h.AddTransposition(0, 2);
  q1.h.AddTransposition(1, 3);
  q2.k.ToggleBit(1);
  q2.k.ToggleBit(2);
  ElementHyperoctahedralGroupR2 q3, q4;
  stOutput << "Q1=" << q1 << ", Q2=" << q2 << "\n";
  stOutput << "Q1*Q2=" << q1 * q2 << " Q2*Q1*Q2=" << q2 * q1 * q2;
  q4 = q2;
  q4.Invert();
  stOutput << " Q1^Q2=" << (q1 ^ q2) << " Q2*Q1*Q4=" << q2 * q1 * q4;

  for (int bni = 1; bni < 6; bni ++) {
    HyperoctahedralGroupData Bn;
    Bn.MakeHyperoctahedralGroup(bni);
    Bn.theGroup->ComputeAllElements(true, - 1);
    Bn.theGroup->ComputeCCSizesAndRepresentatives();
    stOutput << Bn << '\n';
    stOutput << Bn.theGroup->conjugacyClasseS.size << " conjugacy classes\n";
    for (int i = 0; i < Bn.theGroup->conjugacyClasseS.size; i ++) {
      stOutput << Bn.theGroup->conjugacyClasseS[i] << ", ";
    }
    stOutput << '\n';
    Bn.theGroup->VerifyCCSizesAndRepresentativesFormula();
    stOutput << "\n\n";
  }
}

void TestSpechtModules(int N = 7) {
  List<Partition> parts;
  Partition::GetPartitions(parts, N);
  PermutationGroupData PD;
  PD.MakeSymmetricGroupGeneratorsjjPlus1(N);
  FiniteGroup<PermutationR2>& G = *PD.theGroup;
  G.irreps.SetSize(parts.size);
  stOutput << G.PrettyPrintGeneratorCommutationRelations();
  stOutput << "Testing Specht modules for S_" << N << '\n';
  //#pragma omp parallel for
  for (int pi = 0; pi < parts.size; pi ++) {
    PD.SpechtModuleOfPartition(parts[pi], G.irreps[pi]);
    for (int i = 0; i < G.irreps[pi].generatorS.size; i ++) {
      stOutput << G.irreps[pi].generatorS[i].ToStringPlainText() << ",\n\n";
    }
    stOutput << "\n";
    G.irreps[pi].VerifyRepresentation();
  }
  G.irreps.QuickSortAscending();
  for (int i = 0; i < G.irreps.size; i ++) {
    stOutput << G.irreps[i] << '\n';
  }
  stOutput << G.PrettyPrintCharacterTable() << '\n';
}

/* Keeping this around as a warning to future programmers about inheritance
void TestInduction(int n =4, int m=3) {
  PermutationGroup G;
  G.MakeSymmetricGroupGeneratorsjjPlus1(n);
  G.ComputeSpechtModules();
  List<int> l;
  for (int i = 0; i <m- 1; i ++)
    l.AddOnTop(i);
  auto H = G.ParabolicKindaSubgroupGeneratorSubset(l);
  // ParabolicKindaSubgroup gives a Subgroup which is also a FiniteGroup<element type>
  // only PermutationGroup's have access to ComputeSpechtModules();
  // I think I'll switch to functions and use parametric dispatch
  // instead of classes.  I believe this is how Julia works
  PermutationGroup H1;
  H1.MakeSymmetricGroupGeneratorsjjPlus1(m);
  H1.ComputeSpechtModules();
  H.irreps.SetSize(H1.irreps.size);
  // InduceRepresentation requires the right type for the representation
  List<GroupRepresentation<Subgroup<FiniteGroup<PermutationR2>, PermutationR2 >, Rational> > inducethese;
  inducethese.SetSize(H1.irreps.size);
  for (int i = 0; i <H1.irreps.size; i ++) {
    H.irreps[i].ownerGroup = &H;
    H.irreps[i].generatorS = H1.irreps[i].generatorS;
    inducethese[i].ownerGroup = &H;
    inducethese[i].generatorS = H1.irreps[i].generatorS;
  }
  // can't use auto here ;_; could in Python with [induce(R) for R in G]
  List<GroupRepresentation<FiniteGroup<PermutationR2>, Rational> > indreps;
  indreps.SetSize(H.irreps.size);
  for (int i = 0; i <H.irreps.size; i ++)
    indreps[i] = H.InduceRepresentation(inducethese[i]);
  stOutput << G.PrettyPrintCharacterTable();
  stOutput << H.PrettyPrintCharacterTable();
  for (int i = 0; i <H.irreps.size; i ++)
    stOutput << indreps[i].DescribeAsDirectSum() << '\n';
}
*/

void TestInduction(int n = 4, int m = 3) {
  PermutationGroupData PD;
  PD.MakeSymmetricGroupGeneratorsjjPlus1(n);
  stOutput << "1\n";
  PD.ComputeSpechtModules();
  stOutput << "2\n";
  List<int> l;
  for (int i = 0; i < m - 1; i ++) {
    l.AddOnTop(i);
  }
  SubgroupData<FiniteGroup<PermutationR2>, PermutationR2> HD = PD.theGroup->ParabolicKindaSubgroupGeneratorSubset(l);
  stOutput << "3\n";
  PermutationGroupData HPD;
  HPD.theGroup = HD.theSubgroup;
  HPD.ComputeSpechtModules();
  stOutput << "4\n";
  List<GroupRepresentation<FiniteGroup<PermutationR2>, Rational> > indreps;
  indreps.SetSize(HD.theSubgroup->irreps.size);
  for (int i = 0; i <HD.theSubgroup->irreps.size; i ++) {
    indreps[i] = HD.InduceRepresentation(HD.theSubgroup->irreps[i]);
  }
  stOutput << "5\n";
  stOutput << HD.theGroup->PrettyPrintCharacterTable();
  stOutput << HD.theSubgroup->PrettyPrintCharacterTable();
  for (int i = 0; i < indreps.size; i ++) {
    stOutput << indreps[i].DescribeAsDirectSum() << '\n';
  }
}


  /*
      Rational zero = Rational(0,1);
      Rational one = Rational(1,1);
      Rational two = Rational(2,1);
      stOutput << zero*one << "\n" << zero+one << one * two << one +two << two*two << "\n";

  //    Vector<Rational> v;
  //    v.SetSize(3);
  //    v="(1/2,3,- 1/5)";
  //    v[1].AssignString("1/3");
    //  v[2] =5;
      Vector<Rational> v;
      v = "(1/3,1,2)";
      stOutput << v << "\n";

      Vector<Rational> w;
      w.SetSize(3);
      w[0] = 0;
      w[1] = 1;
      w[2] = 2;
      stOutput << w << "\n";

      Matrix<Rational> M;
      M.Resize(3,3,false,&zero);
      M.ActOnVectorColumn(v,zero);
      stOutput << M << "\n";
      stOutput << v << "\n";
  */
  /*
      Rational one = 1;
      DynkinType D;
      D.MakeSimpleType('B',3,&one);
      WeylGroup G;
      G.MakeFromDynkinType(D);
      stOutput << G << "\n";
      stOutput << G.CartanSymmetric << "\n";
      G.ComputeAllElements();
      stOutput << G << "\n";
  */
  /*
      Vector<Rational> v1; v1 = "(1,0)";
      Vector<Rational> v2; v2 = "(0,1)";

      stOutput << D << "\n";
      D.GetCartanSymmetric(M);
      stOutput << M << "\n";
      Vectors<Rational> V;
      G.ComputeWeylGroupAndRootsOfBorel(V);
      stOutput << G.GetReport() << "\n";
      stOutput << "printing G " << G << "\n";
      G.GetMatrixOfElement(0,M);


      GraphOLD *g = new GraphOLD(6,3);
      g->AddEdge(0,1);
      g->AddEdge(1,2);
      g->AddEdge(2,3);
      g->AddEdge(4,5);
      g->AddEdge(0,3);
      stOutput << "g" << g->edges << "\n";
      stOutput << g->data << "\n";
      List<List<int> > ccs;
      ccs = g->DestructivelyGetConnectedComponents();
      stOutput << ccs << "\n";

      stOutput << G.RootsOfBorel << "\n";

      stOutput << G[0] << '\n' << G[1] << '\n' << G[2] << '\n' << "\n";
      M = GetMatrixOfElement(G,G[6]);
      stOutput << GetMatrixOfElement(G,G[6]) << "\n";
      stOutput << G[6] << ' ' << M.GetDeterminant() << ' ' << M(0,0) +M(1,1) +M(2,2) << "\n";
  */
  /*

     DynkinType D;
     D.MakeSimpleType('G',2);
     Matrix<Rational> M;
     D.GetCartanSymmetric(M);
     stOutput << M << "\n";

  //  Matrix<f65521> M2;
  //  M2.init(M.NumRows, M.NumCols);
  //  for (int i = 0; i <M.NumRows; i ++)
  //    for (int j = 0; j<M.NumCols; j ++)
  //     M2.elements[i][j].n = M.elements[i][j].GetNumerator().GetIntValueTruncated();
    CoxeterGroup G;
    G.CartanSymmetric = M;
    G.ComputeIrreducibleRepresentations();
    CoxeterRepresentation<Rational> r = G.irreps[0]*G.irreps[1];
    Matrix<Rational> cfm;
    ClassFunction<Rational> cf;
    cf = G.irreps[0].GetCharacter();
    r.ClassFunctionMatrix(cf,cfm);
    stOutput << cfm.ToString(&consoleFormat) << "\n";
  */

  /*
   CoxeterGroup G;
   G.MakeFrom(M);
   G.ComputeInitialCharacters();
   for (int i = 0; i <G.characterTable.size; i ++)
      stOutput << G.characterTable[i] << "\n";
  */
  /*
    for (int i = 0;i <G.ccCount;i ++){
      int n = G.conjugacyClasses[i][0];
      Matrix<Rational> M = G.TodorsVectorToMatrix(G.rhoOrbit[n]);
      stOutput << i << charpoly(M) << "\n\n";
    //  stOutput << M << '\n';
    //  stOutput << "\n\n";
    }

    G.characterTable.size = 0;
    for (int i = 0; i <10; i ++){
      ClassFunction<Rational> X;
      X.G = &G;
      for (int j = 0; j<10; j ++){
        X.data.AddOnTop(chartable[i][j]);
      }
      G.characterTable.AddOnTop(X);
    }

    stOutput << G.characterTable << "\n";
  */
  /*
    ElementWeylGroup<WeylGroup>Ring<Rational> XeP;
    XeP.MakeFromClassFunction(G.characterTable[0]);
    stOutput << XeP.data << "\n";

    ElementWeylGroup<WeylGroup>Ring<Rational> E;
    E.MakeEi(&G,0);
    stOutput << E << "\n";
    stOutput << XeP*E << "\n";

    ElementWeylGroup<WeylGroup>Ring<Rational> XstdP;
    XstdP.MakeFromClassFunction(G.characterTable[2]);
    stOutput << XstdP << "\n";

    stOutput << "\nhi there\n";
    for (int i = 0; i <G.N; i ++) {
      E.MakeEi(&G,i);
      stOutput << XstdP*E << "\n";
    }

    stOutput << GetMatrix(G.characterTable[0]) << "\n";
    stOutput << GetMatrix(G.characterTable[2]) << "\n";

    E.MakeEi(&G,33);
    stOutput << XstdP*E << "\n";
    stOutput << GetMatrix(G.characterTable[2])*E.data << "\n";

    ElementMonomialAlgebra<CoxeterElement, Rational> Xstdp = FromClassFunction(G.characterTable[2]);
    stOutput << Xstdp << "\n";
    stOutput << "here they are" << "\n";
    List<ElementMonomialAlgebra<CoxeterElement, Rational> > l;
    for (int i = 0; i <G.N; i ++) {
      ElementMonomialAlgebra<CoxeterElement, Rational> tmp;
      tmp.AddMonomial(G.GetCoxeterElement(i),1);
      tmp *= Xstdp;
      l.AddOnTop(tmp);
      stOutput << tmp << '\n' << "\n";
    }
    Xstdp.GaussianEliminationByRowsDeleteZeroRows(l);
    stOutput << l.size << "\n";
  */
  /*
    Matrix<Rational> ct;
    ct.init(G.ccCount,G.ccCount);
    for (int i = 0; i <G.ccCount; i ++)
      for (int j = 0; j<G.ccCount; j ++) {
        ct.elements[i][j] = chartable[i][j];
        ct.elements[i][j] /= G.N;
        ct.elements[i][j] *= G.ccSizes[j];
      }
    */
  /*
  Vectors<Rational> powers;
  Vectors<Rational> powers2;
  Vector<Rational> v;
  v = G.characterTable[2].data;
  stOutput << v << '\n' << ct*v << '\n';
  powers.AddOnTop(v);
  for (int i =1; i <50; i ++) {
    Vector<Rational> v;
    v = G.characterTable[2].data;
    for (int j = 0; j<G.ccCount; j ++)
      v[j] *= powers[i- 1][j];
    Vector<Rational> w = ct*v;
    stOutput << v << '\n' << w << '\n';
    powers.AddOnTop(v);
    powers2.AddOnTop(w);
  }
  stOutput << powers2.GetRank() << "\n";
  stOutput << powers.GetRank() << "\n";

  ClassFunction<Rational> X;
  X.G = &G;
  X.data = powers[5];
  stOutput << X << "\n";

  List<ClassFunction<Rational> > tstct;
  for (int i = 0;i <G.ccCount;i ++)
    stOutput << X.IP(G.characterTable[i]) << "\n";

  Matrix<Rational> XP;
  XP = GetMatrix(G.characterTable[2]);
  ElementWeylGroup<WeylGroup>Ring<Rational> XPP;
  Vector<Rational> vv;
  XPP.MakeFromClassFunction(G.characterTable[2]);
  ElementWeylGroup<WeylGroup>Ring<Rational> vP;
  for (int i = 0; i <G.N; i ++) {
    vv.MakeEi(G.N,i);
    vP.MakeEi(&G,i);
    stOutput << (XP*vv == (XPP*vP).data) << ' ';
  }
  */
  //  Vectors<Rational> V;
  //  XP.GetZeroEigenSpace(V);
  //  stOutput << V << "\n";
  //  stOutput << V.GetRank() << "\n";

  /*
  Matrix<Rational> A;
  A.init(3,3);
  Rational one = 1;

  A(0,0) = 1; A(0,1) = 2; A(0,2) = 3;
  A(1,0) = 4; A(1,1) = 5; A(1,2) = 6;
  A(2,0) = 7; A(2,1) = 8; A(2,2) = 9;
  stOutput << Kernel(A) << "\n";

  A(0,0) = 1; A(0,1) = 0; A(0,2) = 0;
  A(1,0) = 0; A(1,1) = 1; A(1,2) = 0;
  A(2,0) = 0; A(2,1) = 0; A(2,2) = 0;
  stOutput << Kernel(A) << "\n";
  stOutput << Eigenspace(A,one) << "\n";
  */
  /*
    Matrix<Rational> A;
    Rational one = 1;
    A.init(6,6);
    A(0,0) = 2; A(0,1) = - 1; A(0,2) = - 1; A(0,3) = 0; A(0,4) = 0; A(0,5) = 0;
    A(1,0) = - 1; A(1,1) = 2; A(1,2) = - 1; A(1,3) = 0; A(1,4) = 0; A(1,5) = 0;
    A(2,0) = - 1; A(2,1) = - 1; A(2,2) = 2; A(2,3) = 0; A(2,4) = 0; A(2,5) = 0;
    A(3,0) = 0; A(3,1) = 0; A(3,2) = 0; A(3,3) = 2; A(3,4) = - 1; A(3,5) = - 1;
    A(4,0) = 0; A(4,1) = 0; A(4,2) = 0; A(4,3) = - 1; A(4,4) = 2; A(4,5) = - 1;
    A(5,0) = 0; A(5,1) = 0; A(5,2) = 0; A(5,3) = - 1; A(5,4) = - 1; A(5,5) = 2;
    A /= 3;
    stOutput << DestructiveEigenspace(A,one) << "\n";

    Rational zero = 0;
    Rational negone = - 1;
    List<List<Rational> > ll;
    GlobalVariables g;
    A.FindZeroEigenSpacE(ll,one,negone,zero,g);
    stOutput << ll << "\n";
  */
  /*
    Matrix<Rational> XP;
    for (int i = 0; i <G.ccCount; i ++) {

    XP = GetMatrix(G.characterTable[i]);
    XP *= G.characterTable[i].data[0];
    stOutput << G.characterTable[i] << "\n";
    stOutput << DestructiveEigenspace(XP,one) << "\n";
    }
  */
  /*
    Rational three = 3;
    List<Rational> p;
    p.AddOnTop(one); p.AddOnTop(three); p.AddOnTop(three); p.AddOnTop(one);
    stOutput << factorpoly(p,5) << "\n";
    p.size = 0;
    p.AddOnTop(one); p.AddOnTop(three); p.AddOnTop(one); p.AddOnTop(three);
    stOutput << factorpoly(p,5) << "\n";

    Vector<Rational> v;
    List<Vector<Rational> > B;
    v.AssignString("(1,1,0)");
    B.AddOnTop(v);
    v.AssignString("(1,0,1)");
    B.AddOnTop(v);
    v.AssignString("(0,1,1)");
    B.AddOnTop(v);
    v.AssignString("(2,2,2)");
    stOutput << PutInBasis(v,B) << "\n";
    B.size = 0;
    v.AssignString("(1,1,0,0)");
    B.AddOnTop(v);
    v.AssignString("(1,0,0,1)");
    B.AddOnTop(v);
    v.AssignString("(0,0,1,1)");
    B.AddOnTop(v);
    stOutput << "4-vector tests" << "\n";
    v.AssignString("(1,0,0,1)");
    stOutput << PutInBasis(v,B) << "\n";
    v.AssignString("(0,1,1,0)");
    stOutput << PutInBasis(v,B) << "\n";

    M = GetMatrix(G.characterTable[2]);
    M *= 3;
    B = DestructiveEigenspace(M,one);
    stOutput << B << "\n";

    Matrix<Rational> BM;
    BM.init(B.size,B[0].size);
    for (int i = 0; i <B.size; i ++) {
      for (int j = 0; j<B[0].size; j ++) {
        BM.elements[i][j] = B[i][j];
      }
    }

    stOutput << BM.ToString(&consoleFormat) << "\n";
    for (int i = 0; i <BM.NumRows; i ++) {
      stOutput << PutInBasis(B[i],B) << "\n";
    }

    int ge = 37;
    M.init(B.size,B.size);
    for (int i = 0; i <B.size; i ++) {
      Vector<Rational> v;
      v.MakeZero(B[0].size);
      for (int j = 0; j<B[0].size; j ++) {
        v[G.MultiplyElements(ge,j)] = B[i][j];
      }
      v = PutInBasis(v,B);
      for (int j = 0;j<B.size;j ++) {
        M.elements[i][j] = v[j];
      }
    }
    stOutput << M.ToString(&consoleFormat) << "\n";



    List<List<Vector<Rational> > > spaces;
    M = GetMatrix(G.characterTable[2]);
    B = DestructiveColumnSpace(M);
    spaces.AddOnTop(B);
    List<ClassFunction<Rational> > Xs;
    Xs.AddOnTop(G.characterTable[2]);
    stOutput << spaces << "\n";
    { int i =1;
      Xs.AddOnTop(Xs[i- 1]*Xs[i- 1]);
      M = GetMatrix(Xs[i]);
  //    List<Rational> p = charpoly(M);
  //    stOutput << p << "\n";
  //    List<int> ls = factorpoly(p,Xs[i].data[0].floorIfSmall());
  //    stOutput << ls << "\n";
      B = DestructiveColumnSpace(M);
      stOutput << B << "\n";
    }
  */
  /*

    Matrix<Rational> a;
    Matrix<Rational> b;
    a.init(2,2);
    b.init(2,2);
    a(0,0) = - 1; a(0,1) = - 1; a(1,0) =  1; a(1,1) =  0;
    b(0,0) =  1; b(0,1) =  0; b(1,0) = - 1; b(1,1) = - 1;
    M.AssignTensorProduct(b,b);
    stOutput << M.ToString(&consoleFormat) << "\n";
  */
  /*
      stOutput << "this orbit has " << G.rhoOrbit.size << "\n";
      stOutput << G.rhoOrbit << "\n";
      Vector<Rational> v = G.rhoOrbit[34];
      M = G.TodorsVectorToMatrix(v);
      stOutput << M*G.rho << " derp " << v << "\n";
  */
//    for (int i = 0;i <G.rhoOrbit.size;i ++){
//    stOutput << G.TodorsVectorToMatrix(G.rhoOrbit[i]) << "\n\n";
//    }
//    stOutput << "\n";
  /*
      stOutput << v << G.DecomposeTodorsVector(v) << G.ComposeTodorsVector(G.DecomposeTodorsVector(v)) << "\n";

      G.ComputeConjugacyClasses();
      stOutput << G.conjugacyClasses << "\n";
      stOutput << G.ccSizes << "\n";
      G.ComputeSquares();
      stOutput << G.squares << "\n";
      G.ComputeInitialCharacters();
      stOutput << G.characterTable << "\n";
      stOutput << G.characterTable[0].norm() << G.characterTable[1].norm() << G.characterTable[2].norm() << "\n";

  //    ElementMonomialAlgebra<FiniteGroupElement, Rational> a;
      ClassFunction X = G.characterTable[2] - G.characterTable[0];
      stOutput << X << X.norm() << "\n";

      List<Vector<Rational> > l;
      for (int i = 0;i <G.rootSystem.size;i ++){
          bool usethis = true;
          for (int j = 0; j<4; j ++)
              if (G.rootSystem[i][j] < 0){
                  usethis = false;
                  break;
              }
          if (usethis == true)
              l.AddOnTop(G.rootSystem[i]);
      }
      stOutput << l << "\n";
  */
  /*
      std::string* S = [
      ""

      ]
  */
  /*    stOutput << "polynom1al" << "\n";
      polynom1al<Rational> zeropoly;
      Matrix<polynom1al<Rational> > MP;
      MP.MakeZeroMatrix(G.N, zeropoly);
      for (int i1= 0; i1<G.ccCount; i1++) {
        for (int i2= 0; i2<G.ccSizes[i1]; i2++) {
          int i = G.conjugacyClasses[i1][i2];
          for (int j = 0; j<G.N; j ++) {
            M.elements[G.MultiplyElements(i, j)][j] = G.characterTable[2].data[i1]/G.N;
          }
        }
      }
      for (int i = 0; i <G.N; i ++) {
        MP.elements[i][i].AddMonomial(- 1,1);
      }

      polynom1al<Rational> cp;

      cp = MP.GetDeterminant();

      stOutput << cp << "\n";
  */
//    List<bool> isod;
//    isod.AddOnTop(false);
//    int isodd = 0;
  /*    List<Vector<Rational> > bigspace;
      List<List<Vector<Rational> > > spaces;
      for (int i = 0; i <G.N; i ++) {
       Vector<Rational> v;
        v.MakeEi(G.N,i);
        bigspace.AddOnTop(v);
      }
      spaces.AddOnTop(bigspace);
      Rational one = 1;
      Rational zero = 0;
      for (int ci = 0; ci <G.ccCount; ci ++) {
        ClassFunction<Rational> X;
        X.G = &G;
        for (int ii = 0; ii <G.ccCount; ii ++) {
          if (ii == ci)
            X.data.AddOnTop(one);
            continue;
          X.data.AddOnTop(zero);
        }

      }
      */
  /*    ClassFunction<Rational> Xs = G.characterTable[2];
      ClassFunction<Rational> Xcs = G.characterTable[1]*G.characterTable[2];
      List<List<ClassFunction<Rational> > > chars;
      List<ClassFunction<Rational> > charsi;
      charsi.AddOnTop(G.characterTable[0]);
      chars.AddOnTop(charsi);
      HashedList<ClassFunction<Rational> > usedchars;
      int ci = 0;
      int cj = 0;
      while (true) {
        if (!usedchars.Contains(chars[ci][cj])) {
          usedchars.AddOnTop(chars[ci][cj]);
          stOutput << "matrix for " << ci << " " << cj << " " << chars[ci][cj] << "\n";
          Matrix<Rational> MM = GetMatrix(chars[ci][cj]);
          stOutput << "eigenspaces" << "\n";
          List<List<Vector<Rational> > > es = eigenspaces(MM,G.N);
          stOutput << "splitting spaces" << "\n";
          for (int i = 0; i <es.size; i ++) {
            int spaces_to_check = spaces.size;
            for (int j = 0; j<spaces_to_check; j ++) {
              //if (isod[j])
              //  continue;
              if (spaces[j].size < 9)
                continue;
              stOutput << "intersect eigenspace " << i << " with space " << j << "\n";
              List<Vector<Rational> > middle = intersection(es[i], spaces[j]);
              stOutput << "result is " << middle.size << "dimensional" << "\n";
              if (0 < middle.size and middle.size < spaces[j].size) {
                stOutput << "finding relative complement" << "\n";
                List<Vector<Rational> > right = relative_complement(spaces[j],es[i]);
                stOutput << "relative complement is " << right.size << "dimensional" << "\n";
                spaces[j] = middle;
                //if (is_isotypic_component(G,middle))
                //{ isod[j] = true;
                //  isodd += middle.size;
                //  stOutput << "intersection was an isotypic component" << "\n";
                //}
                spaces.AddOnTop(right);
                //if (is_isotypic_component(G,right))
                //{ isod.AddOnTop(true);
                //  isodd += right.size;
                //  stOutput << "complement was an isotypic component" << "\n";
                //} else
                //{ isod.AddOnTop(false);
                //}
                stOutput << "moving on" << "\n";
              }
            }
          }
  //        if (isodd >= G.N)
  //          break;
          if (spaces.size == G.ccCount)
            break;
        }
        stOutput << "finding next character" << "\n";
        if (ci != 0) {
          ci --;
          cj ++;
          chars[ci].AddOnTop(chars[ci][cj- 1]*Xcs);
        } else {
          ci = cj+ 1;
          cj = 0;
          List<ClassFunction<Rational> > cil;
          cil.AddOnTop(chars[ci- 1][0]*Xs);
          chars.AddOnTop(cil);
        }
      }
      stOutput << "spaces" << "\n";
      stOutput << spaces.size << "\n";
      for (int i = 0; i <spaces.size; i ++)
        stOutput << spaces[i].size << " ";
      stOutput << "\n";
      //stOutput << isod << "\n";
      //stOutput << isodd << "\n";
  */
  /*    int i = 0; j = 0;
      while (spaces.size > 0) {


      }
  */

  /*    stOutput << V2.GetCharacter() << "\n";
      CoxeterRepresentation<Rational> V2x2 = V2*V2;
      stOutput << V2x2.GetCharacter() << "\n";
      for (int i = 0; i <G.rank; i ++) {
        stOutput << V2x2.gens[i].ToString(&consoleFormat);
        stOutput << V2x2.gens[i].GetDeterminant() << ' ' << V2x2.gens[i].GetTrace() << "\n";
      }
      stOutput << "class function matrices" << "\n";
      for (int cfi = 0; cfi <G.ccCount; cfi ++) {
        ClassFunction<Rational> cf;
        cf.G = &G;
        cf.MakeZero();
        cf[cfi] = 1;
        stOutput << cfi << "\n";
        stOutput << V2x2.ClassFunctionMatrix(cf).ToString(&consoleFormat) << "\n";
      }
      List<CoxeterRepresentation<Rational> > Vs;
      Vs = V2x2.Decomposition();
      for (int i = 0; i <Vs.size; i ++)
        stOutput << Vs[i].Reduced().GetCharacter() << "\n";
    */

  /*Matrix<Rational> A;
  A.init(6,6);
  A(0,0) = 2; A(0,1) = - 1; A(0,2) = - 1; A(0,3) = 0; A(0,4) = 0; A(0,5) = 0;
  A(1,0) = - 1; A(1,1) = 2; A(1,2) = - 1; A(1,3) = 0; A(1,4) = 0; A(1,5) = 0;
  A(2,0) = - 1; A(2,1) = - 1; A(2,2) = 2; A(2,3) = 0; A(2,4) = 0; A(2,5) = 0;
  A(3,0) = 0; A(3,1) = 0; A(3,2) = 0; A(3,3) = 2; A(3,4) = - 1; A(3,5) = - 1;
  A(4,0) = 0; A(4,1) = 0; A(4,2) = 0; A(4,3) = - 1; A(4,4) = 2; A(4,5) = - 1;
  A(5,0) = 0; A(5,1) = 0; A(5,2) = 0; A(5,3) = - 1; A(5,4) = - 1; A(5,5) = 2;
  stOutput << "polynomial test" << "\n";
  UDPolynomial<Rational> p = MinPoly(A);
  stOutput << p.data.size << "\n";
  stOutput << p << "\n";
  */

  /*stOutput << '[';
  for (int i = 0; i <G.ccCount; i ++)
    stOutput << G.conjugacyClasses[i][0] << ", ";
  stOutput << ']' << "\n";
  for (int i = 0; i <G.ccCount; i ++)
    stOutput << i << ' ' << G.ccSizes[i] << ' ' << G.GetCoxeterElement(G.conjugacyClasses[i][0]) << "\n";
  for (int i = 0; i <G.ccCount; i ++)
    stOutput << G.GetCoxeterElement(G.conjugacyClasses[i][0]) << ", ";*/
//  G.ComputeIrreducibleRepresentations();
  /*
  List<ClassFunction<Rational> > ct;
  List<CoxeterRepresentation<Rational> > gr;

  CoxeterRepresentation<Rational> sr = G.StandardRepresentation();
  CoxeterRepresentation<Rational> sr2 = sr*sr;

  List<CoxeterRepresentation<Rational> > sr2d = sr2.Decomposition(ct,gr);

  stOutput << "std (x) std = ";
  for (int i = 0; i <sr2d.size; i ++) {
    stOutput << sr2d[i].GetCharacter() << "\n";
    for (int j = 0; j<sr2d[i].gens.size; j ++)
     stOutput << sr2d[i].gens[j].ToString(&consoleFormat) << "\n";
  }
  */



  /*
     stOutput << "Building QG" << "\n";
     WeylGroupRepresentation<Rational> QG;
     QG.reset(&G);
     for (int g=1; g<G.CartanSymmetric.NumRows+ 1; g++) {
       Matrix<Rational> M;
       M.MakeZeroMatrix(G.theElements.size);
       for (int i = 0; i <G.theElements.size; i ++) {
         ElementWeylGroup<WeylGroup> gg;
         gg = G.theElements[g];
         gg *= G.theElements[i];
         gg.MakeCanonical();
         M.elements[i][G.theElements.GetIndex(gg)] = 1;
       }
       QG.SetElementImage(g,M);
     }

     stOutput << "getting isotypic components of QG" << "\n";
     List<WeylGroupRepresentation<Rational> > isocomps;
     isocomps.SetSize(G.irreps.size);
     for (int i = 0; i <G.irreps.size; i ++) {
       Matrix<Rational> M;
       QG.GetClassFunctionMatrix(G.irreps[i].GetCharacter(),M);
       VectorSpace<Rational> V;
       Vectors<Rational> B;
       for (int j = 0; j<M.NumCols; j ++) {
         Vector<Rational> v;
         M.GetVectorFromColumn(j,v);
         if (!V.AddVector(v))
           continue;
         B.AddOnTop(v);
       }
       Vector<Rational> v;
       QG.Restrict(B,v,isocomps[i]);
     }

     stOutput << "trying to break up one isotypic component" << "\n";
     int cmpx = 2;
     stOutput << isocomps[cmpx].ToString(&consoleFormat) << "\n";
     int d = isocomps[cmpx].GetRank();
     stOutput << "the smoothed-out inner product" << "\n";
     Matrix<Rational> B;
     B.init(d,d);
     for (int i = 0; i <d; i ++)
       for (int j = 0; j<d; j ++)
         for (int g= 0; g<G.theElements.size; g++) {
           Vector<Rational> v,w;
           isocomps[cmpx].GetElementImage(g).GetVectorFromColumn(i,v);
           isocomps[cmpx].GetElementImage(g).GetVectorFromColumn(j,w);
           B.elements[i][j] = v.ScalarEuclidean(w);
         }
    stOutput << B.ToString(&consoleFormat) << "\n";

    Vector<Rational> v;
    v.MakeZero(B.NumRows);
    for (int i = 0; i <B.NumCols; i ++)
      for (int j = 0; j<B.NumRows; j ++)
        v[j] += B.elements[i][j];
    stOutput << v << "\n";

    VectorSpace<Rational> WS;
    Vectors<Rational> W;
    for (int g= 0; g<G.theElements.size; g++) {
      Vector<Rational> w = isocomps[cmpx].GetElementImage(g) * v;
      if (WS.AddVector(w))
        W.AddOnTop(w);
    }
    WeylGroupRepresentation<Rational> comp;
    Vector<Rational> derp;
    isocomps[cmpx].Restrict(W,derp,comp);

    stOutput << "This should be one component" << "\n";
    stOutput << comp.ToString(&consoleFormat) << "\n";
  */

  /*
  f211 three = 3;
  f211 one = 1;
  f211 third = 3;
  third.Invert();
  stOutput << "f211 test " << third << " " << one/three << " " << (one/three)*three << "\n";
  f211 n2 = -2;
  f211 two = 2;
  stOutput << n2 << " " << n2 + two << " " << n2 + three << " " << " " << one / three << " " << n2 / three << " " << (n2/three)*three << "\n";
  */

//  char letter = 'F';
//  int number = 4;


  //theGlobalVariables.SetStandardStringOutput(HtmlRoutines::MakeStdCoutReport);
  //SemisimpleLieAlgebra theSSlieAlg;
  //theSSlieAlg.theWeyl.MakeArbitrarySimple(letter, number);
  //theSSlieAlg.ComputeChevalleyConstants(&theGlobalVariables);
  //WeylGroup& W = theSSlieAlg.theWeyl;
  //W.ComputeConjugacyClassesThomasVersion();

  //get_macdonald_representations_of_weyl_group_v2(theSSlieAlg);
  //for (int i = 0; i <W.irreps.size; i ++)
  //  stOutput << W.irreps[i].GetName() << '\t' << W.irreps[i].GetCharacter() << "\n";

  //ComputeCharacterTable(W);

  //ComputeIrreps(W);
/*UDPolynomial<Rational> p;
  Matrix<Rational> m;
  for (int i = 0; i <W.conjugacyClasses.size; i ++) {
    for (int j = 0; j<W.conjugacyClasses[i].size; j ++) {
      W.GetStandardRepresentationMatrix(W.conjugacyClasses[i][j],m);
      p.AssignCharPoly(m);
      stOutput << i << ',' << j << ": " << p << "\n";
    }
  }*/

  /*
     JSData data;
     data.readfile("c7ct");
     if (data.theType != JSNULL) {
       stOutput << data << "\n";
       AddCharTable(data,G);
     } else {
       ComputeCharacterTable(G);
       PrintCharTable(G, NULL);
       PrintCharTable(G, "c7ct");
     }
     JSData ts;
     PrettyPrintTauSignatures(G,ts);
     ts.writefile("c7ts");
  */
//   G.ComputeConjugacyClasses();
//   stOutput << "Conjugacy class representatives" << "\n";
//   for (int i = 0; i <G.conjugacyClasses.size; i ++)
//    stOutput << G.rhoOrbit[G.conjugacyClasses[i][0]] << "\n";
  /*   List<Vector<Rational> > chars = ComputeCharacterTable(G);
     for (int i = 0; i <chars.size; i ++)
       stOutput << chars[i] << "\n";
       */


//  AnotherWeylGroup<f211,PackedVector<f211> > G;
//  WeylGroup G;
//  G.MakeArbitrarySimple(letter,number);
//  G.ComputeConjugacyClasses();
//  stOutput << G.ConjugacyClassCount() << "\n";
//  JSData out;
//  for (int i = 0; i <G.conjugacyClasses.size; i ++)
//    out[i] = (double) G.conjugacyClasses[i].size;
//  stOutput << out << "\n";

//  stOutput <<
//  stOutput << FindConjugacyClassRepresentatives(G,500);

//  Polynomial<Rational> p,q;
//  p.MakeMonomiaL(0,1);
//  q.MakeMonomiaL(1,1);
//  stOutput << p << " " << q << " " << p+q << " " << '\n';
//  Vector<Polynomial<Rational> > relations;
//  lie_bracket_relations(relations, 3);

void TestWeylIrrepsFormulas(char letter, int number) {
  WeylGroupData W;
  W.MakeArbitrarySimple(letter, number);
  W.ComputeIrreducibleRepresentationsWithFormulasImplementation(W.theGroup);
}

void LegacyTest() {
  TestWeylIrrepsFormulas('A', 4);
  TestWeylIrrepsFormulas('B', 4);
  TestWeylIrrepsFormulas('D', 4);
}

void TestFiniteFields() {
  f65521 a;
  a.n = 2;
  f65521 b;
  b.n = 5;
  stOutput << (a * b).n << "\n";
  stOutput << (a / b).n << "\n";
}

//   std::string s;
//   std::cin >> s;

int mainTest(List<std::string>& inputArguments) {
  stOutput << inputArguments.ToStringCommaDelimited() << '\n';
  if (inputArguments.size == 0) {
    LegacyTest();
  } else {
    if (inputArguments[0] == "count_permutations") {
      int N;
      if (inputArguments.size == 2) {
        N = atoi(inputArguments[1].c_str());
      } else {
        N = 10;
      }
      TestCountPermutations(N);
    }
    if (inputArguments[0] == "specht_modules") {
      int N;
      if (inputArguments.size == 2) {
        N = atoi(inputArguments[1].c_str());
      } else {
        N = 6;
      }
      TestSpechtModules(N);
    }
    if (inputArguments[0] == "test_induction") {
      if (inputArguments.size == 3) {
        int n = atoi(inputArguments[1].c_str());
        int m = atoi(inputArguments[2].c_str());
        TestInduction(n, m);
      } else {
        TestInduction();
      }
    }
    if (inputArguments[0] == "TestWeylIrrepsFormulas") {
      if (inputArguments.size == 3) {
        char l = (inputArguments[1].c_str())[0];
        int n = atoi(inputArguments[2].c_str());
        TestWeylIrrepsFormulas(l, n);
      } else {
        TestWeylIrrepsFormulas('B', 3);
      }
    }
  }

  stOutput << "Rational::TotalSmallAdditions: " << Rational::TotalSmallAdditions;
  stOutput << "\nRational::TotalLargeAdditions: " << Rational::TotalLargeAdditions;
  stOutput << "\nRational::TotalSmallMultiplications: " << Rational::TotalSmallMultiplications;
  stOutput << "\nRational::TotalLargeMultiplications: " << Rational::TotalLargeMultiplications << "\n";
  stOutput << "GetElapsedTimeInSeconds(): " << theGlobalVariables.GetElapsedSeconds() << "\n";
  return 0;
}
