#include "vpfHeader2Math3_SymmetricGroupsAndGeneralizations.h"

ProjectInformationInstance ProjectInfoVpf2Math3_SymmetricGroupsAndGeneralizations(__FILE__, "Symmetric groups and generalizations ");

// conjugacy classes of type P are of size |Sn|/P.Fulton61z()
int Partition::Fulton61z() const
{ int acc = 1;
  List<int> nums;
  List<int> counts;
  for(int i=0; i<this->p.size; i++)
  { int numdex = nums.GetIndex(this->p[i]);
    if(numdex == -1)
    { nums.AddOnTop(this->p[i]);
      counts.AddOnTop(1);
    } else
    { counts[numdex] += 1;
    }
  }
  for(int i=0; i<nums.size; i++)
  { int acci = 1;
    for(int j=0; j<counts[i]; j++)
      acci *= nums[i];
    for(int j=1; j<counts[i]; j++)
      acci *= j;
    acc *= acci;
  }
  return acc;
}

int& Partition::operator[](int i) const
{ return p[i];
}

void Partition::FromListInt(const List<int> &in, int lastElement)
{ int n = 0;
  int l = -1;
  int i=0;
  if(lastElement == -1)
    lastElement = in.size;
  for(; i<lastElement; i++)
  { if(in[i] < 0)
      crash << "Partitions should not have negative numbers in them";
    if(l == -1)
      l = in[i];
    else
      if(in[i] > l)
        crash << "Partitions should be sorted in descending order";
    if(in[i] == 0)
      break;
    n += in[i];
  }
  this->p.SetSize(i);
  this->n = n;
  for(int j=0; j<i; j++)
    p[j] = in[j];
}

void Partition::GetPartitions(List<Partition>& out, int n)
{ out.SetSize(0);
  List<int> p;
  p.SetSize(n);
  for(int i=0; i<n; i++)
    p[i] = 0;
  p[1] = n;
  int k = 1;
  while(k != 0)
  { int x = p[k-1] + 1;
    int y = p[k] - 1;
    k -= 1;
    while(x <= y)
    { p[k] = x;
      y -= x;
      k += 1;
    }
    p[k] = x+y;
    //out.SetSize(out.size+1);
    //out[out.size-1].FromListInt(p,k+1);
    int s = out.size;
    out.SetSize(s+1);
    out[s].p.SetSize(k+1);
    for(int i=0; i<k+1; i++)
      out[s].p[i] = p[k-i];
    out[s].n = n;
  }
}

void Partition::FillTableau(Tableau& out, List<int>& stuffing) const
{ if(stuffing.size < this->n)
    crash << "need at least " << this->n << " things to stuff tableau with partition " << *this << " but you provided only " << stuffing.size << '\n' << crash;

  out.t.SetSize(this->p.size);
  int cur = 0;
  for(int i=0; i<this->p.size; i++)
  { out.t[i].SetSize(this->p[i]);
    for(int j=0; j<this->p[i]; j++)
    { out.t[i][j] = stuffing[cur];
      cur++;
    }
  }
//  stOutput << "Debugging Partition::FillTableau: " << *this << " fills out to " << out << "\n";
}

void Partition::FillTableauOrdered(Tableau& in) const
{ in.t.SetSize(this->p.size);
  int cur = 0;
  for(int i=0; i<this->p.size; i++)
  { in.t[i].SetSize(this->p[i]);
    for(int j=0; j<this->p[i]; j++)
    { in.t[i][j] = cur;
      cur++;
    }
  }
}

void Partition::GetAllStandardTableaux(List<Tableau>& out) const
{ //stOutput << "Debugging Partition::GetAllStandardTableaux with partition " << *this << "\n";
  PermutationGroup sn;
  sn.MakeSymmetricGroup(this->n);
  sn.ComputeAllElements();
  //stOutput << sn << '\n';
  List<int> ordered;
  ordered.SetSize(this->n);
  for(int i=0; i<this->n; i++)
    ordered[i] = i;
  for(int i=0; i<sn.theElements.size; i++)
  { List<int> permuted = ordered;
    sn.theElements[i].ActOnList(permuted);
    Tableau theTableau;
    this->FillTableau(theTableau,permuted);
    if(theTableau.IsStandard())
      out.AddOnTop(theTableau);
  }
}


