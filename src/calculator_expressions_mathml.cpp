#include "calculator_interface.h"
#include "math_extra_semisimple_lie_algebras.h"
#include "math_general_implementation.h" // IWYU pragma: keep: breaks g++ 'make optimize=1' build.
#include "math_general_polynomial_computations_basic_implementation.h" // IWYU pragma: keep: breaks g++ 'make optimize=1' build.

template < >
bool Expression::toMathMLBuiltIn<Rational>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  out << input.getValue<Rational>().toMathML(format, nullptr);
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<Polynomial<Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  out << input.getValue<Polynomial<Rational> >().toMathML(format, nullptr);
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<Weight<Polynomial<Rational> > >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  FormatExpressions formatLocal;
  input.getContext().getFormat(formatLocal);
  formatLocal.flagFormatWeightAsVectorSpaceIndex = false;
  out
  << input.getValue<Weight<Polynomial<Rational> > >().toMathML(
    &formatLocal, outputProperties
  );
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<Weight<Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  input.checkInitialization();
  (void) format;
  (void) outputProperties;
  out << input.owner->builtInName<Weight<Rational> >();
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<
  ElementUniversalEnveloping<RationalFraction<Rational> >
>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  FormatExpressions formatLocal;
  input.getContext().getFormat(formatLocal);
  out
  << "<mrow><mi>UEE{}</mi>"
  << MathML::leftParenthesis
  << input.getValue<ElementUniversalEnveloping<RationalFraction<Rational> > >()
  .toMathML(&formatLocal)
  << MathML::rightParenthesis
  << "</mrow>";
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<SemisimpleLieAlgebra*>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  STACK_TRACE("Expression::toStringBuiltIn(SemisimpleLieAlgebra*)");
  (void) format;
  (void) outputProperties;
  out
  << "<mi>SSLieAlg</mi><mo>{</mo><mo>}</mo>"
  << MathML::leftParenthesis
  << input.getValue<SemisimpleLieAlgebra*>()->toMathMLLieAlgebraName()
  << MathML::rightParenthesis;
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<RationalFraction<ElementZmodP> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  bool showContext = input.owner ==
  nullptr ? false : input.owner->flagDisplayContext;
  (void) format;
  (void) outputProperties;
  FormatExpressions formatLocal;
  input.getContext().getFormat(formatLocal);
  formatLocal.flagSuppressModP = true;
  formatLocal.flagUseFrac = true;
  const RationalFraction<ElementZmodP>& data =
  input.getValue<RationalFraction<ElementZmodP> >();
  if (showContext) {
    out << "<mrow>";
  }
  if (
    data.expressionType ==
    RationalFraction<ElementZmodP>::TypeExpression::typeRationalFraction
  ) {
    ElementZmodP constantSample =
    data.numerator.getElementConst().coefficients[0];
    out
    << "<mfrac>"
    << constantSample.toMathMLPolynomialCalculator(
      data.numerator.getElementConst(), &formatLocal
    )
    << constantSample.toMathMLPolynomialCalculator(
      data.denominator.getElementConst(), &formatLocal
    )
    << "</mfrac>";
  } else if (
    data.expressionType ==
    RationalFraction<ElementZmodP>::TypeExpression::typePolynomial
  ) {
    ElementZmodP constantSample =
    data.numerator.getElementConst().coefficients[0];
    out
    << constantSample.toMathMLPolynomialCalculator(
      data.numerator.getElementConst(), &formatLocal
    );
  } else {
    Polynomial<ElementZmodP> zero;
    out << data.constantValue.toMathMLPolynomialCalculator(zero, &formatLocal);
  }
  if (showContext) {
    out
    << "<mo>[</mo>"
    << input.getContext().toMathML()
    << "<mo>]</mo></mrow>";
  }
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<
  GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  FormatExpressions localFormat;
  const GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& element =
  input.getValue<GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational> >
  ();
  localFormat.flagUseLatex = true;
  localFormat.flagUseHTML = false;
  localFormat.flagUseReflectionNotation = true;
  out << element.toMathML(&localFormat);
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<
  VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  FormatExpressions formatLocal;
  input.getContext().getFormat(formatLocal);
  const VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>& element
  =
  input.getValue<
    VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
  >();
  formatLocal.flagUseLatex = true;
  formatLocal.flagUseHTML = false;
  formatLocal.flagUseReflectionNotation = true;
  out << element.toMathML(&formatLocal);
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<
  CharacterSemisimpleLieAlgebraModule<Rational>
>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  CharacterSemisimpleLieAlgebraModule<Rational> element =
  input.getValue<CharacterSemisimpleLieAlgebraModule<Rational> >();
  out << element.toMathML(format, outputProperties);
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<JSData>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  JSData data = input.getValue<JSData>();
  std::string dataString = data.toString(&JSData::PrintOptions::HTML());
  out << dataString;
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<VectorPartitionFunction>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  STACK_TRACE("Expression::toMathMLBuiltIn_VectorPartitionFunction");
  (void) format;
  (void) outputProperties;
  const VectorPartitionFunction& vectorPartitionFunction =
  input.getValue<VectorPartitionFunction>();
  out << vectorPartitionFunction.toHTML();
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<Lattice>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  STACK_TRACE("Expression::toMathMLBuiltIn_Lattice");
  (void) format;
  (void) outputProperties;
  const Lattice& lattice = input.getValue<Lattice>();
  out
  << "<mrow><mi>"
  << Calculator::Functions::Names::lattice
  << "{}</mi>"
  << lattice.toStringParentheses()
  << "</mrow>";
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<LittelmannPath>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  out << input.getValue<LittelmannPath>().toMathML();
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<
  GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational>
