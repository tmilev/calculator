#include "math_extra_finite_groups_implementation.h"
#include "math_general_implementation.h"
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "math_extra_symmetric_groups_and_generalizations.h"

// conjugacy classes of type P are of size |Sn|/P.fulton61z()
int Partition::fulton61z() const {
  int acc = 1;
  int samecount = 1;
  for (int j = 1; j <= this->p.size; j ++) {
    // what kind of logic is this?  Make it a do...while loop.
    if ((j != this->p.size) && (this->p[j - 1] == this->p[j])) {
      samecount ++;
    } else {
      acc *= MathRoutines::kToTheNth(this->p[j - 1], samecount);
      acc *= MathRoutines::factorial(samecount);
      samecount = 1;
    }
  }
  return acc;
  /*
int acc = 1;
  List<int> nums;
  List<int> counts;
  for (int i = 0; i < this->p.size; i ++) {
    int numdex = nums.getIndex(this->p[i]);
    if (numdex == - 1) {
      nums.addOnTop(this->p[i]);
      counts.addOnTop(1);
    } else {
      counts[numdex] += 1;
    }
  }
  for (int i = 0; i < nums.size; i ++) {
    int acci = 1;
    for (int j = 0; j < counts[i]; j ++)
      acci *= nums[i];
    for (int j = 1; j < counts[i]; j ++)
      acci *= j;
    acc *= acci;
  }
  return acc;*/
}

int& Partition::operator[](int i) const {
  return p[i];
}

void Partition::fromListInt(const List<int>& in, int lastElement) {
  this->n = 0;
  this->p.setSize(0);
  int l = - 1;
  int i = 0;
  if (lastElement == - 1) {
    lastElement = in.size;
  }
  bool needsSorting = false;
  for (; i < lastElement; i ++) {
    if (in[i] < 0) {
      global.fatal
      << "Partitions should not have negative numbers in them. "
      << global.fatal;
    }
    if ((in[i] > l) && (l != - 1)) {
      needsSorting = true;
    }
    l = in[i];
    if (in[i] == 0) {
      continue;
    }
    this->n += in[i];
    this->p.addOnTop(in[i]);
  }
  if (needsSorting) {
    p.quickSortDescending();
  }
}

void Partition::GetPartitions(List<Partition>& out, int n) {
  out.setSize(0);
  if (n == 0) {
    out.setSize(1);
    out[0].n = 0;
    out[0].p.setSize(0);
    return;
  }
  if (n == 1) {
    out.setSize(1);
    out[0].n = 1;
    out[0].p.setSize(0);
    out[0].p.addOnTop(1);
    return;
  }
  List<int> p;
  p.initializeFillInObject(n, 0);
  p[1] = n;
  int k = 1;
  while (k != 0) {
    int x = p[k - 1] + 1;
    int y = p[k] - 1;
    k -= 1;
    while (x <= y) {
      p[k] = x;
      y -= x;
      k += 1;
    }
    p[k] = x + y;
    // out.setSize(out.size + 1);
    // out[out.size- 1].fromListInt(p,k+ 1);
    int s = out.size;
    out.setSize(s + 1);
    out[s].p.setSize(k + 1);
    for (int i = 0; i < k + 1; i ++) {
      out[s].p[i] = p[k - i];
    }
    out[s].n = n;
  }
}

void Partition::transpose() {
  if (this->n == 0) {
    return;
  }
  // if a partition is laid out like this:
  // example: partition [4,2,1]:
  // ****
  // **
  // *
  // then the transpose operation transposes the boxes above to give the
  // partition
  // [3,2,1,1]:
  // ***
  // **
  // *
  // *
  Partition myCopy = *this;
  this->p.setSize(myCopy.p[0]);
  for (int i = 0; i < *myCopy.p.lastObject(); i ++) {
    this->p[i] = myCopy.p.size;
  }
  int currentIndex = *myCopy.p.lastObject();
  for (int i = myCopy.p.size - 2; i >= 0; i --) {
    for (int j = 0; j < myCopy.p[i] - myCopy.p[i + 1]; j ++) {
      this->p[currentIndex] = i + 1;
      currentIndex ++;
    }
  }
}

void Partition::fillTableau(Tableau& out, List<int>& stuffing) const {
  if (stuffing.size < this->n) {
    global.fatal
    << "need at least "
    << this->n
    << " things to stuff tableau with partition "
    << *this
    << " but you provided only "
    << stuffing.size
    << '\n'
    << global.fatal;
  }
  out.t.setSize(this->p.size);
  int current = 0;
  for (int i = 0; i < this->p.size; i ++) {
    out.t[i].setSize(this->p[i]);
    for (int j = 0; j < this->p[i]; j ++) {
      out.t[i][j] = stuffing[current];
      current ++;
    }
  }
}

void Partition::fillTableauOrdered(Tableau& in) const {
  in.t.setSize(this->p.size);
  int cur = 0;
  for (int i = 0; i < this->p.size; i ++) {
    in.t[i].setSize(this->p[i]);
    for (int j = 0; j < this->p[i]; j ++) {
      in.t[i][j] = cur;
      cur ++;
    }
  }
}

void Partition::getAllStandardTableaux(List<Tableau>& out) const {
  GeneratorPermutationsOfList<int> perms;
  for (
    perms.initialize(this->n); !perms.doneIterating(); ++ perms
  ) {
    Tableau tableau;
    this->fillTableau(tableau, *perms);
    if (tableau.isStandard()) {
      out.addOnTop(tableau);
    }
  }
}

void Partition::testAllSpechtModules(int n) {
  STACK_TRACE("Partition::testAllSpechtModules");
  List<Partition> partitions;
  Partition::GetPartitions(partitions, n);
  int fac = 1;
  for (int i = 1; i <= n; i ++) {
    fac *= i;
  }
  for (int i = 0; i < partitions.size; i ++) {
    global.comments
    << partitions[i]
    << "\n";
    List<Matrix<Rational> > repgens;
    partitions[i].spechtModuleMatricesOfTranspositionsjjplusone(repgens);
    for (int ri = 0; ri < repgens.size; ri ++) {
      global.comments
      << repgens[ri].toStringPlainText();
      Rational det = repgens[ri].getDeterminant();
      global.comments << " determinant is " << det << "\n\n";
      if ((det != 1) && (det != - 1)) {
        global.fatal << "invalid determinant" << global.fatal;
      }
    }
    FiniteGroup<Matrix<Rational> > outg;
    outg.generators = repgens;
    outg.computeConjugacyClassSizesAndRepresentatives();
    if (outg.elements.size != 0) {
      if ((fac % outg.elements.size) != 0) {
        global.fatal << "invalid elements count" << global.fatal;
      }
    }
  }
}

bool Partition::operator==(const Partition& right) const {
  return this->p == right.p;
}

bool Partition::operator!=(const Partition& right) const {
  return this->p != right.p;
}

// largest numbers first
bool Partition::operator<(const Partition& right) const {
  if (this->n < right.n) {
    return true;
  }
  if (this->n > right.n) {
    return false;
  }
  // largest first
  if (this->p > right.p) {
    return true;
  }
  if (this->p < right.p) {
    return false;
  }
  return false;
}