void Partition::TestAllSpechtModules(int n)
{ List<Partition> partitions;
  Partition::GetPartitions(partitions, n);
  int fac = 1;
  for(int i=1; i<=n; i++)
    fac *= i;
  for(int i=0; i<partitions.size; i++)
  { stOutput << partitions[i] << '\n';
    List<Matrix<Rational> > repgens;
    partitions[i].SpechtModuleMatricesOfTranspositionsjjplusone(repgens);
    for(int ri=0; ri<repgens.size; ri++)
    { stOutput << repgens[ri].ToStringPlainText();
      Rational det = repgens[ri].GetDeterminant();
      stOutput << " determinant is " << det << "\n\n";
      if((det != 1) && (det != -1))
        crash << "invalid determinant" << crash;
    }
    SimpleFiniteGroup<Matrix<Rational> > outg;
    outg.generators = repgens;
    outg.ComputeElementsAndCCs();
    stOutput << outg << "\n\n\n";
    if((fac % outg.theElements.size) != 0)
      crash << "invalid elements count" << crash;
  }
}

bool Partition::operator==(const Partition& right) const
{ return this->p == right.p;
}

bool Partition::operator!=(const Partition& right) const
{ return this->p != right.p;
}

// largest numbers first
bool Partition::operator<(const Partition& right) const
{ if(this->n < right.n)
    return true;
  if(this->n > right.n)
    return false;
  // largest first
  if(this->p > right.p)
    return true;
  if(this->p < right.p)
    return false;
  return false;
}

// is there a better way to do this then copypasting codes?
bool Partition::operator>(const Partition& right) const
{ if(this->n < right.n)
    return false;
  if(this->n > right.n)
    return true;
  // largest first
  if(this->p > right.p)
    return false;
  if(this->p < right.p)
    return true;
  return false;
}


std::string Partition::ToString() const
{ std::stringstream out;
  this->IntoStream(out);
  return out.str();
}


// Tableau implementation

List<int> Tableau::TurnIntoList() const
{ List<int> out;
  for(int i=0; i<this->t.size; i++)
    for(int j=0; j<this->t[i].size; j++)
      out.AddOnTop(this->t[i][j]);
  return out;
}

bool Tableau::IsStandard() const
{// stOutput << "Debugging Tableau::IsStandard: ";
 // stOutput << *this << " is ";
  if(this->t.size == 0)
  {// stOutput << "standard.\n";
    return true;
  }
  for(int i=0; i<this->t.size; i++)
  { int cur=-1;
    for(int j=0; j<this->t[i].size; j++)
    { if(!(cur < this->t[i][j]))
      {// stOutput << "not standard (row " << i << ").\n";
        return false;
      }
      cur = this->t[i][j];
    }
  }
  for(int i=0; i<this->t[0].size; i++)
  { int cur = -1;
    int j=0;
    while(true)
    { if(j == this->t.size)
        break;
      if(this->t[j].size <= i)
        break;
      if(!(cur <= this->t[j][i]))
      {// stOutput << "not standard (column " << i << ").\n";
        return false;
      }
      cur = this->t[j][i];
      j++;
    }
  }
  //stOutput << "standard.\n";
  return true;
}

void Tableau::RowStabilizer(PermutationGroup& in) const
{ for(int i=0; i<this->t.size; i++)
  { if(this->t[i].size == 1)
      continue;
    for(int j=1; j<this->t[i].size; j++)
    { in.generators.SetSize(in.generators.size+1);
      in.generators[in.generators.size-1].BuildTransposition(this->t[i][0],this->t[i][j]);
    }
  }
}

void Tableau::ColumnStabilizer(PermutationGroup& in) const
{ if(this->t.size == 0)
    return;
  for(int i=0; i<this->t[0].size; i++)
  { int j=1;
    while(true)
    { if((j==this->t.size) || (this->t[j].size <= i))
        break;
      in.generators.SetSize(in.generators.size+1);
      in.generators[in.generators.size-1].BuildTransposition(this->t[0][i],this->t[j][i]);
      j++;
    }
  }
}