>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  out
  << input.getValue<
    GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational>
  >().toMathML();
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<ElementHyperoctahedralGroupR2>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  out << input.getValue<ElementHyperoctahedralGroupR2>().toMathML(format);
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<ElementEllipticCurve<Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  FormatExpressions formatEllipticCurve;
  input.getContext().getFormat(formatEllipticCurve);
  formatEllipticCurve.flagUseFrac = true;
  out
  << input.getValue<ElementEllipticCurve<Rational> >().toMathML(
    &formatEllipticCurve
  );
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<ElementEllipticCurve<ElementZmodP> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  FormatExpressions formatElementZModP;
  input.getContext().getFormat(formatElementZModP);
  formatElementZModP.flagUseFrac = true;
  out
  << input.getValue<ElementEllipticCurve<ElementZmodP> >().toMathML(
    &formatElementZModP
  );
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<InputBox>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  bool isFinal = format == nullptr ? true : format->flagExpressionIsTopLevel;
  if (!isFinal) {
    out << "<ms>(input box not shown)</ms>";
    return true;
  }
  if (isFinal) {
    out << input.getValueNonConst<InputBox>().name;
  }
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<Plot>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  bool isFinal = format == nullptr ? true : format->flagExpressionIsTopLevel;
  if (isFinal) {
    Plot& plot = input.getValueNonConst<Plot>();
    plot.flagIncludeExtraHtmlDescriptions = (format == nullptr) ?
    true :
    format->flagIncludeExtraHtmlDescriptionsInPlots;
    plot.flagPlotShowJavascriptOnly = input.owner->flagPlotShowJavascriptOnly;
    out << plot.getPlotHtml(*input.owner);
    if (input.owner->flagWriteLatexPlots) {
      out
      << input.owner->writeDefaultLatexFileReturnHtmlLink(
        plot.getPlotStringAddLatexCommands(false), nullptr, true
      );
      out
      << "<br><b>LaTeX code used to generate the output. </b><br>"
      << plot.getPlotStringAddLatexCommands(true);
    }
  } else {
    out << "<ms>(plot not shown)</ms>";
  }
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<Polynomial<ElementZmodP> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  bool showContext = input.owner ==
  nullptr ? false : input.owner->flagDisplayContext;
  FormatExpressions formatLocal;
  const Polynomial<ElementZmodP>& polynomial =
  input.getValue<Polynomial<ElementZmodP> >();
  input.getContext().getFormat(formatLocal);
  formatLocal.flagSuppressModP = true;
  formatLocal.flagUseFrac = true;
  if (!polynomial.isEqualToZero()) {
    out
    << polynomial.coefficients[0].toMathMLPolynomialCalculator(
      polynomial, &formatLocal
    );
  } else {
    out << "<mn>0</mn>";
  }
  if (showContext) {
    out << "<mo>[</mo>" << input.getContext().toMathML() << "<mo>]</mo>";
  }
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<ElementZmodP>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  out << input.getValue<ElementZmodP>().toMathML(format, outputProperties);
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<double>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  std::string currentString =
  FloatingPoint::doubleToString(input.getValue<double>());
  bool useColor = input.owner ==
  nullptr ? false : input.owner->flagUseNumberColors;
  if (useColor) {
    out << "<mn style='color:blue'>";
  } else {
    out << "<mn>";
  }
  out << currentString;
  out << "</mn>";
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<RationalFraction<Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  bool showContext = input.owner ==
  nullptr ? false : input.owner->flagDisplayContext;
  (void) format;
  (void) outputProperties;
  FormatExpressions formatLocal;
  input.getContext().getFormat(formatLocal);
  formatLocal.flagUseFrac = true;
  if (showContext) {
    out << "<mrow>";
  }
  out
  << "<mi>MakeRationalFunction{}</mi>"
  << MathML::leftParenthesis
  << input.getValue<RationalFraction<Rational> >().toMathML(&formatLocal)
  << MathML::rightParenthesis;
  if (showContext) {
    out << "<mo>[</mo>" << input.getContext().toMathML() << "</mo></mrow>";
  }
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<RationalFraction<AlgebraicNumber> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  bool showContext = input.owner ==
  nullptr ? false : input.owner->flagDisplayContext;
  (void) format;
  (void) outputProperties;
  FormatExpressions formatLocal;
  input.getContext().getFormat(formatLocal);
  formatLocal.flagUseFrac = true;
  if (showContext) {
    out << "<mrow>";
  }
  out
  << "<mi>MakeRationalFunction{}</mi>"
  << MathML::leftParenthesis
  << input.getValue<RationalFraction<AlgebraicNumber> >().toMathML(
    &formatLocal
  )
  << MathML::rightParenthesis;
  if (showContext) {
    out << "<mo>[</mo>" << input.getContext().toMathML() << "<mo>]</mo>";
    out << "</mrow>";
  }
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<Polynomial<AlgebraicNumber> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  FormatExpressions formatLocal;
  input.getContext().getFormat(formatLocal);
  bool showContext = input.owner ==
  nullptr ? false : input.owner->flagDisplayContext;
  formatLocal.flagUseFrac = true;
  out << "PolynomialAlgebraicNumbers{}(";
  std::string currentString =
  input.getValue<Polynomial<AlgebraicNumber> >().toString(&formatLocal);
  if (currentString.size() > 0) {
    if (currentString[0] == '-') {
      currentString = currentString.substr(1);
      out << "-";
    }
  }
  if (input.owner->flagUseNumberColors) {
    out << "\\color{red}{";
  }
  out << currentString;
  if (input.owner->flagUseNumberColors) {
    out << "}";
  }
  out << ")";
  if (showContext) {
    out << "[" << input.getContext().toString() << "]";
  }
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<ElementWeylAlgebra<Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  FormatExpressions formatLocal;
  bool showContext = input.owner ==
  nullptr ? false : input.owner->flagDisplayContext;
  input.getContext().getFormat(formatLocal);
  formatLocal.flagUseHTML = false;
  formatLocal.flagUseLatex = true;
  out << "<mrow>";
  out << "<mi>ElementWeylAlgebra{}</mi>" << MathML::leftParenthesis;
  out
  << input.getValue<ElementWeylAlgebra<Rational> >().toMathML(&formatLocal);
  out << MathML::rightParenthesis;
  if (showContext) {
    out << "<mo>[</mo>" << input.getContext().toMathML() << "<mo>]</mo>";
  }
  out << "</mrow>";
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<PolynomialModuloPolynomial<ElementZmodP> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  FormatExpressions formatLocal;
  input.getContext().getFormat(formatLocal);
  formatLocal.flagUseFrac = true;
  const PolynomialModuloPolynomial<ElementZmodP>& element =
  input.getValue<PolynomialModuloPolynomial<ElementZmodP> >();
  ElementZmodP sample;
  formatLocal.flagSuppressModP = true;
  if (!element.modulusContainer.isEqualToZero()) {
    sample = element.modulusContainer.coefficients[0];
    out
    << "<mrow>"
    << sample.toMathMLPolynomialCalculator(element.value, &formatLocal)
    << "<mo>mod</mo>"
    << sample.toMathMLPolynomialCalculator(
      element.modulusContainer, &formatLocal
    )
    << "</mrow>";
  } else {
    out
    << "<mrow>"
    << element.value.toMathML(&formatLocal)
    << " mod "
    << element.modulusContainer.toMathML(&formatLocal)
    << "</mrow>";
  }
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<MatrixTensor<Rational> >(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  FormatExpressions formatLocal;
  input.getContext().getFormat(formatLocal);
  formatLocal.flagUseLatex = true;
  formatLocal.flagUseHTML = false;
  if (
    input.getValue<MatrixTensor<Rational> >().
    getMinimumNumberOfColumnsNumberOfRows() <
    20
  ) {
    out
    << "<mi>MatrixRationalsTensorForm{}</mi>"
    << MathML::leftParenthesis
    << input.getValue<MatrixTensor<Rational> >().toMathMLMatrixForm(
      &formatLocal, outputProperties
    )
    << MathML::rightParenthesis;
  } else {
    out
    << input.getValue<MatrixTensor<Rational> >().toMathML(
      format, outputProperties
    );
  }
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<
  ElementTensorsGeneralizedVermas<RationalFraction<Rational> >
>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  FormatExpressions localFormat;
  input.getContext().getFormat(localFormat);
  out << "<mrow>";
  out
  << "<mi>"
  << input.owner->builtInName<
    ElementTensorsGeneralizedVermas<RationalFraction<Rational> >
  >()
  << "{}</mi>"
  << MathML::leftParenthesis;
  out
  << input.getValue<
    ElementTensorsGeneralizedVermas<RationalFraction<Rational> >
  >().toMathML(&localFormat);
  out << MathML::rightParenthesis;
  bool showContext = input.owner ==
  nullptr ? false : input.owner->flagDisplayContext;
  if (showContext) {
    out << "<mo>[</mo>" << input.getContext().toMathML() << "<mo>]</mo>";
  }
  out << "</mrow>";
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<std::string>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  bool useQuotes = format == nullptr ? false : format->flagUseQuotes;
  bool isFinal = format == nullptr ? true : format->flagExpressionIsTopLevel;
  if (!useQuotes) {
    if (isFinal) {
      out
      << StringRoutines::Conversions::stringToCalculatorDisplay(
        input.getValue<std::string>()
      );
    } else {
      out << "<ms>" << input.getValue<std::string>() << "</ms>";
    }
  } else {
    out
    << "<ms>\""
    << StringRoutines::Conversions::escapeJavascriptLike(
      input.getValue<std::string>()
    )
    << "\"</ms>";
  }
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<AlgebraicNumber>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  FormatExpressions formatLocal;
  if (input.owner->flagUseFracInRationalLaTeX) {
    formatLocal.flagUseFrac = true;
  }
  MathExpressionProperties properties;
  std::string currentString =
  input.getValue<AlgebraicNumber>().toMathML(&formatLocal, &properties);
  bool useColors = input.owner ==
  nullptr ? false : input.owner->flagUseNumberColors;
  if (useColors) {
    out << "<mrow style='color:red'>";
  } else {
    out << "</mrow>";
  }
  out << currentString;
  if (useColors) {
    out << "</mrow>";
  }
  bool showContext = input.owner ==
  nullptr ? false : input.owner->flagDisplayContext;
  if (showContext) {
    out << "<mo>[</mo>" << input.getContext().toMathML() << "<mo>]</mo>";
  }
  if (outputProperties != nullptr) {
    *outputProperties = properties;
  }
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<WeylGroupData>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  FormatExpressions formatLocal;
  WeylGroupData& group = input.getValueNonConst<WeylGroupData>();
  formatLocal.flagUseLatex = true;
  formatLocal.flagUseHTML = false;
  formatLocal.flagUseReflectionNotation = true;
  out << group.toMathML(&formatLocal);
  return true;
}