// is there a better way to do this then copypasting codes?
bool Partition::operator>(const Partition& right) const {
  if (this->n < right.n) {
    return false;
  }
  if (this->n > right.n) {
    return true;
  }
  // largest first
  if (this->p > right.p) {
    return false;
  }
  if (this->p < right.p) {
    return true;
  }
  return false;
}

std::string Partition::toString() const {
  std::stringstream out;
  this->intoStream(out);
  return out.str();
}

std::string Partition::toStringForArticles(
  const std::string& leftParenthesis,
  const std::string& rightParenthesis
) const {
  if (this->n == 0) {
    return "\\emptyset";
  }
  std::stringstream out;
  out << leftParenthesis;
  for (int i = 0; i < this->p.size; i ++) {
    out << this->p[i];
    if (i != this->p.size - 1) {
      out << ", ";
    }
  }
  out << rightParenthesis;
  return out.str();
}

// Tableau implementation
List<int> Tableau::turnIntoList() const {
  List<int> out;
  for (int i = 0; i < this->t.size; i ++) {
    for (int j = 0; j < this->t[i].size; j ++) {
      out.addOnTop(this->t[i][j]);
    }
  }
  return out;
}

void Tableau::getColumns(List<List<int> >& output) const {
  output.setSize(0);
  if (this->t.size == 0) {
    return;
  }
  output.setSize(this->t[0].size);
  for (int i = 0; i < this->t[0].size; i ++) {
    for (int j = 0; j < t.size && t[j].size > i; j ++) {
      output[i].addOnTop(t[j][i]);
    }
  }
}

bool Tableau::isStandard() const {
  if (this->t.size == 0) {
    return true;
  }
  for (int i = 0; i < this->t.size; i ++) {
    int cur = - 1;
    for (int j = 0; j < this->t[i].size; j ++) {
      if (!(cur < this->t[i][j])) {
        return false;
      }
      cur = this->t[i][j];
    }
  }
  for (int i = 0; i < this->t[0].size; i ++) {
    int cur = - 1;
    int j = 0;
    while (true) {
      if (j == this->t.size) {
        break;
      }
      if (this->t[j].size <= i) {
        break;
      }
      if (!(cur <= this->t[j][i])) {
        return false;
      }
      cur = this->t[j][i];
      j ++;
    }
  }
  return true;
}

void Tableau::rowStabilizer(FiniteGroup<PermutationR2>& in) const {
  for (int i = 0; i < this->t.size; i ++) {
    if (this->t[i].size == 1) {
      continue;
    }
    for (int j = 1; j < this->t[i].size; j ++) {
      in.generators.setSize(in.generators.size + 1);
      in.generators[in.generators.size - 1].buildTransposition(
        this->t[i][0], this->t[i][j]
      );
    }
  }
}

void Tableau::columnStabilizer(FiniteGroup<PermutationR2>& in) const {
  if (this->t.size == 0) {
    return;
  }
  for (int i = 0; i < this->t[0].size; i ++) {
    int j = 1;
    while (true) {
      if ((j == this->t.size) || (this->t[j].size <= i)) {
        break;
      }
      in.generators.setSize(in.generators.size + 1);
      in.generators[in.generators.size - 1].buildTransposition(
        this->t[0][i], this->t[j][i]
      );
      j ++;
    }
  }
}

std::string Tableau::toString() const {
  std::stringstream out;
  this->intoStream(out);
  return out.str();
}

// int operator*(int) expects the Permutation to be canonical
// having each number exactly once
// this method can take any list of cyles
void PermutationR2::makeCanonical() {
  int bon = this->largestOccurringNumber();
  List<bool> used;
  used.initializeFillInObject(bon + 1, false);
  for (int i = 0; i < used.size; i ++) {
    used[i] = false;
  }
  List<List<int> > tmp;
  for (int i = 0; i <= bon; i ++) {
    if (used[i]) {
      continue;
    }
    int head = i;
    int cur = head;
    bool incycle = false;
    while (true) {
      // same as int operator*(int) but without the short circuit
      for (int j = this->cycles.size - 1; j >= 0; j --) {
        for (int k = 0; k < this->cycles[j].size; k ++) {
          if (this->cycles[j][k] != cur) {
            continue;
          }
          if (k != this->cycles[j].size - 1) {
            cur = this->cycles[j][k + 1];
          } else {
            cur = this->cycles[j][0];
          }
          break;
        }
      }
      if (cur == head) {
        break;
      }
      if (!incycle) {
        tmp.setSize(tmp.size + 1);
        tmp[tmp.size - 1].addOnTop(head);
        used[head] = true;
        incycle = true;
      }
      tmp[tmp.size - 1].addOnTop(cur);
      used[cur] = true;
    }
  }
  this->cycles = tmp;
}

int PermutationR2::largestOccurringNumber() const {
  int bon = 0;
  for (int i = 0; i < this->cycles.size; i ++) {
    for (int j = 0; j < this->cycles[i].size; j ++) {
      if (bon < this->cycles[i][j]) {
        bon = this->cycles[i][j];
      }
    }
  }
  return bon;
}

int PermutationR2::operator*(int cur) const {
  for (int i = 0; i < this->cycles.size; i ++) {
    for (int j = 0; j < this->cycles[i].size; j ++) {
      if (this->cycles[i][j] == cur) {
        if (j + 1 != this->cycles[i].size) {
          cur = this->cycles[i][j + 1];
        } else {
          cur = this->cycles[i][0];
        }
        break;
      }
    }
  }
  return cur;
}

int PermutationR2::makeFromMultiplicities(
  const PermutationR2& left, const PermutationR2& right
) {
  int lbon = left.largestOccurringNumber();
  int rbon = right.largestOccurringNumber();
  int bon = (lbon > rbon) ? lbon : rbon;
  List<bool> unused;
  unused.setSize(bon + 1);
  for (int i = 0; i < bon; i ++) {
    unused[i] = true;
  }
  bool incycle = false;
  this->cycles.setSize(0);
  for (int head = 0; head < bon; head ++) {
    if (!unused[head]) {
      continue;
    }
    unused[head] = false;
    int cur = head;
    while (true) {
      cur = right * cur;
      cur = left * cur;
      if (cur == head) {
        incycle = false;
        break;
      }
      if (!incycle) {
        this->cycles.setSize(this->cycles.size + 1);
        this->cycles[this->cycles.size - 1].setSize(0);
        this->cycles[this->cycles.size - 1].addOnTop(head);
        incycle = true;
      }
      this->cycles[this->cycles.size - 1].addOnTop(cur);
      unused[cur] = false;
    }
  }
  return bon;
}

PermutationR2 PermutationR2::operator*(const PermutationR2& right) const {
  PermutationR2 out;
  out.makeFromMultiplicities(*this, right);
  return out;
}

void PermutationR2::makeIdentity(const PermutationR2& unused) {
  (void) unused;
  this->cycles.setSize(0);
}