std::string Tableau::ToString() const
{ std::stringstream out;
  this->IntoStream(out);
  return out.str();
}



// int operator*(int) expects the permutation to be canonical
// having each number exactly once
// this method can take any list of cyles
void PermutationR2::MakeCanonical()
{ int bon = this->BiggestOccurringNumber();
  List<bool> used;
  used.initFillInObject(bon+1,false);
  for(int i=0; i<used.size; i++)
    used[i] = false;
  List<List<int> > tmp;
  for(int i=0; i<=bon; i++)
  { if(used[i])
      continue;
    int head = i;
    int cur = head;
    bool incycle = false;
    while(true)
    { // same as int operator*(int) but without the short circuit
      for(int j=this->cycles.size-1; j>=0; j--)
      { for(int k=0; k<this->cycles[j].size; k++)
        { if(this->cycles[j][k] != cur)
            continue;
          if(k != this->cycles[j].size-1)
            cur = this->cycles[j][k+1];
          else
            cur = this->cycles[j][0];
        break;
        }
      }
      if(cur == head)
        break;
      if(!incycle)
      { tmp.SetSize(tmp.size + 1);
        tmp[tmp.size-1].AddOnTop(head);
        used[head] = true;
        incycle = true;
      }
      tmp[tmp.size-1].AddOnTop(cur);
      used[cur] = true;
    }
  }
//  stOutput << "MakeCanonical: turning " << *this;
  this->cycles = tmp;
//  stOutput << " into " << *this << ".";
}

int PermutationR2::BiggestOccurringNumber() const
{ int bon=0;
  for(int i=0; i<this->cycles.size; i++)
    for(int j=0; j<this->cycles[i].size; j++)
      if(bon < this->cycles[i][j])
        bon = this->cycles[i][j];
  return bon;
}

int PermutationR2::operator*(int cur) const
{ for(int i=0; i<this->cycles.size; i++)
    for(int j=0; j<this->cycles[i].size; j++)
      if(this->cycles[i][j] == cur)
      { if(j+1 != this->cycles[i].size)
          cur = this->cycles[i][j+1];
        else
          cur = this->cycles[i][0];
        break;
      }
  return cur;
}

int PermutationR2::MakeFromMul(const PermutationR2& left, const PermutationR2& right)
{ int lbon = left.BiggestOccurringNumber();
  int rbon = right.BiggestOccurringNumber();
  int bon = (lbon > rbon)?lbon:rbon;
  List<bool> unused;
  unused.SetSize(bon+1);
  for(int i=0; i<bon; i++)
    unused[i] = true;
  bool incycle = false;
  this->cycles.SetSize(0);
  for(int head=0; head<bon; head++)
  { if(!unused[head])
      continue;
    unused[head] = false;
    int cur = head;
    while(true)
    { cur = right*cur;
      cur = left*cur;
      if(cur == head)
      { incycle = false;
        break;
      }
      if(!incycle)
      { this->cycles.SetSize(this->cycles.size+1);
        this->cycles[this->cycles.size-1].SetSize(0);
        this->cycles[this->cycles.size-1].AddOnTop(head);
        incycle = true;
      }
      this->cycles[this->cycles.size-1].AddOnTop(cur);
      unused[cur] = false;
    }
  }
  return bon;
}

PermutationR2 PermutationR2::operator*(const PermutationR2& right) const
{ PermutationR2 out;
  out.MakeFromMul(*this,right);
  return out;
}

void PermutationR2::MakeID(const PermutationR2& unused)
{ this->cycles.SetSize(0);
}

void PermutationR2::Invert()
{ for(int i=0; i<this->cycles.size; i++)
  { if(this->cycles[i].size == 2)
      continue;
    this->cycles[i].ReverseRange(1,this->cycles.size);
  }
}