template < >
bool Expression::toMathMLBuiltIn<ElementWeylGroup>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  FormatExpressions localFormat;
  const ElementWeylGroup& element = input.getValue<ElementWeylGroup>();
  localFormat.flagUseLatex = true;
  localFormat.flagUseHTML = false;
  localFormat.flagUseReflectionNotation = true;
  out << element.toMathML(&localFormat);
  return true;
}

std::string ElementZmodP::toMathMLPolynomialCalculator(
  const Polynomial<ElementZmodP>& input, FormatExpressions* format
) const {
  std::stringstream out;
  out << "<mrow>";
  out << "<mi>PolynomialModP{}</mi>" << MathML::leftParenthesis;
  out << input.toMathML(format);
  out
  << "<mo>,</mo><mn>"
  << this->modulus
  << "</mn>"
  << MathML::rightParenthesis;
  out << "</mrow>";
  return out.str();
}

bool Expression::toMathMLFactorial(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opFactorial(), 2)) {
    return false;
  }
  out << "<mrow>";
  if (input[1].needsParenthesisForBaseOfExponent()) {
    out
    << MathML::leftParenthesis
    << input[1].toMathML(format)
    << MathML::rightParenthesis;
  } else {
    out << input[1].toMathML(format);
  }
  out << "<mo>!</mo>" << "</mrow>";
  return true;
}

bool Expression::toMathMLSqrt(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opSqrt())) {
    return false;
  }
  if (input.size() == 2) {
    return Expression::toMathMLSqrt2(input, out, format);
  }
  return Expression::toMathMLSqrt3(input, out, format);
}