void PermutationR2::invert() {
  for (int i = 0; i < this->cycles.size; i ++) {
    if (this->cycles[i].size == 2) {
      continue;
    }
    this->cycles[i].reverseRange(1, this->cycles[i].size);
  }
}

PermutationR2 PermutationR2::operator^(const PermutationR2& right) const {
  PermutationR2 tmp = right;
  tmp.invert();
  return right *(*this) * tmp;
}

void PermutationR2::conjugationAction(
  const PermutationR2& conjugateWith,
  const PermutationR2& conjugateOn,
  PermutationR2& out
) {
  out = conjugateOn ^ conjugateWith;
}

bool PermutationR2::areConjugate(
  const PermutationR2& x, const PermutationR2& y
) {
  List<int> xcsh, ycsh;
  x.getCycleStructure(xcsh);
  y.getCycleStructure(ycsh);
  if (xcsh != ycsh) {
    return false;
  }
  return true;
}

int PermutationR2::sign() const {
  int sign = 1;
  for (int i = 0; i < this->cycles.size; i ++) {
    if (!(this->cycles[i].size % 2)) {
      sign *= - 1;
    }
  }
  return sign;
}

void PermutationR2::buildCycle(const List<int>& cycle) {
  this->cycles.addOnTop(cycle);
}

void PermutationR2::buildTransposition(int i, int j) {
  this->cycles.setSize(this->cycles.size + 1);
  this->cycles[0].setSize(2);
  if (i < j) {
    this->cycles[0][0] = i;
    this->cycles[0][1] = j;
  } else {
    this->cycles[0][0] = j;
    this->cycles[0][1] = i;
  }
}

void PermutationR2::addCycle(const List<int>& cycle) {
  this->buildCycle(cycle);
  this->makeCanonical();
}

void PermutationR2::addTransposition(int i, int j) {
  this->buildTransposition(i, j);
  this->makeCanonical();
}

void PermutationR2::makeFromListOfCycles(
  const List<List<int> >& inCycles
) {
  this->cycles = inCycles;
  this->makeCanonical();
}

// same code as operator* lolol
void PermutationR2::makeFromActionDescription(
  const List<int>& actionDescription
) {
  int bon = actionDescription.size - 1;
  List<bool> unused;
  unused.setSize(bon + 1);
  for (int i = 0; i < bon; i ++) {
    unused[i] = true;
  }
  bool incycle = false;
  this->cycles.setSize(0);
  for (int head = 0; head < bon; head ++) {
    if (!unused[head]) {
      continue;
    }
    unused[head] = false;
    int cur = head;
    while (true) {
      cur = actionDescription[cur];
      if (cur == head) {
        incycle = false;
        break;
      }
      if (!incycle) {
        this->cycles.setSize(this->cycles.size + 1);
        this->cycles[this->cycles.size - 1].setSize(0);
        this->cycles[this->cycles.size - 1].addOnTop(head);
        incycle = true;
      }
      this->cycles[this->cycles.size - 1].addOnTop(cur);
      unused[cur] = false;
    }
  }
}

void PermutationR2::makeFromString(const std::string& cppin) {
  const char* in = cppin.c_str();
  unsigned insize = static_cast<unsigned>(cppin.length());
  List<List<int> > cycles;
  unsigned curintstart = 0;
  for (unsigned i = 0; i < insize; i ++) {
    switch (in[i]) {
    case '(':
      cycles.setSize(cycles.size + 1);
      curintstart = i + 1;
      break;
    case ')':
      if (curintstart != i) {
        std::string ss = cppin.substr(curintstart, i);
        cycles.lastObject()->addOnTop(atoi(ss.c_str()));
      } else {
        cycles.setSize(cycles.size - 1);
      }
      break;
    case ',':
      std::string ss = cppin.substr(curintstart, i);
      cycles.lastObject()->addOnTop(atoi(ss.c_str()));
      curintstart = i + 1;
      break;
    }
  }
  this->makeFromListOfCycles(cycles);
}

void PermutationR2::getCycleStructure(List<int>& out) const {
  int N = 0;
  for (int i = 0; i < this->cycles.size; i ++) {
    if (N < this->cycles[i].size) {
      N = this->cycles[i].size;
    }
  }
  out.initializeFillInObject(N + 1, 0);
  for (int i = 0; i < this->cycles.size; i ++) {
    out[this->cycles[i].size] += 1;
  }
}

void PermutationR2::getCycleStructure(Partition& out, int n_in_Sn) const {
  // unless otherwise specified, use the smallest group we are
  // canonically an element of
  if (n_in_Sn == - 1) {
    n_in_Sn = this->largestOccurringNumber();
  }
  out.n = n_in_Sn;
  out.p.setSize(this->cycles.size);
  for (int i = 0; i < this->cycles.size; i ++) {
    out.p[i] = this->cycles[i].size;
  }
  out.p.quickSortDescending();
}

void PermutationR2::makeFromTableauRows(const Tableau& in) {
  this->cycles.setSize(in.t.size);
  for (int i = 0; i < in.t.size; i ++) {
    this->cycles[i] = in.t[i];
  }
  this->makeCanonical();
}

bool PermutationR2::operator==(const PermutationR2& right) const {
  return this->cycles == right.cycles;
}

bool PermutationR2::isIdentity() const {
  return (this->cycles.size == 0);
}

unsigned int PermutationR2::hashFunction() const {
  unsigned int acc = 0;
  int n = 0;
  for (int i = 0; i < this->cycles.size; i ++) {
    for (int j = 0; j < this->cycles[i].size; j ++) {
      acc +=
      HashConstants::getConstantIncrementCounter(n) * static_cast<unsigned>(
        this->cycles[i][j]
      );
    }
  }
  return acc;
}

std::string PermutationR2::toString(FormatExpressions* format) const {
  std::stringstream out;
  if (format != nullptr) {
    out << "(";
    for (int i = 0; i < this->cycles.size; i ++) {
      out << "(";
      for (int j = 0; j < this->cycles[i].size; j ++) {
        out << this->cycles[i][j] + 1;
        if (j != this->cycles[i].size - 1) {
          out << ", ";
        }
      }
      out << ")";
    }
    out << ")";
    return out.str();
  }
  this->intoStream(out);
  return out.str();
}

//  MonomialTensor<T1, T2> operator*(MonomialTensor<T1,T2>& right) const;
// should this get stuffed in MonomialTensor?
void PermutationR2::actOnMonomialTensor(
  MonomialTensor<int, HashFunctions::hashFunction>& out,
  const MonomialTensor<int, HashFunctions::hashFunction>& in
) const {
  int rank = 0;
  for (int i = 0; i < in.powers.size; i ++) {
    rank += in.powers[i];
  }
  List<int> expanded;
  expanded.setSize(rank);
  for (int i = 0, cur = 0; i < in.generatorsIndices.size; i ++) {
    for (int j = 0; j < in.powers[i]; j ++) {
      expanded[cur] = in.generatorsIndices[i];
      cur ++;
    }
  }
  this->actOnList(expanded);
  int i = 0, xi = 0;
  while (xi < expanded.size) {
    out.generatorsIndices.addOnTop(expanded[xi]);
    xi ++;
    for (int j = 1;; j ++, xi ++) {
      if ((xi == expanded.size) || (
          expanded[xi] != out.generatorsIndices[i]
        )
      ) {
        out.powers.addOnTop(j);
        break;
      }
    }
    i ++;
  }
}

