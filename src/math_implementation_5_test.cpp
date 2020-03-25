// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general_polynomial_computations_basic_implementation.h"

bool MonomialP::Test::All() {
  return MonomialP::Test::TestMonomialOrdersSatisfyTheDefinition();
}

bool MonomialP::Test::TestMonomialOrdersSatisfyTheDefinition() {
  MonomialP xOne, xTwo, xOneSquared, xTwoSquared, xOneXtwo, one;
  one.MakeOne();
  xOne.MakeEi(0, 1);
  xTwo.MakeEi(1, 1);
  xOneSquared.MakeEi(0, 2);
  xTwoSquared.MakeEi(1, 2);
  xOneXtwo = xOne;
  xOneXtwo *= xTwo;
  List<bool (*)(const MonomialP&, const MonomialP&)> comparisons;
  comparisons.AddOnTop(MonomialP::greaterThan_leftLargerWins);
  comparisons.AddOnTop(MonomialP::greaterThan_rightLargerWins);
  comparisons.AddOnTop(MonomialP::greaterThan_totalDegree_leftLargerWins);
  comparisons.AddOnTop(MonomialP::greaterThan_totalDegree_rightSmallerWins);
  List<List<MonomialP>::Comparator> allOrders;
  allOrders.SetSize(comparisons.size);
  for (int i = 0; i < comparisons.size; i ++) {
    allOrders[i].setComparison(comparisons[i]);
  }
  List<List<MonomialP> > elementsIncreasingOrder ({
    {one, xOne, xOneSquared},
  });



  return true;
}