bool Expression::toMathMLSqrt2(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opSqrt(), 2)) {
    return false;
  }
  out << "<msqrt>" << input[1].toMathML(format) << "</msqrt>";
  return true;
}

bool Expression::toMathMLSqrt3(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opSqrt(), 3)) {
    return false;
  }
  int power = 0;
  bool hasPowerTwo = input[1].isSmallInteger(&power);
  if (hasPowerTwo) {
    hasPowerTwo = (power == 2);
  }
  if (hasPowerTwo) {
    out << "<msqrt>" << input[2].toMathML(format) << "</msqrt>";
  } else {
    out
    << "<mroot>"
    << input[2].toMathML(format)
    << input[1].toMathML(format)
    << "</mroot>";
  }
  return true;
}

bool Expression::toMathMLDefine(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opDefine(), 3)) {
    return false;
  }
  std::string firstE = input[1].toMathML(format);
  std::string secondE = input[2].toMathML(format);
  out << "<mrow>";
  if (
    input[1].isListStartingWithAtom(input.owner->opDefine()) ||
    input[1].isListStartingWithAtom(input.owner->opGreaterThan()) ||
    input[1].isListStartingWithAtom(input.owner->opGreaterThanOrEqualTo()) ||
    input[1].isListStartingWithAtom(input.owner->opLessThan()) ||
    input[1].isListStartingWithAtom(input.owner->opLessThanOrEqualTo())
  ) {
    out << MathML::leftParenthesis << firstE << MathML::rightParenthesis;
  } else {
    out << firstE;
  }
  out << "<mo>=</mo>";
  if (input[2].isListStartingWithAtom(input.owner->opDefine())) {
    out << MathML::leftParenthesis << secondE << MathML::rightParenthesis;
  } else {
    out << secondE;
  }
  out << "</mrow>";
  return true;
}

void Expression::initializeToMathMLHandlers(Calculator& toBeInitialized) {
  STACK_TRACE("Expression::initializeToMathMLHandlers");
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opDefine(), Expression::toMathMLDefine
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opIsDenotedBy(), Expression::toStringIsDenotedBy
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opLog(), Expression::toStringLnAbsoluteInsteadOfLogarithm
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opLogBase(), Expression::toStringLogBase
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opIntervalOpen(), Expression::toStringIntervalOpen
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opIntervalRightClosed(),
    Expression::toStringIntervalRightClosed
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opIntervalLeftClosed(),
    Expression::toStringIntervalLeftClosed
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opIntervalClosed(), Expression::toStringIntervalClosed
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opQuote(), Expression::toStringQuote
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opDefineConditional(),
    Expression::toStringDefineConditional
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opDivide(), Expression::toMathMLDivide
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opTensor(), Expression::toMathMLTensor
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opIn(), Expression::toStringIn
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opOr(), Expression::toStringOr
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opAnd(), Expression::toStringAnd
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opBinom(), Expression::toStringBinom
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opUnderscore(), Expression::toMathMLUnderscore
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opSetMinus(), Expression::toStringSetMinus
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opLimitBoundary(), Expression::toStringLimitBoundary
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opTimes(), Expression::toMathMLTimes
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opCrossProduct(), Expression::toStringCrossProduct
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opSqrt(), Expression::toMathMLSqrt
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opFactorial(), Expression::toMathMLFactorial
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opAbsoluteValue(), Expression::toStringAbsoluteValue
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opPower(), Expression::toMathMLPower
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opPlus(), Expression::toMathMLPlus
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opDirectSum(), Expression::toStringDirectSum
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opMinus(), Expression::toMathMLMinus
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opBind(), Expression::toStringBind
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opEqualEqual(), Expression::toStringEqualEqual
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opEqualEqualEqual(), Expression::toStringEqualEqualEqual
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opDifferentiate(), Expression::toStringDifferentiate
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opDifferential(), Expression::toStringDifferential
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opSum(), Expression::toStringSumOrIntegral
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opIntegral(), Expression::toStringSumOrIntegral
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opGreaterThan(), Expression::toStringGreaterThan
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opGreaterThanOrEqualTo(),
    Expression::toStringGreaterThanOrEqualTo
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opLessThanOrEqualTo(),
    Expression::toStringLessThanOrEqualTo
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opLimit(), Expression::toStringLimit
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opLimitProcess(), Expression::toStringLimitProcess
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opLessThan(), Expression::toStringLessThan
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opSequence(), Expression::toMathMLSequence
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opLieBracket(), Expression::toStringLieBracket
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opMod(), Expression::toStringMod
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opUnion(), Expression::toStringUnion
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opIntersection(), Expression::toStringIntersection
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opUnionNoRepetition(),
    Expression::toStringUnionNoRepetition
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opError(), Expression::toStringError
  );
  toBeInitialized.addOneMathMLCompositeHandler(
    toBeInitialized.opMatrix(), Expression::toMathMLMatrix
  );
}

std::string Expression::toMathMLFinal(FormatExpressions* format) const {
  return
  MathML::toMathMLFinal(
    this->toMathML(format), this->toString(format), format
  );
}