void PermutationR2::getWordjjPlus1(List<int>& word) const {
  List<int> transpositions;
  for (int i = 0; i < this->cycles.size; i ++) {
    for (int j = 0; j < this->cycles[i].size - 1; j ++) {
      transpositions.addOnTop(this->cycles[i][j]);
      transpositions.addOnTop(this->cycles[i][j + 1]);
    }
  }
  for (int i = 0; i < transpositions.size; i += 2) {
    int ti = transpositions[i];
    int tj = transpositions[i + 1];
    if (ti > tj) {
      int tk = ti;
      ti = tj;
      tj = tk;
    }
    int tl = tj - ti - 1;
    for (int tt = 0; tt < tl; tt ++) {
      word.addOnTop(ti + tt);
    }
    word.addOnTop(tj - 1);
    int end = word.size - 2;
    for (int tt = 0; tt < tl; tt ++) {
      word.setSize(word.size + 1);
      word[word.size - 1] = word[end - tt];
    }
  }
}

void PermutationGroupData::makeSymmetricGroup(int n) {
  this->group = &this->groupMayBeHereNameIsLongToDiscourageUse;
  this->group->generators.setSize(n - 1);
  for (int i = 0; i < n - 1; i ++) {
    this->group->generators[i].addTransposition(0, i + 1);
  }
  this->flagIsSymmetricGroup = true;
  this->flagHasGenerators1j = true;
  this->group->computeConjugacyClassSizesAndRepresentativesByFormula =
  PermutationGroupData::
  computeCCSizesAndRepresentativesByFormulaImplementation;
  this->group->areConjugateByFormula = PermutationR2::areConjugate;
  this->group->getSizeByFormula =
  PermutationGroupData::getSizeByFormulaImplementation;
  this->group->specificDataPointer = this;
}

void PermutationGroupData::makeSymmetricGroupGeneratorsjjPlus1(int n) {
  this->group = &this->groupMayBeHereNameIsLongToDiscourageUse;
  this->group->generators.setSize(n - 1);
  for (int i = 0; i < n - 1; i ++) {
    this->group->generators[i].addTransposition(i, i + 1);
  }
  this->flagIsSymmetricGroup = true;
  this->flagHasGeneratorsjjPlus1 = true;
  this->group->computeConjugacyClassSizesAndRepresentativesByFormula =
  PermutationGroupData::
  computeCCSizesAndRepresentativesByFormulaImplementation;
  this->group->areConjugateByFormula = PermutationR2::areConjugate;
  this->group->getSizeByFormula =
  PermutationGroupData::getSizeByFormulaImplementation;
  this->group->getWordByFormula =
  PermutationGroupData::getWordjjPlus1Implementation;
  this->group->specificDataPointer = this;
}

/*bool PermutationGroup::areConjugate(const PermutationR2& x, const PermutationR2& y) {
  if (this->isSymmetricGroup)
    return PermutationR2::areConjugate(x,y);
  return this->SimpleFiniteGroup<PermutationR2>::areConjugate(x,y);
}*/
LargeInteger PermutationGroupData::getSizeByFormulaImplementation(
  FiniteGroup<PermutationR2>& G
) {
  PermutationGroupData* PD = static_cast<PermutationGroupData*>(G.specificDataPointer);
  if (!PD || !PD->flagIsSymmetricGroup) {
    global.fatal
    << "This method should not have been called. "
    << global.fatal;
    // control reaches end of non-void function
    return - 1;
  }
  return MathRoutines::factorial(G.generators.size + 1);
}

void PermutationGroupData::
computeCCSizesAndRepresentativesByFormulaImplementation(
  FiniteGroup<PermutationR2>& G
) {
  PermutationGroupData* PD = static_cast<PermutationGroupData*>(G.specificDataPointer);
  if (!PD || !PD->flagIsSymmetricGroup) {
    global.fatal << "This should not have been called. " << global.fatal;
  }
  G.flagCCsComputed = true;
  G.flagCCRepresentativesComputed = true;
  int N = G.generators.size + 1;
  List<Partition> parts;
  Partition::GetPartitions(parts, N);
  G.conjugacyClasses.setSize(parts.size);
  int facn = MathRoutines::factorial(N);
  for (int i = 0; i < parts.size; i ++) {
    Tableau t;
    parts[i].fillTableauOrdered(t);
    G.conjugacyClasses[i].representative.makeFromTableauRows(t);
    G.getWord(
      G.conjugacyClasses[i].representative,
      G.conjugacyClasses[i].representativeWord
    );
    G.conjugacyClasses[i].flagRepresentativeWordComputed = true;
    G.conjugacyClasses[i].size = facn / parts[i].fulton61z();
  }
}

bool PermutationGroupData::getWordjjPlus1Implementation(
  FiniteGroup<PermutationR2>& G,
  const PermutationR2& g,
  List<int>& word
) {
  (void) G;
  g.getWordjjPlus1(word);
  return true;
}

std::string PermutationGroupData::toString() {
  std::stringstream out;
  this->intoStream(out);
  return out.str();
}

void PermutationGroupData::computeSpechtModules() {
  List<Partition> ps;
  Partition::GetPartitions(ps, this->group->generators.size + 1);
  this->group->irreducibleRepresentations.setSize(ps.size);
  for (int i = 0; i < ps.size; i ++) this->spechtModuleOfPartition(
    ps[i], this->group->irreducibleRepresentations[i]
  );
  this->group->irreducibleRepresentations.quickSortAscending();
}

