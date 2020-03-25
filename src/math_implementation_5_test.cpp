// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general_polynomial_computations_basic_implementation.h"

bool MonomialP::Test::All() {
  return MonomialP::Test::TestMonomialOrdersSatisfyTheDefinition();
}

bool MonomialP::Test::TestMonomialOrdersSatisfyTheDefinitionOne(
  const MonomialP& mustBeSmaller,
  const MonomialP& mustBeLarger,
  List<MonomialP>::Comparator& order
) {
  if (!order.leftGreaterThanRight(mustBeLarger, mustBeSmaller)) {
    global.fatal << "Monomial: "
    << mustBeLarger.ToString()
    << " is not larger than "
    << mustBeSmaller.ToString()
    << " when using monomial: " << order.name
    << global.fatal;
  }
  return true;
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
  List<List<MonomialP>::Comparator> allOrders;
  allOrders.AddOnTop(MonomialP::greaterThan_leftLargerWins);
  allOrders.LastObject()->name = "lexicographic";
  allOrders.AddOnTop(MonomialP::greaterThan_rightLargerWins);
  allOrders.LastObject()->name = "lexicographic opposite";
  allOrders.AddOnTop(MonomialP::greaterThan_totalDegree_leftLargerWins);
  allOrders.LastObject()->name = "graded lexicographic";
  allOrders.AddOnTop(MonomialP::greaterThan_totalDegree_rightSmallerWins);
  allOrders.LastObject()->name = "graded reverse lexicographic";
  List<List<MonomialP> > elementsIncreasingOrder ({
    {one, xOne, xOneSquared},
    {one, xTwo, xTwoSquared},
    {one, xOne, xOneXtwo},
    {one, xTwo, xOneXtwo}
  });
  for (int i = 0; i < allOrders.size; i ++) {
    for (int j = 0; j < elementsIncreasingOrder.size; j ++) {
      for (int k = 0; k < elementsIncreasingOrder[i].size; k ++) {
        for (int l = k + 1; l < elementsIncreasingOrder[i].size; l ++) {
          MonomialP::Test::TestMonomialOrdersSatisfyTheDefinitionOne(
            elementsIncreasingOrder[j][k], elementsIncreasingOrder[j][l], allOrders[i]
          );
        }
      }
    }
  }
  return true;
}