void Expression::toMathML(
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties,
  Expression* startingExpression,
  bool unfoldCommandEnclosures,
  JSData* outputJS
) const {
  STACK_TRACE("Expression::toMathML");
  RecursionDepthCounter recursionCounter;
  recursionCounter.initialize(&this->owner->recursionDepth);
  this->checkConsistency();
  if (this->owner != nullptr) {
    if (this->owner->recursionDepth + 1 > this->owner->maximumRecursionDepth) {
      out << "<ms>(...)</ms>";
      return;
    }
  } else {
    out << "<ms>(Error:NoOwner)</ms>";
    return;
  }
  MemorySaving<FormatExpressions> formatContainer;
  if (format == nullptr) {
    format = &formatContainer.getElement();
    format->flagUseQuotes = false;
  }
  if (startingExpression != nullptr && unfoldCommandEnclosures) {
    Expression newStart;
    Expression newMe;
    if (
      CalculatorBasics::functionFlattenCommandEnclosuresOneLayer(
        *this->owner, *this, newMe
      ) &&
      CalculatorBasics::functionFlattenCommandEnclosuresOneLayer(
        *this->owner, *startingExpression, newStart
      )
    ) {
      newMe.toMathML(
        out, format, outputProperties, &newStart, false, outputJS
      );
      return;
    }
  }
  if (
    !this->isOfType<std::string>() &&
    !this->startsWith(this->owner->opCommandSequence())
  ) {
    if (startingExpression == nullptr) {
      format->flagUseQuotes = true;
    } else {
      format->flagUseQuotes = false;
    }
  }
  if (outputJS != nullptr) {
    outputJS->reset();
  }
  int notationIndex =
  owner->objectContainer.expressionWithNotation.getIndex(*this);
  if (notationIndex != - 1) {
    out << owner->objectContainer.expressionNotation[notationIndex];
    return;
  }
  if (this->toMathMLData(out, format, outputProperties)) {
    return;
  } else if (this->toMathMLWithAtomHandler(out, outputProperties)) {
    return;
  } else if (this->toMathMLWithCompositeHandler(out, outputProperties)) {} else
  if (this->toMathMLEndStatement(out, startingExpression, outputJS, format)) {
    return;
  } else if (this->size() == 1) {
    (*this)[0].toMathML(
      out, format, outputProperties, nullptr, false, nullptr
    );
  } else if (this->toMathMLGeneral(out, format, outputProperties)) {} else {
    out << "<ms>(ProgrammingError:NotDocumented)</ms>";
  }
}

bool Expression::toMathMLEndStatement(
  std::stringstream& out,
  Expression* startingExpression,
  JSData* outputJS,
  FormatExpressions* format
) const {
  STACK_TRACE("Expression::toMathMLEndStatement");
  if (!this->isListStartingWithAtom(this->owner->opCommandSequence())) {
    return false;
  }
  if (startingExpression != nullptr) {
    return
    this->toMathMLEndStatementTopLevel(
      out, *startingExpression, outputJS, format
    );
  }
  return this->toMathMLEndStatementNested(out, format);
}

bool Expression::toMathMLEndStatementTopLevel(
  std::stringstream& out,
  Expression& startingExpression,
  JSData* outputJS,
  FormatExpressions* format
) const {
  STACK_TRACE("Expression::toMathMLEndStatementTopLevel");
  MemorySaving<FormatExpressions> temporaryFormat;
  if (format == nullptr) {
    format = &temporaryFormat.getElement();
    format->flagExpressionIsTopLevel = true;
  }
  bool isFinal = format->flagExpressionIsTopLevel;
  bool createTable = true;
  bool createSingleTable = true;
  if (
    createTable == false &&
    format != nullptr &&
    global.runMode != GlobalVariables::RunMode::consoleRegular
  ) {}
  if (!createSingleTable && !createTable && this->size() > 2) {
    out << "(";
  }
  if (createSingleTable) {
    out << "<table class='tableCalculatorOutput'>";
  }
  std::string currentOutput;
  if (outputJS != nullptr) {
    (*outputJS)["input"].elementType = JSData::Type::tokenArray;
    (*outputJS)["output"].elementType = JSData::Type::tokenArray;
  }
  for (int i = 1; i < this->size(); i ++) {
    const Expression expression = (*this)[i];
    this->toMathMLEndStatementOneRowTopLevel(
      out, &startingExpression, outputJS, i, *format
    );
    if (format != nullptr) {
      format->flagExpressionIsTopLevel = isFinal;
    }
  }
  if (createSingleTable) {
    out << "</table>";
  }
  if (!createSingleTable && !createTable && this->size() > 2) {
    out << ")";
  }
  return true;
}

bool Expression::toMathMLEndStatementOneRowTopLevel(
  std::stringstream& out,
  Expression* startingExpression,
  JSData* outputJS,
  int index,
  FormatExpressions& format
) const {
  STACK_TRACE("Expression::toMathMLEndStatementOneRowTopLevel");
  std::string currentInput, currentOutput;
  const Expression currentE = (*this)[index];
  out << "<tr><td class='cellCalculatorInput'>";
  if (!this->owner->flagHideLHS) {
    if (index < (*startingExpression).size()) {
      format.flagDontCollalpseProductsByUnits = true;
      currentInput = (*startingExpression)[index].toMathMLFinal(&format);
    } else {
      currentInput =
      "No matching starting expression - "
      "possible use of the Melt keyword.";
    }
  } else {
    currentInput = "...";
  }
  out << currentInput;
  if (outputJS != nullptr) {
    (*outputJS)["input"][index - 1] = currentInput;
  }
  if (index != this->size() - 1) {
    out << ";";
  }
  out << "</td><td class='cellCalculatorResult'>";
  if (currentE.isOfType<std::string>()) {
    currentOutput =
    StringRoutines::Conversions::stringToCalculatorDisplay(
      currentE.getValue<std::string>()
    );
  } else if (currentE.requiresNoMathTags()) {
    format.flagDontCollalpseProductsByUnits = false;
    currentOutput = currentE.toString(&format);
  } else {
    format.flagDontCollalpseProductsByUnits = false;
    std::string childString = currentE.toString(&format);
    if (StringRoutines::stringContains(childString, "\\(")) {
      // The string contains the math tag \(. We assume the childString
      // has embedded descriptive latex strings. We should not generate math
      // tags; the expression is either too complicated to be formatted
      // correctly,
      // or the childString has already got all it's tags.
      currentOutput = childString;
    } else {
      currentOutput = currentE.toMathMLFinal(&format);
    }
  }
  currentOutput += currentE.toStringAllSlidersInExpression();
  if (outputJS != nullptr) {
    (*outputJS)["output"][index - 1] = currentOutput;
  }
  out << currentOutput;
  out << "</td></tr>";
  return true;
}