/*
void ElementHyperoctahedralGroup::MakeFromPermutation(const PermutationR2& in) {
  this->p = in;
}

void ElementHyperoctahedralGroup::MakeFromBits(const List<bool>& in) {
  this->s = in;
}

void ElementHyperoctahedralGroup::addTransposition(int i, int j) {
  this->p.addTransposition(i, j);
}

void ElementHyperoctahedralGroup::toggleBit(int i) {
  if (i + 1< this->s.size);
  { int n = this->s.size;
    this->s.setSize(i + 1);
    for (int j = n; j < i + 1; j ++)
      this->s[j] = false;
  }
  this->s[i] = !this->s[i];
}

bool NeedRankAdjustment(const ElementHyperoctahedralGroup& left, const ElementHyperoctahedralGroup& right) {
  int rank= left.s.size;
  if (rank == 0)
    rank = right.s.size;
  if (rank == 0)
    return false;
  if (left.s.size == 0)
    return true;
  else
    if (left.s.size != rank)
      global.fatal << "Change here and maybe elsewhere if you want to do stuff with multiple ranks of ElementHyperoctahedralGroup" << global.fatal;
  if (right.s.size == 0)
    return true;
  else
    if (right.s.size != rank)
      global.fatal << "Change here and maybe elsewhere if you want to do stuff with multiple ranks of ElementHyperoctahedralGroup" << global.fatal;
  return false;
}


void EnsureSameRank(ElementHyperoctahedralGroup& left, ElementHyperoctahedralGroup& right) {
  int rank= left.s.size;
  if (rank == 0)
    rank = right.s.size;
  if (rank == 0)
    return;
  if (left.s.size == 0)
    left.s.initializeFillInObject(rank, false);
  if (right.s.size == 0)
    right.s.initializeFillInObject(rank, false);
}

void ElementHyperoctahedralGroup::makeFromMultiplicities(const ElementHyperoctahedralGroup& left, const ElementHyperoctahedralGroup& right) {
  this->p.makeFromMultiplicities(left.p,right.p);
  int rightprank = right.p.largestOccurringNumber() + 1;
  int thisssize = std::max(left.s.size,rightprank);
  thisssize = std::max(thisssize, right.s.size);
  this->s.setSize(thisssize);
  for (int i = 0; i <left.s.size; i ++)
    this->s[i] = left.s[i];
  for (int i = left.s.size; i < thisssize; i ++)
    this->s[i] = false;
  // see the documentation for ElementHyperoctahedralGroup if you
  // don't understand why the right wants to act on the left in this way
  // the following three lines are the mathematical part of this function,
  // the rest is to ensure the element looks good
  auto pp = right.p;
  pp.invert();
  pp.actOnList(this->s);
  for (int i = 0; i <right.s.size; i ++)
    this->s[i] ^= right.s[i];

  int lastset = 0;
  for (int i = 0; i < this->s.size; i ++)
    if (this->s[i])
      lastset = i;
  this->s.setSize(lastset + 1);
  //global.Comments << left << "*" << right << "=" << *this << "   ";
}

int ElementHyperoctahedralGroup::SmallestN()  const {
  int pn = this->p.largestOccurringNumber() + 1;
  if (this->p.cycles.size == 0)
    pn = 0;
  int sn = 0;
  for (int i = 0; i < this->s.size; i ++)
    if (this->s[i])
      sn = i;
  return pn > sn ? pn : sn;
}

ElementHyperoctahedralGroup ElementHyperoctahedralGroup::operator*(const ElementHyperoctahedralGroup& right) const {
  ElementHyperoctahedralGroup out;
  out.makeFromMultiplicities(*this, right);
  return out;
}

void ElementHyperoctahedralGroup::makeIdentity(const ElementHyperoctahedralGroup& unused) {
  this->p.makeIdentity(this->p);
  this->s.setSize(0);
}

bool ElementHyperoctahedralGroup::isIdentity() const {
  if (!this->p.isIdentity())
    return false;
  for (int i = 0; i < this->s.size; i ++)
    if (this->s[i])
      return false;
  return true;
}

void ElementHyperoctahedralGroup::invert() {
  this->p.invert();
}

void ElementHyperoctahedralGroup::conjugationAction(const ElementHyperoctahedralGroup& conjugateWith, const ElementHyperoctahedralGroup& conjugateOn, ElementHyperoctahedralGroup& out) {
  out = conjugateOn^conjugateWith;
}

ElementHyperoctahedralGroup ElementHyperoctahedralGroup::operator^(const ElementHyperoctahedralGroup& right) const {
  ElementHyperoctahedralGroup rightInverse;
  rightInverse.p = right.p;
  rightInverse.p.invert();
  rightInverse.s = right.s;
  return right * (*this) * rightInverse;
}

int ElementHyperoctahedralGroup::CountSetBits() const {
  int cnt = 0;
  for (int i = 0; i < this->s.size; i ++)
    if (this->s[i])
      cnt ++;
  return cnt;
}

bool ElementHyperoctahedralGroup::hasDifferentConjugacyInvariantsFrom(const ElementHyperoctahedralGroup& other) const {
  return !ElementHyperoctahedralGroup::areConjugate(*this, other);
}

//static gets applied exactly once for some reason
bool ElementHyperoctahedralGroup::areConjugate(const ElementHyperoctahedralGroup& x, const ElementHyperoctahedralGroup& y) {
  int xsp = x.CountSetBits();
  int ysp = y.CountSetBits();
  if (xsp != ysp)
    return false;
  return PermutationR2::areConjugate(x.p,y.p);
}

void ElementHyperoctahedralGroup::getCharacteristicPolynomialStandardRepresentation(Polynomial<Rational>& out) const {
  Matrix<Rational> M;
  int n = this->SmallestN();
  if (n == 0) // this is the identity element.  it thinks it lives in the zero group, because it lives in all the groups
  { out.makeConstant(1);
    return;
  }
  M.initialize(n, n);
  M.makeZero();
  for (int i = 0; i < n; i ++) {
    int a = this->p*i;
    M.elements[i][a] = (this->s[i])?- 1:1; // ahaha 0 is false
  }
  out.assignCharacteristicPolynomial(M);
}


bool ElementHyperoctahedralGroup::operator==(const ElementHyperoctahedralGroup& right) const {
  int sharedsize = std::min(this->s.size, right.s.size);
  for (int i = 0; i <sharedsize; i ++)
    if (this->s[i] != right.s[i])
      return false;
  for (int i =sharedsize; i < this->s.size; i ++)
    if (this->s[i])
      return false;
  for (int i =sharedsize; i <right.s.size; i ++)
    if (right.s[i])
      return false;
  return this->p == right.p;
}

unsigned int ElementHyperoctahedralGroup::hashFunction() const {
  int acc = this->p.hashFunction();
  // idk how to make bools their own like hash function or whatever lol
  // I mean why not just have a parametric collection of global hashFunction
  // that does normal stuff to normal types and calls .hashFunction() on
  // class types or whatever
  // its ok though because I need to be sure that a string of false at the end
  // hashes to the exact same thing as not having a string of false at the end
  for (int i = 0; (i < this->s.size)&&(i <someRandomPrimesSize); i ++)
    acc += this->s[i] ? someRandomPrimes[i] : 0;
  return acc;
}

bool ElementHyperoctahedralGroup::operator>(const ElementHyperoctahedralGroup& right) const {
  int n = this->s.size;
  // the following register load and jump instructions will likely never be used
  // but they are fast
  int nr = right.s.size;
  if (n > nr)
    return true;
  if (n<nr)
    return false;
  // ok back to stuff that gets used
  Partition l,r;
  this->p.getCycleStructure(l,n);
  right.p.getCycleStructure(r,n);
  if (l>r)
    return true;
  if (r>l)
    return false;
  if (this->p.cycles>right.p.cycles)
    return true;
  if (this->p.cycles<right.p.cycles)
    return false;
  if (this->s>right.s)
    return true;
  if (this->s<right.s)
    return false;
  return false;
}


std::string ElementHyperoctahedralGroup::toString(FormatExpressions* unused) const {
  std::stringstream out;
  this->intoStream(out);
  return out.str();
}*/
/*void HyperoctahedralGroup::makeHyperoctahedralGroup(int n) {
  this->isEntireHyperoctahedralGroup = true;
// these methods are currently broken because I don't actually know what conjugacy
// in hyperoctahedral groups means
//this->computeConjugacyClassSizesAndRepresentativesByFormula = HyperoctahedralGroup::computeCCSizesAndRepresentativesByFormulaImplementation;
//  this->areConjugateByFormula = ElementHyperoctahedralGroup::areConjugate;
  this->getWordByFormula = HyperoctahedralGroup::getWordByFormulaImplementation;
  this->getSizeByFormula = HyperoctahedralGroup::getSizeByFormulaImplementation;
  this->N = n;
  this->generators.setSize(n- 1+n);
  for (int i = 0; i < n- 1; i ++) {
    this->generators[i].p.addTransposition(i,i + 1);
    this->generators[i].s.setSize(n);
    for (int j = 0; j < n; j ++)
      this->generators[i].s[j] = false;
  }
  for (int i = 0; i < n; i ++) {
    this->generators[i + n - 1].s.setSize(n);
    for (int j = 0; j < n; j ++) {
      this->generators[i + n - 1].s[j] = false;
    }
    this->generators[i + n - 1].s[i] = true;
  }
}

void HyperoctahedralGroup::MakeBn(int n) {
  this->isEntireHyperoctahedralGroup = true;
  this->N = n;
  this->generators.setSize(n- 1);
  for (int i = 0; i < this->generators.size; i ++)
    this->generators[i].addTransposition(i,i + 1);
  if (this->generators.size > 0)
    this->generators[0].toggleBit(0);
}



void HyperoctahedralGroup::spechtModuleOfPartititons(const Partition &positive, const Partition &negative,
                                                     GroupRepresentation<SimpleFiniteGroup<ElementHyperoctahedralGroup>, Rational> &out,
                                                     PermutationGroup *subsn) {
  if (this->generators.size > this->N)
    this->SpechtModuleOfPartititonsSymmetricPlusGenerators(positive, negative, out, subsn);
  else
    this->SpechtModuleOfPartitionsBnGenerators(positive, negative, out, subsn);
}*/
/*void HyperoctahedralGroup::spechtModuleOfPartititons(const Partition &positive, const Partition &negative,
                                                     GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroup>, Rational> &out) {
  List<Matrix<Rational> > pozm, negm;
  global.Comments << "HyperoctahedralGroup::SpectModuleOfPartitions(" << positive << ", " << negative << ")\n";
  // the next two things should be done in parallel.  How can I make that happen?
  positive.spechtModuleMatricesOfTranspositionsjjplusone(pozm);
  negative.spechtModuleMatricesOfTranspositionsjjplusone(negm);

  Subgroup<HyperoctahedralGroup, ElementHyperoctahedralGroup> Sn;
  auto Sngens = this->generators;
  Sngens.setSize(this->N- 1);
  Sn.makeTranslatableWordsSubgroup(*this,Sngens);

  Subgroup<Subgroup<HyperoctahedralGroup, ElementHyperoctahedralGroup>, ElementHyperoctahedralGroup> PxM;
  auto PxMgens = Sn.generators;
  if ((positive.n > 0) && (negative.n > 0))
    PxMgens.removeIndexShiftDown(positive.n);
  PxM.makeTranslatableWordsSubgroup(Sn,PxMgens);

  GroupRepresentation<Subgroup<Subgroup<HyperoctahedralGroup, ElementHyperoctahedralGroup>, ElementHyperoctahedralGroup>, Rational> pxmr;
  pxmr.ownerGroup = &PxM;
  pxmr.generators = pozm;
  pxmr.generators.addListOnTop(negm);
  GroupRepresentation<Subgroup<HyperoctahedralGroup, ElementHyperoctahedralGroup>, Rational> snr;
  PxM.induceRepresentation(pxmr,snr);
  GroupRepresentation<HyperoctahedralGroup, Rational> outreboxme;
  Sn.induceRepresentation(snr,outreboxme);

  out.ownerGroup = this;
  out.generators = outreboxme.generators;
  std::stringstream ids;
  ids << negative << ", " << positive;
  out.identifyingString = ids.str();
  if (!out.verifyRepresentation())
    global.fatal << global.fatal;
}

void HyperoctahedralGroup::spechtModuleOfPartititons(const Partition &positive, const Partition &negative,
                                                     GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroup>, Rational> &out) {
  List<Matrix<Rational> > pozm, negm;
  global.Comments << "HyperoctahedralGroup::SpectModuleOfPartitions(" << positive << ", " << negative << ")\n";
  // the next two things should be done in parallel.  How can I make that happen?
  positive.spechtModuleMatricesOfTranspositionsjjplusone(pozm);
  negative.spechtModuleMatricesOfTranspositionsjjplusone(negm);
  Subgroup<HyperoctahedralGroup, ElementHyperoctahedralGroup> PxM;
  List<ElementHyperoctahedralGroup> subgens;
  subgens.setSize(this->generators.size);
  for (int i = 0; i < this->generators.size; i ++)
    subgens[i] = this->generators[i];
  if ((positive.n > 0) && (negative.n > 0))
    subgens.removeIndexShiftDown(positive.n- 1);
  PxM.makeTranslatableWordsSubgroup(*this, subgens);
  global.Comments << "Generating subgroup:\n";
  for (int i = 0; i < this->generators.size; i ++)
    global.Comments << i << " " << this->generators[i] << " " << PxM.superGeneratorSubWordExists[i] << " "
             << PxM.superGeneratorSubWords[i].toStringCommaDelimited() << '\n';
  for (int i = 0; i <PxM.generators.size; i ++)
    global.Comments << i << " " << PxM.generators[i] << '\n';

  GroupRepresentation<Subgroup<HyperoctahedralGroup, ElementHyperoctahedralGroup>, Rational> pxmr;
  pxmr.ownerGroup = &PxM;
  pxmr.generators.setExpectedSize(PxM.generators.size);
  pxmr.generators.addListOnTop(pozm);
  pxmr.generators.addListOnTop(negm);
  int cur = pxmr.generators.size;
  pxmr.generators.setSize(PxM.generators.size);
  int repRank = pxmr.generators[0].numberOfRows;
  if (repRank == 0)
    repRank = 1;
  for (int i = 0; i < this->N; i ++)
    if (i < positive.n)
      pxmr.generators[cur+ i].makeIdentityMatrix(repRank);
    else {
      pxmr.generators[cur+ i].makeIdentityMatrix(repRank);
      pxmr.generators[cur+ i] *= - 1;
    }
  if (!pxmr.verifyRepresentation())
    global.fatal << "lol" << global.fatal;
  auto outreboxme = PxM.induceRepresentation(pxmr);
  out.ownerGroup = this;
  out.generators = outreboxme.generators;
  std::stringstream ids;
  ids << negative << ", " << positive;
  out.identifyingString = ids.str();
  if (!out.verifyRepresentation())
    global.fatal << global.fatal;
}*/
void HyperoctahedralGroupData::spechtModuleOfPartititons(
  const Partition& positive,
  const Partition& negative,
  GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational>&
  out
) {
  List<Matrix<Rational> > pozm, negm;
  // global.Comments << "HyperoctahedralGroupR2::SpectModuleOfPartitions(" <<
  // positive << ", " << negative << ")\n";
  positive.spechtModuleMatricesOfTranspositionsjjplusone(pozm);
  negative.spechtModuleMatricesOfTranspositionsjjplusone(negm);
  List<int> subgenids;
  for (int i = 0; i < this->group->generators.size; i ++) {
    subgenids.addOnTop(i);
  }
  if ((positive.n > 0) && (negative.n > 0)) {
    subgenids.removeIndexShiftDown(positive.n - 1);
  }
  auto PxM = this->group->parabolicKindaSubgroupGeneratorSubset(subgenids);
  auto pxmr = PxM.subgroupContent->getEmptyRationalRepresentation();
  pxmr.generators.setExpectedSize(PxM.subgroupContent->generators.size);
  pxmr.generators.addListOnTop(pozm);
  pxmr.generators.addListOnTop(negm);
  int cur = pxmr.generators.size;
  pxmr.generators.setSize(PxM.subgroupContent->generators.size);
  int repRank = pxmr.generators[0].numberOfRows;
  if (repRank == 0) {
    repRank = 1;
  }
  for (int i = 0; i < this->dimension; i ++) {
    if (i < positive.n) {
      pxmr.generators[cur + i].makeIdentityMatrix(repRank);
    } else {
      pxmr.generators[cur + i].makeIdentityMatrix(repRank);
      pxmr.generators[cur + i] *= - 1;
    }
  }
  if (!pxmr.verifyRepresentation()) {
    global.fatal << "Representation not verified. " << global.fatal;
  }
  out = PxM.induceRepresentation(pxmr);
  out.ownerGroup = this->group;
  std::stringstream ids;
  ids << negative << ", " << positive;
  out.identifyingString = ids.str();
  if (!out.verifyRepresentation()) {
    global.fatal << "Representation not verified at the end. " << global.fatal;
  }
}