void PermutationR2::Conjugate(const PermutationR2& conjugateMe, const PermutationR2& conjugateBy, PermutationR2& out)
{ PermutationR2 conjugateInverse = conjugateBy;
  conjugateInverse.Invert();
  PermutationR2 tmp;
  tmp.MakeFromMul(conjugateMe,conjugateBy);
  out.MakeFromMul(conjugateInverse,tmp);
}

bool PermutationR2::AreConjugate(const PermutationR2& x, const PermutationR2& y)
{ List<int> xcsh, ycsh;
  x.GetCycleStructure(xcsh);
  y.GetCycleStructure(ycsh);
  if(xcsh != ycsh)
    return false;
  return true;
}

int PermutationR2::Sign() const
{ int sign = 1;
  for(int i=0; i<this->cycles.size; i++)
    if(!(this->cycles[i].size % 2))
      sign *= -1;
  return sign;
}

void PermutationR2::BuildCycle(const List<int>& cycle)
{ this->cycles.AddOnTop(cycle);
}

void PermutationR2::BuildTransposition(int i, int j)
{ this->cycles.SetSize(this->cycles.size+1);
  this->cycles[0].SetSize(2);
  if(i<j)
  { this->cycles[0][0] = i;
    this->cycles[0][1] = j;
  }
  else
  { this->cycles[0][0] = j;
    this->cycles[0][1] = i;
  }
}

void PermutationR2::AddCycle(const List<int>& cycle)
{ this->BuildCycle(cycle);
  this->MakeCanonical();
}

void PermutationR2::AddTransposition(int i, int j)
{ this->BuildTransposition(i,j);
  this->MakeCanonical();
}

void PermutationR2::MakeFromListOfCycles(const List<List<int> >& inCycles)
{ this->cycles = inCycles;
  this->MakeCanonical();
}

// same code as operator* lolol
void PermutationR2::MakeFromActionDescription(const List<int>& actionDescription)
{ int bon = actionDescription.size-1;
  List<bool> unused;
  unused.SetSize(bon+1);
  for(int i=0; i<bon; i++)
    unused[i] = true;
  bool incycle = false;
  this->cycles.SetSize(0);
  for(int head=0; head<bon; head++)
  { if(!unused[head])
      continue;
    unused[head] = false;
    int cur = head;
    while(true)
    { cur = actionDescription[cur];
      if(cur == head)
      { incycle = false;
        break;
      }
      if(!incycle)
      { this->cycles.SetSize(this->cycles.size+1);
        this->cycles[this->cycles.size-1].SetSize(0);
        this->cycles[this->cycles.size-1].AddOnTop(head);
        incycle = true;
      }
      this->cycles[this->cycles.size-1].AddOnTop(cur);
      unused[cur] = false;
    }
  }
}

void PermutationR2::GetCycleStructure(List<int>& out) const
{ int N = 0;
  for(int i=0; i<this->cycles.size; i++)
    if(N<this->cycles[i].size)
      N = this->cycles[i].size;
  out.initFillInObject(N+1,0);

  for(int i=0; i<this->cycles.size; i++)
    out[this->cycles[i].size] += 1;
}

void PermutationR2::GetCycleStructure(Partition& out, int n_in_Sn) const
{ // unless otherwise specified, use the smallest group we are
  // canonically an element of
  if(n_in_Sn == -1)
    n_in_Sn = this->BiggestOccurringNumber();
  out.n = n_in_Sn;
  out.p.SetSize(this->cycles.size);
  for(int i=0; i<this->cycles.size; i++)
    out.p[i] = this->cycles[i].size;
  out.p.QuickSortDescending();
}

void PermutationR2::MakeFromTableauRows(const Tableau& in)
{ this->cycles.SetSize(in.t.size);
  for(int i=0; i<in.t.size; i++)
    this->cycles[i] = in.t[i];
  this->MakeCanonical();
}

bool PermutationR2::operator==(const PermutationR2& right) const
{ return this->cycles == right.cycles;
}