bool Expression::toMathMLEndStatementNested(
  std::stringstream& out, FormatExpressions* format
) const {
  STACK_TRACE("Expression::toMathMLEndStatementNested");
  out << "<mtable>";
  for (int i = 1; i < this->size(); i ++) {
    out << "<mtr><mtd>";
    (*this)[i].toMathML(format, nullptr, nullptr, false, nullptr);
    out << "</mtd></mtr>";
  }
  out << "</mtable>";
  return true;
}

bool Expression::toMathMLData(
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) const {
  STACK_TRACE("Expression::toMathMLData");
  if (this->owner == nullptr) {
    out << "<ms>(non-initialized)</ms>";
    return true;
  }
  if (this->isAtom()) {
    if (this->isOperationGiven(this->owner->opDifferential())) {
      out << "<mtext>d</mtext>";
    } else if (this->isOperationGiven(this->owner->opPhantom())) {
      out << "";
    } else if (
      this->owner->flagUseLnInsteadOfLog &&
      this->isOperationGiven(this->owner->opLog())
    ) {
      out << "<ln/>";
    } else if (
      this->data < this->owner->getOperations().size && this->data >= 0
    ) {
      std::string latex = this->owner->getOperations()[this->data];
      out << MathML::latexCommandToMathMLEquivalent(latex);
    } else {
      out << "[unknown atom of value " << this->data << "]";
    }
    return true;
  }
  if (this->isMatrixOfType<RationalFraction<Rational> >()) {
    FormatExpressions currentFormat;
    this->getContext().getFormat(currentFormat);
    currentFormat.flagUseHTML = false;
    currentFormat.flagUseLatex = true;
    Matrix<RationalFraction<Rational> > matrix;
    CalculatorConversions::functionGetMatrix(
      *this->owner, *this, matrix, false
    );
    out << matrix.toMathML(&currentFormat);
    return true;
  }
  int typeIndex = - 1;
  if (!this->isBuiltInType(&typeIndex)) {
    return false;
  }
  Calculator& commands = *this->owner;
  // The following handlers are initialized by
  // calling Calculator::addBuiltInType
  // in Calculator::initialize.
  if (!commands.toMathMLDataHandlers.contains(typeIndex)) {
    return false;
  }
  // handler must hold the function pointer:
  // Expression::toStringBuiltIn<builtInType>,
  // where builtInType is one of the types registered in
  // Calculator::initializeToStringHandlers.
  Expression::ToStringHandler handler =
  commands.toMathMLDataHandlers.getValueNoFail(typeIndex);
  return handler(*this, out, format, outputProperties);
}

bool Expression::toMathMLGeneral(
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) const {
  if (this->size() < 2) {
    return false;
  }
  out << "<mrow>";
  (*this)[0].toMathML(out, format, outputProperties, nullptr, false, nullptr);
  bool needParenthesis = true;
  if (this->size() == 2) {
    if ((*this)[0].isAtomWhoseExponentsAreInterpretedAsFunction()) {
      needParenthesis = !(*this)[1].isAtom();
    }
    if ((*this)[0].isPowerOfAtomWhoseExponentsAreInterpretedAsFunction()) {
      needParenthesis = !(*this)[1].isAtom();
    }
  }
  if (needParenthesis) {
    out << MathML::leftParenthesis;
  }
  for (int i = 1; i < this->children.size; i ++) {
    (*this)[i].toMathML(
      out, format, outputProperties, nullptr, false, nullptr
    );
    if (i != this->children.size - 1) {
      out << "<mo>,</mo>";
    }
  }
  if (needParenthesis) {
    out << MathML::rightParenthesis;
  }
  out << "</mrow>";
  return true;
}

bool Expression::toMathMLSequence(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  if (!input.isListStartingWithAtom(input.owner->opSequence())) {
    return false;
  }
  (void) format;
  out << "<mrow>" << MathML::leftParenthesis;
  for (int i = 1; i < input.size(); i ++) {
    std::string currentChildString =
    input[i].toMathML(format, outputProperties);
    out << currentChildString;
    if (i != input.children.size - 1) {
      out << "<mo>,</mo>";
    }
  }
  out << MathML::rightParenthesis;
  if (input.size() == 2) {
    out << "}";
  }
  return true;
}

bool Expression::toMathMLPower(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  Calculator& commands = *input.owner;
  if (!input.startsWith(commands.opPower(), 3)) {
    return false;
  }
  bool involvesExponentsInterpretedAsFunctions = false;
  const Expression& firstE = input[1];
  const Expression& secondE = input[2];
  bool isSuperScriptOfUnderscoredOperator = false;
  if (firstE.startsWith(commands.opUnderscore(), 3)) {
    if (firstE[1].isOperationGiven(commands.opIntegral())) {
      isSuperScriptOfUnderscoredOperator = true;
    }
  }
  if (isSuperScriptOfUnderscoredOperator) {
    out << "<msubsup>";
    firstE[1].toMathML(out, format, outputProperties);
    firstE[2].toMathML(out, format, outputProperties);
    secondE.toMathML(out, format, outputProperties);
    return true;
  }
  if (firstE.startsWith(- 1, 2)) {
    bool shouldProceed =
    firstE[0].isAtomWhoseExponentsAreInterpretedAsFunction() &&
    !secondE.isEqualToMOne() &&
    secondE.isRational();
    if (
      shouldProceed &&
      firstE[0].isOperationGiven(commands.opLog()) &&
      commands.flagUseLnAbsInsteadOfLogForIntegrationNotation
    ) {
      shouldProceed = false;
    }
    if (shouldProceed) {
      involvesExponentsInterpretedAsFunctions = true;
      Expression newFunE;
      newFunE.makeXOX(*input.owner, commands.opPower(), firstE[0], input[2]);
      newFunE.checkConsistency();
      out << "<mrow>";
      newFunE.toMathML(out, format, outputProperties);
      if (
        firstE[1].needsParenthesisForMultiplicationWhenSittingOnTheRightMost()
        ||
        firstE[1].startsWith(commands.opTimes())
      ) {
        out << MathML::leftParenthesis;
        firstE[1].toMathML(out, format, outputProperties);
        out << MathML::rightParenthesis;
      } else {
        firstE[1].toMathML(out, format, outputProperties);
      }
    }
  }
  if (!involvesExponentsInterpretedAsFunctions) {
    bool isSqrt = false;
    if (input[2].isOfType<Rational>()) {
      if (input[2].getValue<Rational>().isEqualTo(Rational(1, 2))) {
        isSqrt = true;
      }
    }
    if (isSqrt) {
      out << "<msqrt>";
      input[1].toMathML(out, format, outputProperties);
      out << "</msqrt>";
    } else {
      out << "<msup>";
      bool needsParentheses = input[1].needsParenthesisForBaseOfExponent();
      if (needsParentheses) {
        out << MathML::leftParenthesis;
      }
      input[1].toMathML(out, format, outputProperties);
      if (needsParentheses) {
        out << MathML::rightParenthesis;
      }
      input[2].toMathML(out, format, outputProperties);
      out << "</msup>";
    }
  }
  return true;
}