/*
void HyperoctahedralGroup::SomeModuleOfPartititons(const Partition& positive, const Partition& negative,
                                                     GroupRepresentation<SimpleFiniteGroup<ElementHyperoctahedralGroup>,Rational> &out,
                                                     Subgroup<HyperoctahedralGroup, ElementHyperoctahedralGroup>* subsn) {
  List<Matrix<Rational> > pozm, negm;
  global.Comments << "HyperoctahedralGroup::SomeModuleOfPartitions(" << positive << ", " << negative << ")\n";
  // the next two things should be done in parallel.  How can I make that happen?
  positive.spechtModuleMatricesOfTranspositionsjjplusone(pozm);
  negative.spechtModuleMatricesOfTranspositionsjjplusone(negm);

  if (subsn == NULL) {
    Subgroup<HyperoctahedralGroup, ElementHyperoctahedralGroup> sn;
    subsn = &sn;
  }
  if (subsn->generators.size == 0) {
    List<ElementHyperoctahedralGroup> subgens;
    subgens.setSize(this->N- 1);
    for (int i = 0; i < this->N- 1; i ++)
      subgens[i] = this->generators[i];
    subsn->makeTranslatableWordsSubgroup(*this, subgens);
  }
  Subgroup<PermutationGroup, PermutationR2> PxM;
  List<int> subgens;
  subgens.setSize(subsn->generators.size);
  for (int i = 0; i <subsn->generators.size; i ++)
    subgens[i] = i;
  if (pozm.size + negm.size != subsn->generators.size)
    subgens.removeIndexShiftDown(pozm.size);
  PxM.makeTranslatableWordsSubgroup(*subsn, subgens);
  global.Comments << "Generating subgroup:\n";
  for (int i = 0; i <subsn->generators.size; i ++)
    global.Comments << i << " " << subsn->generators[i] << " " << PxM.superGeneratorSubWordExists[i] << " "
             << PxM.superGeneratorSubWords[i].toStringCommaDelimited() << '\n';
  for (int i = 0; i < PxM.generators.size; i ++)
    global.Comments << i << " " << PxM.generators[i] << '\n';
  //PxM.cosetRepresentativeEnumerator = HyperoctahedralGroup::ParabolicSubgroupCosetRepresentativeEnumerator;
  PxM.getWordByFormula = MissingGeneratorsSubgroupElementGetWord<PermutationGroup, PermutationR2>;
  GroupRepresentation<Subgroup<PermutationGroup, PermutationR2>, Rational> pxmr;
  pxmr.ownerGroup = &PxM;
  pxmr.generators.setSize(PxM.generators.size);
  Matrix<Rational> hid, kid;
  if (pozm.size == 0)
    hid.makeIdentityMatrix(1);
  else
    hid.makeIdentity(pozm[0]);
  if (negm.size == 0)
    kid.makeIdentityMatrix(1);
  else
    kid.makeIdentity(negm[0]);
  int i = 0;
  for (int pi = 0; pi < pozm.size; i ++, pi ++)
    pxmr.generators[i].assignTensorProduct(pozm[pi], kid);
  for (int ni = 0; ni < negm.size; i ++, ni ++) {
    pxmr.generators[i].assignTensorProduct(hid, negm[ni]);
    pxmr.generators[i] *= - 1;
  }
  GroupRepresentation<PermutationGroup, Rational> subsnr;
  PxM.induceRepresentation(pxmr,subsnr);
  subsnr.induceRepresentation(subsnr,out);
  std::stringstream ids;
  ids << negative << ", " << positive;
  out.identifyingString = ids.str();
  out.ownerGroup = this;
  if (!out.verifyRepresentation())
    global.fatal << global.fatal;
}

void HyperoctahedralGroup::allSpechtModules() {
  int N = this->GetN();
  for (int p = 0; p <= N; p++) {
    List<Partition> nps;
    Partition::GetPartitions(nps,p);
    for (int npi = 0; npi <nps.size; npi ++) {
      List<Partition> pps;
      Partition::GetPartitions(pps,N-p);
      for (int ppi = 0; ppi <pps.size; ppi ++) {
        GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroup>, Rational> sm;
        global.Comments << "Computing representation {" << nps[npi] << "}, {" << pps[ppi] << "}\n";
        this->spechtModuleOfPartititons(pps[ppi],nps[npi],sm);
        sm.verifyRepresentation();
        global.Comments << sm << '\n';
        this->irreps.addOnTop(sm);
      }
    }
  }
  this->irreps.quickSortAscending();
  global.Comments << this->prettyPrintCharacterTable() << '\n';
  representationDataIntoJS().writefile("representations_hyperoctahedral_group");
}*/
void HyperoctahedralGroupData::allSpechtModules() {
  for (int p = 0; p <= this->dimension; p ++) {
    List<Partition> nps;
    Partition::GetPartitions(nps, p);
    for (int npi = 0; npi < nps.size; npi ++) {
      List<Partition> pps;
      Partition::GetPartitions(pps, this->dimension - p);
      for (int ppi = 0; ppi < pps.size; ppi ++) {
        GroupRepresentation<
          FiniteGroup<ElementHyperoctahedralGroupR2>, Rational
        > sm;
        // global.Comments << "Computing representation {" << nps[npi] << "},
        // {"
        // << pps[ppi] << "}\n";
        this->spechtModuleOfPartititons(pps[ppi], nps[npi], sm);
        // sm.verifyRepresentation();
        this->group->addIrreducibleRepresentation(sm);
      }
    }
  }
  global.comments << this->group->prettyPrintCharacterTable() << '\n';
  // this->group->representationDataIntoJS().writefile("representations_hyperoctahedral_group");
}