unsigned int PermutationR2::HashFunction() const
{ unsigned int acc = 0;
  unsigned int n = 0;
  for(int i=0; i<this->cycles.size; i++)
    for(int j=0; j<this->cycles[i].size; j++)
    { acc += SomeRandomPrimes[n] * this->cycles[i][j];
      n++;
      if(n >= SomeRandomPrimesSize)
        n = 0;
    }
  return acc;
}


std::string PermutationR2::ToString() const
{ std::stringstream out;
  this->IntoStream(out);
  return out.str();
}




//  MonomialTensor<T1, T2> operator*(MonomialTensor<T1,T2>& right) const;
// should this get stuffed in MonomialTensor?
void PermutationR2::ActOnMonomialTensor(MonomialTensor<int,MathRoutines::IntUnsignIdentity>& out, const MonomialTensor<int,MathRoutines::IntUnsignIdentity>& in) const
{// stOutput << "Debugging PermutationR2::ActOnMonomialTensor: " << *this << " acting on " << in << "\n";
  int rank=0;
  for(int i=0; i<in.Powers.size; i++)
    rank += in.Powers[i];
  List<int> expanded;
  expanded.SetSize(rank);
  for(int i=0,cur=0; i<in.generatorsIndices.size; i++)
     for(int j=0; j<in.Powers[i]; j++)
     { expanded[cur] = in.generatorsIndices[i];
         cur++;
     }
  //stOutput << "Exponents expanded: ";
  //for(int i=0; i<expanded.size; i++)
  //  stOutput << expanded[i] << " ";
  this->ActOnList(expanded);
  //stOutput << "\nIndices permuted:   ";
  //for(int i=0; i<expanded.size; i++)
  //  stOutput << expanded[i] << " ";
  //stOutput << "\n";
  int i=0, xi=0;
  while(xi < expanded.size)
  { out.generatorsIndices.AddOnTop(expanded[xi]);
    xi++;
    for(int j=1; ; j++,xi++)
      if((xi==expanded.size) || (expanded[xi]!=out.generatorsIndices[i]))
      { out.Powers.AddOnTop(j);
        break;
      }
    i++;
  }
  //stOutput << "Reassembled: " << out << "\n";
}


void PermutationGroup::MakeSymmetricGroup(int n)
{ this->generators.SetSize(n-1);
  for(int i=0; i<n-1; i++)
  { this->generators[i].AddTransposition(0,i+1);
  }
  isSymmetricGroup = true;
}

bool PermutationGroup::AreConjugate(const PermutationR2& x, const PermutationR2& y)
{ if(this->isSymmetricGroup)
    return PermutationR2::AreConjugate(x,y);
  return this->SimpleFiniteGroup<PermutationR2>::AreConjugate(x,y);
}


std::string PermutationGroup::ToString() const
{ std::stringstream out;
  this->IntoStream(out);
  return out.str();
}



void ElementHyperoctahedralGroup::MakeFromPermutation(const PermutationR2& in)
{ this->p = in;
}

void ElementHyperoctahedralGroup::MakeFromBits(const List<bool>& in)
{ this->s = in;
}

bool NeedRankAdjustment(const ElementHyperoctahedralGroup& left, const ElementHyperoctahedralGroup& right)
{ int theRank=left.s.size;
  if (theRank==0)
    theRank = right.s.size;
  if (theRank==0)
    return false;
  if (left.s.size==0)
    return true;
  else
    if (left.s.size!=theRank)
      crash <<"Change here and maybe elsewhere if you want to do stuff with multiple ranks of ElementHyperoctahedralGroup" << crash;
  if (right.s.size==0)
    return true;
  else
    if (right.s.size!=theRank)
      crash <<"Change here and maybe elsewhere if you want to do stuff with multiple ranks of ElementHyperoctahedralGroup" << crash;
  return false;
}


void EnsureSameRank(ElementHyperoctahedralGroup& left, ElementHyperoctahedralGroup& right)
{ int theRank=left.s.size;
  if (theRank==0)
    theRank = right.s.size;
  if (theRank==0)
    return;
  if (left.s.size==0)
    left.s.initFillInObject(theRank, false);
  if (right.s.size==0)
    right.s.initFillInObject(theRank, false);
}