bool Expression::toMathMLUnderscore(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opUnderscore())) {
    return false;
  }
  if (!input[1].startsWith(input.owner->opPower())) {
    out << "<msub>";
    input[1].toMathML(out, format, nullptr);
    input[2].toMathML(out, format, nullptr);
    out << "</msub>";
    return true;
  }
  // We have an expression along the lines of {x^2}_1.
  // In latex, this would render with the subscript
  // being to the right of the superscript.
  // We shall render that as the latex of x^2_1 which
  // looks identical as the latex of x_1^2 and has
  // the super- and sub- scripts with the same indentation levels.
  out << "<msubsup>";
  // The base of the exponent:
  input[1][1].toMathML(out, format, nullptr);
  // The subscript:
  input[2].toMathML(out, format, nullptr);
  // The power of the exponent:
  input[1][2].toMathML(out, format, nullptr);
  out << "</msubsup>";
  return true;
}

bool Expression::toMathMLTimes(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opTimes(), 3)) {
    return false;
  }
  MathExpressionProperties secondExpressionProperties;
  MathExpressionProperties firstExpressionProperties;
  std::string secondExpressionMathML =
  input[2].toMathML(format, &secondExpressionProperties);
  if (input[1].isOperationGiven(input.owner->opSqrt())) {
    // A malformed expression such as: "\sqrt 3" will be parsed as "sqrt * 3"
    // and later corrected to "\sqrt{3}".
    out << "<msqrt>" << secondExpressionMathML << "</msqrt>";
    return true;
  }
  std::string firstExpression =
  input[1].toMathML(format, &firstExpressionProperties);
  bool firstNeedsBrackets = input[1].needsParenthesisForMultiplication();
  bool secondNeedsBrackets =
  input[2].needsParenthesisForMultiplicationWhenSittingOnTheRightMost(
    &(input[1])
  );
  if (secondExpressionProperties.startsWithMinus) {
    secondNeedsBrackets = true;
  }
  bool mustHaveTimes = false;
  bool collapseUnits = true;
  if (format != nullptr) {
    collapseUnits = !format->flagDontCollalpseProductsByUnits;
  }
  if (collapseUnits) {
    if (firstExpressionProperties.isNegativeOne) {
      firstExpression = MathML::negativeSign;
      firstNeedsBrackets = false;
    }
    if (firstExpressionProperties.isOne) {
      firstExpression = "";
    }
  }
  bool needsRow = !firstExpressionProperties.isOne &&
  !secondExpressionProperties.isOne;
  if (needsRow) {
    out << "<mrow>";
  }
  if (firstNeedsBrackets) {
    out
    << MathML::leftParenthesis
    << firstExpression
    << MathML::rightParenthesis;
  } else {
    out << firstExpression;
  }
  if (
    !firstNeedsBrackets &&
    !secondNeedsBrackets &&
    firstExpression != "" &&
    firstExpressionProperties.isNegativeOne
  ) {
    if (secondExpressionProperties.startsWithDigit) {
      mustHaveTimes = true;
    }
    if (firstExpressionProperties.endsWithDigit) {
      if (secondExpressionProperties.startsWithFraction) {
        mustHaveTimes = true;
      }
    }
  }
  if (mustHaveTimes) {
    out << "<mo>&sdot;</mo>";
  }
  if (secondNeedsBrackets) {
    out
    << MathML::leftParenthesis
    << secondExpressionMathML
    << MathML::rightParenthesis;
  } else {
    out << secondExpressionMathML;
  }
  if (needsRow) {
    out << "</mrow>";
  }
  return true;
}

bool Expression::toMathMLPlus(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opPlus())) {
    return false;
  }
  if (input.size() < 3) {
    global.fatal
    << "Plus operation takes at least 2 arguments, "
    << "whereas this expression has "
    << input.children.size - 1
    << " arguments. "
    << global.fatal;
  }
  input.checkInitialization();
  List<Expression> summands;
  input.owner->collectOpands(input, input.owner->opPlus(), summands);
  out << "<mrow>";
  for (int i = 0; i < summands.size; i ++) {
    const Expression& summand = summands[i];
    bool needsParenthesesForAddition = summand.needsParenthesisForAddition();
    MathExpressionProperties properties;
    std::string summandString = summand.toMathML(format, &properties);
    if (i > 0) {
      if (needsParenthesesForAddition || !properties.startsWithMinus) {
        out << "<mo>+</mo>";
      }
    }
    if (needsParenthesesForAddition) {
      out << MathML::leftParenthesis;
    }
    out << summandString;
    if (needsParenthesesForAddition) {
      out << MathML::rightParenthesis;
    }
  }
  out << "</mrow>";
  return true;
}

bool Expression::toMathMLMatrix(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.isMatrix()) {
    return false;
  }
  out << "<mrow>";
  out << MathML::leftParenthesis;
  out << "<mtable>";
  for (int i = 1; i < input.size(); i ++) {
    out << "<mtr>";
    for (int j = 1; j < input[i].size(); j ++) {
      out << "<mtd>" << input[i][j].toMathML(format, nullptr) << "</mtd>";
    }
    out << "</mtr>";
  }
  out << "</mtable>";
  out << MathML::rightParenthesis;
  out << "</mrow>";
  return true;
}