/*
int HyperoctahedralGroup::GetN() {
  if (isEntireHyperoctahedralGroup)
    return this->N;
  this->N = 0;
  for (int i = 0; i < this->generators.size; i ++) {
    int gn = generators[i].SmallestN();
    if (gn > this->N)
      this->N = gn;
  }
  return this->N;
}
*/
LargeInteger HyperoctahedralGroupData::getSizeByFormulaImplementation(
  FiniteGroup<ElementHyperoctahedralGroupR2>& group
) {
  HyperoctahedralGroupData* hyperOctahedralData = static_cast<HyperoctahedralGroupData*>(group.specificDataPointer);
  // global.Comments << "HyperoctahedralGroup::getSize() called. N =" << HD->N
  // << '\n';
  if (!hyperOctahedralData) {
    global.fatal << "Consistency error. " << global.fatal;
  }
  if (hyperOctahedralData->flagIsEntireHyperoctahedralGroup) {
    return
    MathRoutines::factorial(hyperOctahedralData->dimension) *(
      1 << hyperOctahedralData->dimension
    );
  }
  if (hyperOctahedralData->flagIsEntireDn) {
    return
    MathRoutines::factorial(hyperOctahedralData->dimension) *(
      1 << hyperOctahedralData->dimension
    ) /
    2;
  }
  global.fatal << "This method should not have been called. " << global.fatal;
  // control reaches end of non-void function
  return - 1;
}

