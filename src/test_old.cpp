// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include <assert.h>


#include "system_functions_global_objects.h"

//#include "testlib.h"
#include <iostream>
#include <algorithm>

#include "json.h"
#include "math_extra_finite_fields.h"
#include "math_extra_packed_vector_implementation.h"
#include "math_extra_finite_groups_implementation.h"

#include "math_extra_symmetric_groups_and_generalizations.h"
#include "general_test.h"
#include "math_general_implementation.h"

extern ProjectInformationInstance ProjectInfoTestRoutinesOld;
ProjectInformationInstance ProjectInfoTestRoutinesOld(__FILE__, "Deprecated experimental code. To be merged or deleted. ");


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

  void MakeHomomorphismSemidirectGroupData(FiniteGroup<kelt>* subgroup, GroupHomomorphism<kelt, kelt>* outer) {
    this->theSubgroup = subgroup;
    this->oa = outer;
    this->theGroup = &theGroupMayBeHereNameIsLongToDiscourageUse;
    this->theGroup->generators = this->theSubgroup->generators;
    this->theGroup->generators.SetSize(this->theGroup->generators.size + 1);
    this->theGroup->generators.LastObject()->MakeID(this->theGroup->generators[0]);
    this->theGroup->generators.LastObject()->h = true;
  }
};

class CharacterTable {
public:
  List<List<int> > representatives;
  List<int> sizes;
  List<Vector<Rational> > characters;

  void write(std::string filename);
  void read(std::string filename);
};

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
    result += static_cast<unsigned int>(this->TheObjects[i]) * SomeRandomPrimes[i];
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
      memmove(this->things.TheObjects + newsz, this->things.TheObjects + size, static_cast<unsigned>(oldos - size));
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
    memmove(viewed->things.TheObjects + nslp + deltas, viewed->things.TheObjects + nslp, static_cast<unsigned>(viewed->things.size - nslp));
    viewed->things.SetSize(viewed->things.size + deltas);
  } else {
    viewed->things.SetSize(viewed->things.size + deltas);
    memmove(viewed->things.TheObjects + nslp + deltas, viewed->things.TheObjects + nslp, static_cast<unsigned>(viewed->things.size - nslp));
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

template <typename weylgroup>
void PrettyPrintTauSignatures(weylgroup& G, JSData& data, bool pseudo = false) {
  if (G.characterTable.size == 0) {
    ComputeCharacterTable(G);
  }
  List<List<bool> > ts;
  AllTauSignatures(&G, ts, true);
  for (int i = 0; i < ts.size; i ++) {
    for (int j = 0; j < ts[i].size; j ++) {
      data[i][j] = ts[i][j];
    }
  }

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
  for (int i = 0; Rational(i) < W.theGroup.GetSize(); i ++) {
    W.GetStandardRepresentationMatrix(i, m);
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

static int maxpoints;

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

void lie_bracket_relations(Vector<Polynomial<Rational> >& out, int N) {
  Matrix<Vector<Polynomial<Rational> > > brackets;
  brackets.init(N, N);
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

int testOld(List<std::string>& inputArguments){
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