bool Expression::toMathMLDivide(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  Calculator& commands = *input.owner;
  if (!input.startsWith(commands.opDivide(), 3)) {
    return false;
  }
  bool doUseFrac = commands.flagUseFracInRationalLaTeX;
  if (
    doUseFrac && (
      input[1].startsWith(commands.opTimes()) ||
      input[1].startsWith(commands.opDivide())
    )
  ) {
    List<Expression> multiplicandsLeft;
    input.getMultiplicandsDivisorsRecursive(multiplicandsLeft, 0);
    for (int i = 0; i < multiplicandsLeft.size; i ++) {
      if (
        multiplicandsLeft[i].startsWith(commands.opIntegral()) ||
        multiplicandsLeft[i].isOperationGiven(commands.opIntegral())
      ) {
        doUseFrac = false;
        break;
      }
    }
  }
  if (!doUseFrac) {
    std::string firstMathML = input[1].toMathML(format);
    std::string secondMathML = input[2].toMathML(format);
    bool firstNeedsBrackets =
    !(
      input[1].isListStartingWithAtom(commands.opTimes()) ||
      input[1].isListStartingWithAtom(commands.opDivide())
    );
    bool secondNeedsBrackets = true;
    if (input[2].isOfType<Rational>()) {
      if (input[2].getValue<Rational>().isInteger()) {
        secondNeedsBrackets = false;
      }
    }
    if (firstNeedsBrackets) {
      out
      << MathML::leftParenthesis
      << firstMathML
      << MathML::rightParenthesis;
    } else {
      out << firstMathML;
    }
    out << "<mo>/</mo>";
    if (secondNeedsBrackets) {
      out
      << MathML::leftParenthesis
      << secondMathML
      << MathML::rightParenthesis;
    } else {
      out << secondMathML;
    }
  } else {
    StateMaintainer<bool> maintain;
    if (format != nullptr) {
      maintain.initialize(format->flagExpressionNewLineAllowed);
      format->flagExpressionNewLineAllowed = false;
    } else {
      // Compilers will complain at certain optimization levels about
      // maintain not being initialized.
      maintain.toMaintain = nullptr;
      maintain.contentAtStart = false;
    }
    std::string firstMathML = input[1].toMathML(format);
    std::string secondMathML = input[2].toMathML(format);
    out << "<mfrac>" << firstMathML << secondMathML << "</mfrac>";
  }
  return true;
}

bool Expression::toMathMLMinus(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opMinus())) {
    return false;
  }
  if (input.size() == 2) {
    return Expression::toMathMLMinus2(input, out, format);
  }
  return Expression::toMathMLMinus3(input, out, format);
}

bool Expression::toMathMLMinus3(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opMinus(), 3)) {
    return false;
  }
  if (input.children.size != 3) {
    global.fatal
    << "The minus function expects 1 or 2 arguments, "
    << "instead there are "
    << input.children.size - 1
    << ". "
    << global.fatal;
  }
  out << input[1].toMathML(format) << MathML::negativeSign;
  if (
    input[2].startsWith(input.owner->opPlus()) ||
    input[2].startsWith(input.owner->opMinus())
  ) {
    out
    << MathML::leftParenthesis
    << input[2].toMathML(format)
    << MathML::rightParenthesis;
  } else {
    out << input[2].toMathML(format);
  }
  return true;
}

bool Expression::toMathMLMinus2(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opMinus(), 2)) {
    return false;
  }
  if (
    input[1].startsWith(input.owner->opPlus()) ||
    input[1].startsWith(input.owner->opMinus())
  ) {
    out
    << MathML::negativeSign
    << MathML::leftParenthesis
    << input[1].toMathML(format)
    << MathML::rightParenthesis;
  } else {
    out << MathML::negativeSign << input[1].toMathML(format);
  }
  return true;
}

bool Expression::toMathMLTensor(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opTensor(), 3)) {
    return false;
  }
  input.toMathMLOpMultiplicative(out, "<mo>&otimes;</mo>", format);
  return true;
}

void Expression::toMathMLOpMultiplicative(
  std::stringstream& out,
  const std::string& operation,
  FormatExpressions* format
) const {
  MathExpressionProperties propertiesOfFirst;
  MathExpressionProperties propertiesOfSecond;
  std::string firstMathML = (*this)[1].toMathML(format, &propertiesOfFirst);
  std::string secondMathML = (*this)[2].toMathML(format, &propertiesOfSecond);
  bool firstNeedsBrackets = (*this)[1].needsParenthesisForMultiplication();
  bool secondNeedsBrackets = (*this)[2].needsParenthesisForMultiplication();
  if (propertiesOfFirst.isNegativeOne) {
    firstMathML = MathML::negativeSign;
    firstNeedsBrackets = false;
  }
  if (propertiesOfFirst.isOne) {
    firstMathML = "";
    firstNeedsBrackets = false;
  }
  if (firstNeedsBrackets) {
    out << MathML::leftParenthesis << firstMathML << MathML::rightParenthesis;
  } else {
    out << firstMathML;
  }
  out << operation;
  if (secondNeedsBrackets) {
    out << MathML::leftParenthesis << secondMathML << MathML::rightParenthesis;
  } else {
    out << secondMathML;
  }
}

std::string ExpressionContext::toMathML() const {
  std::stringstream out;
  // this->checkInitialization();
  out << "Context. ";
  if (this->variables.size > 0) {
    out << "Variables: " << this->variables.toStringCommaDelimited() << ". ";
  }
  if (this->differentialOperatorVariables.size > 0) {
    out
    << "Differential operators: "
    << this->variables.toStringCommaDelimited()
    << ". ";
  }
  if (this->indexAmbientSemisimpleLieAlgebra != - 1) {
    out
    << "Ambient semisimple Lie algebra: "
    << this->getAmbientSemisimpleLieAlgebra()->weylGroup.dynkinType.toString();
  }
  return out.str();
}