/*
bool HyperoctahedralGroup::getWordByFormulaImplementation(void* GG, const ElementHyperoctahedralGroup& g, List<int>& word) {
  HyperoctahedralGroup* G = (HyperoctahedralGroup*) GG;
  if (G->isEntireHyperoctahedralGroup) {
    g.p.getWordjjPlus1(word);
    for (int i = 0; i <g.s.size; i ++)
      if (g.s[i])
        word.addOnTop(G->N- 1+ i);
    return true;
  }
  global.fatal << "This method should not have been called " << __FILE__ << ":" << __LINE__ << global.fatal;
  return true;
}*/
bool HyperoctahedralGroupData::getWordByFormulaImplementation(
  FiniteGroup<HyperoctahedralBitsAutomorphism::ElementHyperoctahedralGroupR2>&
  group,
  const HyperoctahedralBitsAutomorphism::ElementHyperoctahedralGroupR2& element
  ,
  List<int>& word
) {
  HyperoctahedralGroupData* HD = static_cast<HyperoctahedralGroupData*>(group.specificDataPointer);
  if (HD->flagIsEntireHyperoctahedralGroup) {
    element.h.getWordjjPlus1(word);
    for (int i = 0; i < element.k.bits.size; i ++) {
      if (element.k.bits[i]) {
        word.addOnTop(HD->dimension - 1 + i);
      }
    }
    return true;
  }
  global.fatal << "This method should not have been called. " << global.fatal;
  return true;
}

/*
// number of conjugacy classes: partitions * #[0,N]
// representative of conjugacy classes: (a standard tableau, first k bits)
// number of elements of conjugacy classes: n!/(elements in row)/(same rows)! * (n choose k)
void HyperoctahedralGroup::computeCCSizesAndRepresentativesByFormulaImplementation(void* GG) {
  HyperoctahedralGroup* G = (HyperoctahedralGroup*) GG;
  if (!G->isEntireHyperoctahedralGroup)
    return G->computeConjugacyClassSizesAndRepresentatives(0);
  List<Partition> ccp;
  Partition::GetPartitions(ccp, G->N);
  int psize = ccp.size;
  int ssize = G->N+ 1;
  int nfac = 1;
  for (int i =2; i <= G->N; i ++)
    nfac *= i;
  G->conjugacyClasses.setSize(psize * ssize);
  for (int pi = 0; pi <psize; pi ++) {
    int p61z = ccp[pi].fulton61z();
    int snccsz = nfac / p61z;
    for (int si = 0; si <ssize; si ++) {
      Tableau t;
      ccp[pi].fillTableauOrdered(t);
      G->conjugacyClasses[pi*ssize + si].representative.p.makeFromTableauRows(t);
      G->conjugacyClasses[pi*ssize + si].representative.s.setSize(G->N);
      for (int i = 0; i <G->N; i ++)
        if (i <si)
          G->conjugacyClasses[pi*ssize + si].representative.s[i] = true;
        else
          G->conjugacyClasses[pi*ssize + si].representative.s[i] = false;
      G->conjugacyClasses[pi*ssize + si].size = snccsz;
      G->conjugacyClasses[pi*ssize +si].size *= MathRoutines::nChooseK(G->N, si);
      G->conjugacyClasses[pi*ssize +si].flagRepresentativeWordComputed = true;
      G->getWord(G->conjugacyClasses[pi*ssize +si].representative, G->conjugacyClasses[pi*ssize +si].representativeWord);
    }
  }
  G->flagCCsComputed = true;
  G->flagCCRepresentativesComputed = true;
  // VerifyFormulaCCRepresentatives
  //if (this->easyConjugacyDetermination)
  //  for (int i = 0; i < this->conjugacyClasses.size; i ++)
  //    for (int j = i + 1; j < this->conjugacyClasses.size; j ++)
  //      if (this->areConjugate(this->conjugacyClasses[i].representative, this->conjugacyClasses[j].representative))
  //        global.fatal << "Claimed conjugacy class representatives are actually conjugate " << __FILE__ << ":" << __LINE__ << global.fatal;

}
*/
/*
std::string HyperoctahedralGroup::toString() const {
  std::stringstream out;
  this->intoStream(out);
  return out.str();
}
*/
/*
template <>
bool FiniteGroup<ElementHyperoctahedralGroup>::areConjugate
(const ElementHyperoctahedralGroup& x, const ElementHyperoctahedralGroup& y) {
 return ElementHyperoctahedralGroup::areConjugate(x,y);}
*/
template < >
void ElementHyperoctahedralGroupR2::makeFromString(const std::string& in) {
  unsigned sep = static_cast<unsigned>(in.find_last_of(','));
  global.comments << in.substr(1, sep) << '\n';
  global.comments << in.substr(sep, in.size() - 1) << '\n';
  this->h.makeFromString(in.substr(1, sep));
  this->k.makeFromString(in.substr(sep, in.size() - 1));
}

std::string HyperoctahedralGroupData::toString() const {
  std::stringstream out;
  this->intoStream(out);
  return out.str();
}

std::ostream& operator<<(
  std::ostream& out, const HyperoctahedralGroupData& data
) {
  return data.intoStream(out);
}