void ElementHyperoctahedralGroup::MakeFromMul(const ElementHyperoctahedralGroup& left, const ElementHyperoctahedralGroup& right)
{ int prank = this->p.MakeFromMul(left.p,right.p)+1;
  int rightprank = right.p.BiggestOccurringNumber()+1;
  int thisssize = std::max(left.s.size,rightprank);
  thisssize = std::max(thisssize, right.s.size);
  this->s.SetSize(thisssize);
  for(int i=0; i<left.s.size; i++)
    this->s[i] = left.s[i];
  for(int i=left.s.size; i<thisssize; i++)
    this->s[i] = false;
  // see the documentation for ElementHyperoctahedralGroup if you
  // don't understand why the right wants to act on the left in this way
  // the following three lines are the mathematical part of this function,
  // the rest is to ensure the element looks good or whatever
  right.p.ActOnList(this->s);
  for(int i=0; i<right.s.size; i++)
    this->s[i] ^= right.s[i];

  int lastset = 0;
  for(int i=0; i<thisssize; i++)
    if(this->s[i])
      lastset = i;
  this->s.SetSize(std::min(prank,lastset));

/*  if(NeedRankAdjustment(left, right))
  { ElementHyperoctahedralGroup newLeft=left;
    ElementHyperoctahedralGroup newRight=right;
    EnsureSameRank(newLeft, newRight);
    this->MakeFromMul(newLeft, newRight);
    return;
  }
  if(right.s.size != 0)
  { this->s = left.s;
    right.p.ActOnList(this->s);
    for(int i=0; i<this->s.size; i++)
      this->s[i] ^= right.s[i];
  }
  if(left.s.size != 0)
  { this->s = left.s;
    right.p.ActOnList(this->s);
  }*/

  //stOutput << "Multiplying:" << left << "*" << right << "=" << *this;
}

ElementHyperoctahedralGroup ElementHyperoctahedralGroup::operator*(const ElementHyperoctahedralGroup& right) const
{ ElementHyperoctahedralGroup out;
  out.MakeFromMul(*this, right);
  return out;
}

void ElementHyperoctahedralGroup::MakeID(const ElementHyperoctahedralGroup& unused)
{ this->p.MakeID(this->p);
  this->s.SetSize(0);
}

void ElementHyperoctahedralGroup::Invert()
{ this->p.Invert();
}

void ElementHyperoctahedralGroup::Conjugate(const ElementHyperoctahedralGroup& element, const ElementHyperoctahedralGroup& conjugateBy, ElementHyperoctahedralGroup& out)
{ ElementHyperoctahedralGroup conjugateInverse = conjugateBy;
  conjugateInverse.Invert();
  ElementHyperoctahedralGroup tmp;
  tmp.MakeFromMul(element,conjugateBy);
  out.MakeFromMul(conjugateInverse,tmp);
}

int ElementHyperoctahedralGroup::CountSetBits() const
{ int cnt = 0;
  for(int i=0; i<this->s.size; i++)
    if(this->s[i])
      cnt++;
  return cnt;
}

bool ElementHyperoctahedralGroup::HasDifferentConjugacyInvariantsFrom(const ElementHyperoctahedralGroup& other) const
{ return !ElementHyperoctahedralGroup::AreConjugate(*this, other);
}

//static gets applied exactly once for some reason
bool ElementHyperoctahedralGroup::AreConjugate(const ElementHyperoctahedralGroup& x, const ElementHyperoctahedralGroup& y)
{ int xsp = x.CountSetBits();
  int ysp = y.CountSetBits();
  if(xsp != ysp)
    return false;
  return PermutationR2::AreConjugate(x.p,y.p);
}

void ElementHyperoctahedralGroup::GetCharacteristicPolyStandardRepresentation(Polynomial<Rational>& out) const
{ Matrix<Rational> M;
  int n = this->s.size;
  M.init(n,n);
  M.MakeZero();
  for(int i=0; i<n; i++)
  { int a = this->p*i;
    M.elements[i][a] = (this->s[i])?-1:1; // ahaha 0 is false
  }
  out.AssignCharPoly(M);
}


bool ElementHyperoctahedralGroup::operator==(const ElementHyperoctahedralGroup& right) const
{ /*if(this->s.size > 0)
    if(right.s.size > 0)
      for(int i=0; i<this->s.size; i++)
        if(this->s[i] != right.s[i])
          return false;
  if(this->s.size > 0)
    if(right.s.size == 0)
      for(int i=0; i<this->s.size; i++)
        if(this->s[i] != false)
          return false;
  if(this->s.size == 0)
    if(right.s.size > 0)
      for(int i=0; i<right.s.size; i++)
        if(right.s[i] != false)
          return false;*/
  int sharedsize = std::min(this->s.size, right.s.size);
  for(int i=0; i<sharedsize; i++)
    if(this->s[i] != right.s[i])
      return false;
  for(int i=sharedsize; i<this->s.size; i++)
    if(this->s[i])
      return false;
  for(int i=sharedsize; i<right.s.size; i++)
    if(right.s[i])
      return false;
  return this->p == right.p;
}

unsigned int ElementHyperoctahedralGroup::HashFunction() const
{ int acc = this->p.HashFunction();
  // idk how to make bools their own like hash function or whatever lol
  // I mean why not just have a parametric collection of global HashFunction
  // that does normal stuff to normal types and calls .HashFunction() on
  // class types or whatever
  // its ok though because I need to be sure that a string of false at the end
  // hashes to the exact same thing as not having a string of false at the end
  for(int i=0; (i<this->s.size)&&(i<SomeRandomPrimesSize); i++)
    acc += this->s[i] ? SomeRandomPrimes[i] : 0;
  return acc;
}

bool ElementHyperoctahedralGroup::operator>(const ElementHyperoctahedralGroup& right) const
{ int n = this->s.size;
  // the following register load and jump instructions will likely never be used
  // but they are fast
  int nr = right.s.size;
  if(n > nr)
    return true;
  if(n<nr)
    return false;
  // ok back to stuff that gets used
  Partition l,r;
  this->p.GetCycleStructure(l,n);
  right.p.GetCycleStructure(r,n);
  if(l>r)
    return true;
  if(r>l)
    return false;
  if(this->p.cycles>right.p.cycles)
    return true;
  if(this->p.cycles<right.p.cycles)
    return false;
  if(this->s>right.s)
    return true;
  if(this->s<right.s)
    return false;
  return false;
}


std::string ElementHyperoctahedralGroup::ToString() const
{ std::stringstream out;
  this->IntoStream(out);
  return out.str();
}

void HyperoctahedralGroup::MakeHyperoctahedralGroup(int n)
{ this->generators.SetSize(n-1+n);
  for(int i=0; i<n-1; i++)
  { this->generators[i].p.AddTransposition(0,i+1);
    this->generators[i].s.SetSize(n);
    for(int j=0; j<n; j++)
      this->generators[i].s[j] = false;
  }
  for(int i=0; i<n; i++)
  { this->generators[i+n-1].s.SetSize(n);
    for(int j=0; j<n; j++)
    { this->generators[i+n-1].s[j] = false;
    }
    this->generators[i+n-1].s[i] = true;
  }
}

bool HyperoctahedralGroup::AreConjugate(const ElementHyperoctahedralGroup& x, const ElementHyperoctahedralGroup& y)
{ if(this->isEntireHyperoctahedralGroup)
    return ElementHyperoctahedralGroup::AreConjugate(x,y);
//  if(this->isEntireDn)
//    return ElementHyperoctahedralGroup::AreConjugateDn(x,y);
  return SimpleFiniteGroup::AreConjugate(x,y);
}



std::string HyperoctahedralGroup::ToString() const
{ std::stringstream out;
  this->IntoStream(out);
  return out.str();
}


template <>
bool FiniteGroup<ElementHyperoctahedralGroup>::AreConjugate
(const ElementHyperoctahedralGroup& x, const ElementHyperoctahedralGroup& y)
{return ElementHyperoctahedralGroup::AreConjugate(x,y);}
