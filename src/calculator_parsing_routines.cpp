#include  "calculator_interface.h"
#include <iostream>

List<SyntacticElement::Role> CalculatorParser::singleCharacterSyntacticRoles;
MapList<std::string, SyntacticElement::Role> CalculatorParser::
keyWordsToSyntacticRoles;
MapList<std::string, std::string> CalculatorParser::keyWordAutocorrections;
HashedList<std::string> CalculatorParser::keyWordsWithDigits;

bool SyntacticElement::endsOnWhitespace() const {
  int size = static_cast<int>(this->source.size());
  const HashedList<std::string> whiteSpaces =
  CalculatorParser::whitespaceContainer();
  for (int i = 0; i < 3; i ++) {
    if (i >= size) {
      return false;
    }
    std::string end = this->source.substr(size - i - 1);
    if (whiteSpaces.contains(end)) {
      return true;
    }
  }
  return false;
}

bool SyntacticElement::isCommandEnclosure() const {
  Calculator* owner = this->data.owner;
  if (this->data.owner == nullptr) {
    return false;
  }
  if (
    this->syntacticRole != SyntacticElement::EXPRESSION &&
    this->syntacticRole != SyntacticElement::SEQUENCE_STATEMENTS
  ) {
    return false;
  }
  if (this->data.startsWith(owner->opCommandEnclosure())) {
    return true;
  }
  if (
    this->data.startsWith(owner->opCommandSequence()) && this->data.size() >= 2
  ) {
    if (this->data[1].startsWith(owner->opCommandEnclosure())) {
      return true;
    }
  }
  return false;
}

std::string SyntacticElement::toStringDebug() const {
  return this->toStringSyntaxRole();
}

std::string SyntacticElement::syntaxRoleToString(SyntacticElement::Role role) {
  switch (role) {
  case LOGARITHM:
    return "LOGARITHM";
  case SyntacticElement::ERROR:
    return "ERROR";
  case TO:
    return "TO";
  case SyntacticElement::EQUALS_COLON:
    return "EQUALS_COLON";
  case SyntacticElement::EQUALS_EQUALS_EQUALS:
    return "EQUALS_EQUALS_EQUALS";
  case SyntacticElement::FACTORIAL:
    return "FACTORIAL";
  case SyntacticElement::INTEGRAL_WITH_SUBSCRIPT:
    return "INTEGRAL_WITH_SUBSCRIPT";
  case SyntacticElement::INTEGRAL_OPERATOR:
    return "INTEGRAL_OPERATOR";
  case SyntacticElement::INTEGRAL_WITH_SUPERSCRIPT:
    return "INTEGRAL_WITH_SUPERSCRIPT";
  case SyntacticElement::INTEGRAL_SIGN:
    return "INTEGRAL_SIGN";
  case SyntacticElement::UPPER_CARET:
    return "UPPER_CARET";
  case SyntacticElement::DOLLAR:
    return "DOLLAR";
  case SyntacticElement::SEQUENCE:
    return "SEQUENCE";
  case SyntacticElement::LEFT_RIGHT_CURLY_BRACE:
    return "LEFT_RIGHT_CURLY_BRACE";
  case SyntacticElement::CIRC:
    return "CIRC";
  case SyntacticElement::DIGITS:
    return "DIGITS";
  case SyntacticElement::DIGITS_RAW:
    return "DIGITS_RAW";
  case SyntacticElement::LETTERS:
    return "LETTERS";
  case SyntacticElement::QUOTE_UNDER_CONSTRUCTION:
    return "QUOTE_UNDER_CONSTRUCTION";
  case SyntacticElement::RAW:
    return "RAW";
  case SyntacticElement::UNKNOWN:
    return "UNKNOWN";
  case SyntacticElement::VARIABLE:
    return "VARIABLE";
  case SyntacticElement::START_FILLER:
    return "START_FILLER";
  case SyntacticElement::END_PROGRAM:
    return "END_PROGRAM";
  case OPLUS:
    return "OPLUS";
  case SET_MINUS:
    return "SET_MINUS";
  case SyntacticElement::QUOTE:
    return "QUOTE";
  case SyntacticElement::BACKSLASH:
    return "BACKSLASH";
  case SyntacticElement::PERCENT:
    return "PERCENT";
  case SyntacticElement::WHITE_SPACE:
    return "WHITE_SPACE";
  case SyntacticElement::TILDE:
    return "TILDE";
  case SyntacticElement::EQUALS:
    return "EQUALS";
  case SyntacticElement::EXPRESSION:
    return "EXPRESSION";
  case SyntacticElement::COLON:
    return "COLON";
  case SyntacticElement::IF:
    return "IF";
  case SyntacticElement::LEFT_PARENTHESIS:
    return "LEFT_PARENTHESES";
  case SyntacticElement::RIGHT_PARENTHESIS:
    return "RIGHT_PARENTHESIS";
  case SyntacticElement::LEFT_BRACKET:
    return "LEFT_BRACKET";
  case SyntacticElement::RIGHT_BRACKET:
    return "RIGHT_BRACKET";
  case SyntacticElement::LEFT_CURLY_BRACE:
    return "LEFT_CURLY_BRACE";
  case SyntacticElement::RIGHT_CURLY_BRACE:
    return "RIGHT_CURLY_BRACE";
  case SyntacticElement::DOUBLE_BACKSLASH:
    return "DOUBLE_BACKSLASH";
  case SyntacticElement::DOT:
    return "DOT";
  case SyntacticElement::AMPERSAND:
    return "AMPERSAND";
  case SyntacticElement::BEGIN:
    return "BEGIN";
  case SyntacticElement::END:
    return "END";
  case SyntacticElement::FRAC:
    return "FRAC";
  case SyntacticElement::MATRIX_ENVIRONMENT:
    return "MATRIX_ENVIRONMENT";
  case SyntacticElement::ARRAY_ENVIRONMENT:
    return "ARRAY_ENVIRONMENT";
  case SyntacticElement::MATRIX_UNDER_CONSTRUCTION:
    return "MATRIX_UNDER_CONSTRUCTION";
  case SyntacticElement::MATRIX_END:
    return "MATRIX_END";
  case SyntacticElement::SEMICOLON:
    return "SEMICOLON";
  case SyntacticElement::FONT_MODIFIER:
    return "FONT_MODIFIER";
  case SyntacticElement::CHOOSE:
    return "CHOOSE";
  case SyntacticElement::COMMA:
    return "COMMA";
  case SyntacticElement::PLUS:
    return "PLUS";
  case SyntacticElement::MINUS:
    return "MINUS";
  case SyntacticElement::AND:
    return "AND";
  case SyntacticElement::OR:
    return "OR";
  case SEQUENCE_STATEMENTS:
    return "SEQUENCE_STATEMENTS";
  case LEFT_SEPARATOR:
    return "LEFT_SEPARATOR";
  case RIGHT_SEPARATOR:
    return "RIGHT_SEPARATOR";
  case SyntacticElement::EQUALS_EQUALS:
    return "EQUALS_EQUALS";
  case SyntacticElement::LESS_THAN_LIKE:
    return "LESS_THAN_LIKE";
  case SyntacticElement::GREATER_THAN_LIKE:
    return "GREATER_THAN";
  case SyntacticElement::PIPE:
    return "PIPE";
  case UNDERSCORE:
    return "UNDERSCORE";
  case STAR:
    return "STAR";
  case DIVISION_SIGN:
    return "DIVISION_SIGN";
  case CUP:
    return "CUP";
  case CAP:
    return "CAP";
  case TENSOR_PRODUCT:
    return "TENSOR_PRODUCT";
  case SQRT:
    return "SQRT";
  case SQ_CUP:
    return "SQ_CUP";
  case MAKE_SEQUENCE:
    return "MAKE_SEQUENCE";
  case CDOT:
    return "CDOT";
  case IN:
    return "IN";
  case SyntacticElement::TIMES_X_SIGN:
    return "TIMES_X_SIGN";
  case SyntacticElement::LOGARITHM_BASE:
    return "LOGARITH_BASE";
  case TEXT:
    return "TEXT";
  case MOD:
    return "MOD";
  case BINOM:
    return "BINOM";
  case LIM:
    return "LIM";
  }
  return "SHOULD_BE_UNREACHABLE";
}

std::string SyntacticElement::toStringSyntaxRole() const {
  std::stringstream out;
  if (!this->source.empty()) {
    out << this->source;
  }
  out << "[";
  out << this->syntaxRoleToString(this->syntacticRole);
  out << "]";
  return out.str();
}

void CalculatorParser::initializeSingleCharacterSyntacticRoles() {
  singleCharacterSyntacticRoles.setSize(128);
  for (int i = 0; i < 128; i ++) {
    singleCharacterSyntacticRoles[i] = SyntacticElement::RAW;
  }
  registerSyntacticRole('"', SyntacticElement::QUOTE);
  registerSyntacticRole('%', SyntacticElement::PERCENT);
  registerSyntacticRole('\n', SyntacticElement::WHITE_SPACE);
  registerSyntacticRole('\r', SyntacticElement::WHITE_SPACE);
  registerSyntacticRole('\t', SyntacticElement::WHITE_SPACE);
  registerSyntacticRole('\v', SyntacticElement::WHITE_SPACE);
  registerSyntacticRole('\f', SyntacticElement::WHITE_SPACE);
  registerSyntacticRole(' ', SyntacticElement::WHITE_SPACE);
  registerSyntacticRole('~', SyntacticElement::TILDE);
  registerSyntacticRole('_', SyntacticElement::UNDERSCORE);
  registerSyntacticRole('=', SyntacticElement::EQUALS);
  registerSyntacticRole(':', SyntacticElement::COLON);
  registerSyntacticRole(';', SyntacticElement::SEMICOLON);
  registerSyntacticRole('.', SyntacticElement::DOT);
  registerSyntacticRole('\\', SyntacticElement::BACKSLASH);
  registerSyntacticRole('(', SyntacticElement::LEFT_PARENTHESIS);
  registerSyntacticRole(')', SyntacticElement::RIGHT_PARENTHESIS);
  registerSyntacticRole('[', SyntacticElement::LEFT_BRACKET);
  registerSyntacticRole(']', SyntacticElement::RIGHT_BRACKET);
  registerSyntacticRole('{', SyntacticElement::LEFT_CURLY_BRACE);
  registerSyntacticRole('}', SyntacticElement::RIGHT_CURLY_BRACE);
  registerSyntacticRole('&', SyntacticElement::AMPERSAND);
  registerSyntacticRole('+', SyntacticElement::PLUS);
  registerSyntacticRole('*', SyntacticElement::STAR);
  registerSyntacticRole('-', SyntacticElement::MINUS);
  registerSyntacticRole(',', SyntacticElement::COMMA);
  registerSyntacticRole('|', SyntacticElement::PIPE);
  registerSyntacticRole('/', SyntacticElement::DIVISION_SIGN);
  registerSyntacticRole('>', SyntacticElement::GREATER_THAN_LIKE);
  registerSyntacticRole('!', SyntacticElement::FACTORIAL);
  registerSyntacticRole('<', SyntacticElement::LESS_THAN_LIKE);
  registerSyntacticRole('^', SyntacticElement::UPPER_CARET);
}

void CalculatorParser::initializeKeyWordsToSyntacticRoles() {
  registerKeyword("if", SyntacticElement::IF);
  registerKeyword("and", SyntacticElement::AND);
  registerKeyword("or", SyntacticElement::OR);
  registerKeyword("\\begin", SyntacticElement::BEGIN);
  registerKeyword("\\end", SyntacticElement::END);
  registerKeyword("\\frac", SyntacticElement::FRAC);
  registerKeyword("\\choose", SyntacticElement::CHOOSE);
  registerKeyword("matrix", SyntacticElement::MATRIX_ENVIRONMENT);
  registerKeyword("pmatrix", SyntacticElement::MATRIX_ENVIRONMENT);
  registerKeyword("bmatrix", SyntacticElement::MATRIX_ENVIRONMENT);
  registerKeyword("array", SyntacticElement::ARRAY_ENVIRONMENT);
  registerKeyword("\\mathbf", SyntacticElement::FONT_MODIFIER);
  registerKeyword("\\mathfrak", SyntacticElement::FONT_MODIFIER);
  registerKeyword("\\mathcal", SyntacticElement::FONT_MODIFIER);
  registerKeyword("\\mathbb", SyntacticElement::FONT_MODIFIER);
  registerKeyword("\\left", SyntacticElement::LEFT_SEPARATOR);
  registerKeyword("\\right", SyntacticElement::RIGHT_SEPARATOR);
  registerKeyword("\u00F7", SyntacticElement::DIVISION_SIGN);
  registerKeyword("\\to", SyntacticElement::TO);
  registerKeyword("\\leq", SyntacticElement::LESS_THAN_LIKE);
  registerKeyword("\\displaystyle", SyntacticElement::WHITE_SPACE);
  registerKeyword("\\geq", SyntacticElement::GREATER_THAN_LIKE);
  registerKeyword("\\circ", SyntacticElement::CIRC);
  registerKeyword("\\int", SyntacticElement::INTEGRAL_SIGN);
  registerKeyword("\\cup", SyntacticElement::CUP);
  registerKeyword("\\sqcup", SyntacticElement::SQ_CUP);
  registerKeyword("\\cap", SyntacticElement::CAP);
  registerKeyword("\\sqrt", SyntacticElement::SQRT);
  registerKeyword("\\cdot", SyntacticElement::CDOT);
  registerKeyword("\\otimes", SyntacticElement::TENSOR_PRODUCT);
  registerKeyword("\\oplus", SyntacticElement::OPLUS);
  registerKeyword("\\times", SyntacticElement::TIMES_X_SIGN);
  registerKeyword("MakeSequence", SyntacticElement::MAKE_SEQUENCE);
  registerKeyword("Sequence", SyntacticElement::MAKE_SEQUENCE);
  registerKeyword("\\text", SyntacticElement::TEXT);
  registerKeyword("\\in", SyntacticElement::IN);
  registerKeyword("\\mod", SyntacticElement::MOD);
  registerKeyword("\\setminus", SyntacticElement::SET_MINUS);
  registerKeyword("\\binom", SyntacticElement::BINOM);
  registerKeyword("\\lim", SyntacticElement::LIM);
  registerKeyword("\\log", SyntacticElement::LOGARITHM);
  keyWordAutocorrections.setKeyValue("sqrt", "\\sqrt");
  keyWordAutocorrections.setKeyValue("ln", "\\log");
  keyWordAutocorrections.setKeyValue("\\ln", "\\log");
  keyWordAutocorrections.setKeyValue("log", "\\log");
  keyWordAutocorrections.setKeyValue("infinity", "\\infty");
  keyWordAutocorrections.setKeyValue("infty", "\\infty");
  keyWordAutocorrections.setKeyValue("pi", "\\pi");
  keyWordAutocorrections.setKeyValue("arcsin", "\\arcsin");
  keyWordAutocorrections.setKeyValue("arccos", "\\arccos");
  keyWordAutocorrections.setKeyValue("arctan", "\\arctan");
  keyWordAutocorrections.setKeyValue("sin", "\\sin");
  keyWordAutocorrections.setKeyValue("cos", "\\cos");
  keyWordAutocorrections.setKeyValue("tan", "\\tan");
  keyWordAutocorrections.setKeyValue("cot", "\\cot");
  keyWordAutocorrections.setKeyValue("csc", "\\csc");
  keyWordAutocorrections.setKeyValue("sec", "\\sec");
  keyWordAutocorrections.setKeyValue("choose", "\\choose");
  keyWordAutocorrections.setKeyValue("sqrt", "\\sqrt");
  keyWordAutocorrections.setKeyValue("mod", "\\mod");
  keyWordsWithDigits.addOnTop("Plot2D");
  keyWordsWithDigits.addOnTop("ToUTF8String");
  keyWordsWithDigits.addOnTop("Plot2DWithBars");
  keyWordsWithDigits.addOnTop("ConvertBase64ToHex");
  keyWordsWithDigits.addOnTop("ConvertBase58ToHex");
  keyWordsWithDigits.addOnTop("AppendDoubleSha256Check");
  keyWordsWithDigits.addOnTop("ConvertBase64ToString");
  keyWordsWithDigits.addOnTop("Base64ToHex");
  keyWordsWithDigits.addOnTop("TestASN1Decode");
  keyWordsWithDigits.addOnTop("X509CertificateDecode");
  keyWordsWithDigits.addOnTop("Sha256Verbose");
  keyWordsWithDigits.addOnTop("HmmG2inB3");
  keyWordsWithDigits.addOnTop("HmmG2inB");
  keyWordsWithDigits.addOnTop("SplitFDpartB3overG");
  keyWordsWithDigits.addOnTop("SplitFDpartB3overG2");
  keyWordsWithDigits.addOnTop("SplitFDpartB3overG2CharsOnly");
  keyWordsWithDigits.addOnTop("PrintB3G");
  keyWordsWithDigits.addOnTop("PrintB3G2");
  keyWordsWithDigits.addOnTop("PrintB3G2branchingTable");
  keyWordsWithDigits.addOnTop("PrintB3G2branchingTableCharsOnly");
}

std::string SyntacticElement::toStringHumanReadable(
  bool includeLispifiedExpressions
) const {
  std::string controlString = this->toStringSyntaxRole();
  bool makeTable = false;
  if (
    this->syntacticRole == SyntacticElement::EXPRESSION ||
    this->syntacticRole == SyntacticElement::ERROR ||
    this->syntacticRole == SyntacticElement::SEQUENCE ||
    this->syntacticRole == SyntacticElement::SEQUENCE_STATEMENTS ||
    this->syntacticRole == SyntacticElement::VARIABLE ||
    this->syntacticRole == SyntacticElement::INTEGRAL_WITH_SUPERSCRIPT ||
    this->syntacticRole == SyntacticElement::INTEGRAL_WITH_SUBSCRIPT ||
    this->syntacticRole == SyntacticElement::INTEGRAL_OPERATOR
  ) {
    makeTable = true;
  }
  if (!makeTable) {
    return controlString;
  }
  std::stringstream out;
  out << "<table style = 'vertical-align:top;border-spacing:0px 0px'>";
  out
  << "<tr><td style = 'text-align:center'>"
  << this->data.toString(nullptr)
  << "</td></tr>";
  out << "<tr><td style = 'color:#AAAAAA'>" << controlString << "</td></tr>";
  if (includeLispifiedExpressions) {
    out
    << "<tr><td style ='color:#AAAAAA'>"
    << this->data.toStringFull()
    << "</td></tr>";
  }
  if (this->errorString != "") {
    out << "<tr><td>" << this->errorString << "</td></tr>";
  }
  out << "</table>";
  return out.str();
}

void CalculatorParser::rationalFromIntegerDotInteger(
  const std::string& beforeDecimalPoint,
  const std::string& afterDecimalPoint,
  Expression& output
) {
  int powerOfTenToDivideBy = static_cast<int>(afterDecimalPoint.size());
  Rational denominator = 10;
  denominator.raiseToPower(powerOfTenToDivideBy);
  Rational afterDecimalPointInteger;
  afterDecimalPointInteger.assignString(afterDecimalPoint);
  Rational result;
  result.assignString(beforeDecimalPoint);
  result *= denominator;
  result += afterDecimalPointInteger;
  result /= denominator;
  output.assignValue(*this->owner, result);
}

void CalculatorParser::initialize(Calculator* inputOwner) {
  this->owner = inputOwner;
  this->addFlagDescription("LogParsing", &this->flagLogSyntaxRules);
  this->addFlagDescription("LogFull", &this->owner->flagLogFullTreeCrunching);
  this->addFlagDescription(
    "NumberColors",
    &this->owner->flagUseNumberColors,
    "<span style='color:blue'>Floating point numbers</span> "
    "are displayed in "
    "<span style='color:blue'>blue</span>."
    "<br><span style='color:red'>Algebraic numbers</span> "
    "are displayed in <span style='color:red'>red</span>. "
    "<br>Rational numbers are displayed in default color.<br>"
  );
  this->addFlagDescription(
    "PlotShowJavascriptOnly", &this->owner->flagPlotShowJavascriptOnly
  );
  this->addFlagDescription("PlotDetails", &this->owner->flagPlotNoControls);
  this->addFlagDescription(
    "UseMathML",
    &this->owner->flagUseMathML,
    "<b style='color:red'>WORK IN PROGRESS.</b> "
    "Using mathML for the output.<br>"
  );
  this->addFlagDescription(
    "UseBracketForIntervals",
    &this->owner->flagUseBracketsForIntervals,
    "Using brackets for intervals. "
  );
  this->addFlagDescription(
    "DontUsePredefinedWordSplits",
    &this->owner->flagDontUsePredefinedWordSplits,
    "Predefined word splits are OFF. xy will *not* be replaced by x*y. "
  );
  this->addFlagDescription(
    "WriteLatexDetails", &this->owner->flagWriteLatexPlots
  );
  this->addFlagDescription(
    "HidePolynomialDataStructure",
    &this->owner->flagHidePolynomialBuiltInTypeIndicator
  );
  this->addFlagDescription("LogEvaluation", &this->owner->flagLogEvaluation);
  this->addFlagDescription(
    "UseLnInsteadOfLog",
    &this->owner->flagUseLnInsteadOfLog,
    "Displaying complex logarithms as ln. "
  );
  this->addFlagDescription(
    "UseLnAbsInsteadOfLog",
    &this->owner->flagUseLnAbsInsteadOfLogForIntegrationNotation
  );
  this->addFlagDescription("LogCache", &this->owner->flagLogCache);
  this->addFlagDescription("LogRules", &this->owner->flagLogRules);
  this->addFlagDescription("ShowContext", &this->owner->flagDisplayContext);
  this->addFlagDescription("SkipEvaluation", &this->owner->flagSkipEvaluation);
  this->addFlagDescription(
    "FullTree", &this->owner->flagDisplayFullExpressionTree
  );
  this->addFlagDescription("HideLHS", &this->owner->flagHideLHS);
  this->addFlagDescription("NoFrac", &this->owner->flagDontUseFracInRational);
}

void CalculatorParser::addFlagDescription(
  const std::string& keyWord,
  bool* inputFlagLocation,
  const std::string& inputDescription
) {
  CalculatorParser::FlagDescription flagDescription;
  flagDescription.flagLocation = inputFlagLocation;
  flagDescription.description = inputDescription;
  this->modifyableFlags.setKeyValue(keyWord, flagDescription);
}

void CalculatorParser::reset() {
  this->numberOfEmptyTokensStart = 9;
  this->flagLogSyntaxRules = false;
  this->flagEncounteredSplit = false;
  this->toBeParsedContainer.setSize(0);
  this->syntacticStackContainer.setSize(0);
  this->syntaxErrors = "";
  this->syntacticStack = &this->syntacticStackContainer;
  this->toBeParsed = &this->toBeParsedContainer;
  this->stringsThatSplitIfFollowedByDigit.clear();
}

bool CalculatorParser::decreaseStackSetCharacterRanges(int decrease) {
  if (decrease <= 0) {
    return true;
  }
  if ((*this->syntacticStack).size - decrease <= 0) {
    global.fatal << "Bad stack decrease. " << global.fatal;
  }
  (*this->syntacticStack).setSize((*this->syntacticStack).size - decrease);
  return true;
}

bool CalculatorParser::parse(
  const std::string& input, bool stripCommandSequence, Expression& output
) {
  if (
    !this->parseAndExtractExpressions(
      input,
      output,
      this->toBeParsedContainer,
      this->syntacticStackContainer,
      nullptr
    )
  ) {
    return false;
  }
  if (stripCommandSequence) {
    if (output.startsWith(this->owner->opCommandSequence(), 2)) {
      output = output[1];
    }
  }
  return true;
}

void CalculatorParser::initializeStatic() {
  if (CalculatorParser::singleCharacterSyntacticRoles.size > 0) {
    return;
  }
  CalculatorParser::initializeSingleCharacterSyntacticRoles();
  CalculatorParser::whitespaceContainer();
  CalculatorParser::initializeKeyWordsToSyntacticRoles();
}

void CalculatorParser::registerSyntacticRole(
  char character, SyntacticElement::Role role
) {
  int index = static_cast<int>(static_cast<unsigned char>(character));
  CalculatorParser::singleCharacterSyntacticRoles[index] = role;
}

void CalculatorParser::registerKeyword(
  const std::string& keyWord, SyntacticElement::Role role
) {
  if (keyWordsToSyntacticRoles.contains(keyWord)) {
    global.fatal
    << "Not allowed to add calculator parser keyword twice. "
    << global.fatal;
  }
  keyWordsToSyntacticRoles.setKeyValue(keyWord, role);
}

bool CalculatorParser::parseAndExtractExpressionsDefault(
  const std::string& input, Expression& output
) {
  return
  this->parseAndExtractExpressions(
    input,
    output,
    this->toBeParsedContainer,
    this->syntacticStackContainer,
    &this->syntaxErrors
  );
}

bool CalculatorParser::parseAndExtractExpressions(
  const std::string& input,
  Expression& output,
  List<SyntacticElement>& outputAllSyntacticElements,
  List<SyntacticElement>& outputSyntacticStack,
  std::string* outputSyntacticErrors
) {
  STACK_TRACE("CalculatorParser::parseAndExtractExpressions");
  this->syntacticStack = &outputSyntacticStack;
  this->toBeParsed = &outputAllSyntacticElements;
  this->extractSyntacticElementsDefaultOutput(input);
  bool result =
  this->extractExpressionsFromPreprocessed(output, outputSyntacticErrors);
  this->syntacticStack = &this->syntacticStackContainer;
  this->toBeParsed = &this->toBeParsedContainer;
  return result;
}

std::string CalculatorParser::toStringSyntacticStackHTMLSimple() {
  STACK_TRACE("CalculatorParser::toStringSyntacticStackHTMLSimple");
  if (this->syntacticStack->size == 0) {
    global.fatal << "Unexpected empty syntactic stack." << global.fatal;
  }
  std::stringstream out;
  bool isBad = ((*this->syntacticStack).size >
    this->numberOfEmptyTokensStart + 1
  );
  SyntacticElement& lastSyntacticElement =
  *(*this->syntacticStack).lastObject();
  if ((*this->syntacticStack).size == this->numberOfEmptyTokensStart + 1) {
    if (lastSyntacticElement.syntacticRole != SyntacticElement::EXPRESSION) {
      isBad = true;
    }
  }
  if (!isBad) {
    out << this->syntacticStack->lastObject()->data.toString();
    return out.str();
  }
  for (
    int i = this->numberOfEmptyTokensStart; i < (*this->syntacticStack).size;
    i ++
  ) {
    SyntacticElement& currentElement = (*this->syntacticStack)[i];
    if (currentElement.isCommandEnclosure()) {
      continue;
    }
    if (currentElement.syntacticRole != SyntacticElement::EXPRESSION) {
      out
      << " <b style='color:red'>"
      << currentElement.toStringSyntaxRole()
      << "</b> ";
    } else {
      out << currentElement.data.toString();
    }
  }
  return out.str();
}

void CalculatorParser::initializeStringsThatSplitIfFollowedByDigit() {
  STACK_TRACE(
    "CalculatorParser::initializeStringsThatSplitIfFollowedByDigit"
  );
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew(
    "\\cdot"
  );
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew(
    "\\circ"
  );
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew(
    "\\frac"
  );
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew(
    "\\ln"
  );
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew(
    "\\log"
  );
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew(
    "\\sin"
  );
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew(
    "\\cos"
  );
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew(
    "\\tan"
  );
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew(
    "\\cot"
  );
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew(
    "\\sec"
  );
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew(
    "\\csc"
  );
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew(
    "\\arctan"
  );
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew(
    "\\arcsin"
  );
  this->stringsThatSplitIfFollowedByDigit.addOnTopNoRepetitionMustBeNew(
    "\\arccos"
  );
}

SyntacticElement CalculatorParser::getStartFillerElement() {
  SyntacticElement result;
  result.syntacticRole = SyntacticElement::START_FILLER;
  result.data.reset(*this->owner);
  return result;
}

Calculator::EvaluationStatistics::EvaluationStatistics() {
  this->reset();
}

void Calculator::EvaluationStatistics::reset() {
  this->expressionsEvaluated = 0;
  this->maximumCallsBeforeReportGeneration = 5000;
  this->report.freeMemory();
  this->totalSubstitutions = 0;
  this->totalEvaluationLoops = 0;
  this->totalPatternMatchesPerformed = 0;
  this->totalEvaluationLoops = 0;
  this->numberOfListsStart = - 1;
  this->numberListResizesStart = - 1;
  this->numberHashResizesStart = - 1;
  this->numberOfSmallAdditionsStart = - 1;
  this->numberOfSmallMultiplicationsStart = - 1;
  this->numberOfSmallGreatestCommonDivisorsStart = - 1;
  this->numberOfLargeAdditionsStart = - 1;
  this->numberOfLargeMultiplicationsStart = - 1;
  this->numberOfLargeGreatestCommonDivisorsStart = - 1;
  this->millisecondsLastLog = - 1;
  this->startTimeEvaluationMilliseconds = - 1;
  this->startParsing = - 1;
  this->lastStopwatchParsing = - 1;
  this->trivialPerformancePerHandler.makeZero();
  this->nonTrivialPerformancePerHandler.makeZero();
  this->patternMatchMilliseconds = 0;
  this->loopDetectionMilliseconds = 0;
  this->cachePerformanceMilliseconds = 0;
}

void Calculator::reset() {
  this->statistics.reset();
  this->ruleCollectionId = 0;
  this->mode = Calculator::Mode::full;
  this->maximumAlgebraicTransformationsPerExpression = 100;
  this->maximumRecursionDepth = 3000;
  this->recursionDepth = 0;
  this->depthRecursionReached = 0;
  this->flagWriteLatexPlots = false;
  this->flagLogEvaluation = false;
  this->flagUseNumberColors = false;
  this->flagLogRules = false;
  this->flagLogCache = false;
  this->flagLogpatternMatching = false;
  this->flagUseLnAbsInsteadOfLogForIntegrationNotation = false;
  this->flagLogFullTreeCrunching = false;
  this->flagNewContextNeeded = true;
  this->flagDisplayFullExpressionTree = false;
  this->flagHidePolynomialBuiltInTypeIndicator = false;
  this->flagDontUseFracInRational = false;
  this->flagForkingprocessAllowed = true;
  this->flagDontUsePredefinedWordSplits = false;
  this->flagPlotNoControls = true;
  this->flagPlotShowJavascriptOnly = false;
  this->flagHasGraphics = false;
  this->flagUseBracketsForIntervals = false;
  this->flagUseMathML = false;
  this->flagSkipEvaluation = false;
  this->maximumLatexChars = 2000;
  this->objectContainer.reset();
  this->operations.clear();
  this->operationsTransformingChildren.clear();
  this->namedRules.clear();
  this->allBuiltInTypes.clear();
  this->atomsThatAllowCommutingOfCompositesStartingWithThem.clear();
  this->atomsThatFreezeArguments.clear();
  this->atomsNotInterpretedAsFunctions.clear();
  this->atomsNotAllowingChainRule.clear();
  this->atomsWhoseExponentsAreInterpretedAsFunctions.clear();
  this->atomsThatMustNotBeCached.clear();
  this->arithmeticOperations.clear();
  this->knownFunctionsWithComplexRange.clear();
  this->knownDoubleConstants.clear();
  this->knownDoubleConstantValues.setSize(0);
  this->knownOperationsInterpretedAsFunctionsMultiplicatively.clear();
  this->flagTimeLimitErrorDetected = false;
  this->flagFirstErrorEncountered = false;
  this->flagMaxTransformationsErrorEncountered = false;
  this->flagMaxRecursionErrorEncountered = false;
  this->flagAbortComputationASAP = false;
  this->flagDisplayContext = false;
  this->evaluatedExpressionsStack.clear();
  this->evaluationErrors.setSize(0);
  this->programExpression.reset(*this);
  this->cachedExpressionsPerStack.clear();
  this->globalCache.clear();
  // The expression container must be cleared second to last.
  this->allChildExpressions.clear();
  // The hashes list below is used in computing the hashes of the list above.
  // It must therefore be cleared last.
  this->allChildExpressionHashes.clear();
  this->parser.reset();
  this->approximationHandlers.clear();
}

void Calculator::initializeLogDuration(Calculator::Mode desiredMode) {
  int start = global.getElapsedMilliseconds();
  this->initialize(desiredMode);
  int duration = global.getElapsedMilliseconds() - start;
  global
  << "Calculator initialization took "
  << Logger::blue
  << duration
  << " ms."
  << Logger::endL;
}

void Calculator::initializeBuiltInTypes() {
  STACK_TRACE("Calculator::initializeBuiltInTypes");
  this->addBuiltInType<Rational>();
  this->addBuiltInType<ElementZmodP>();
  this->addBuiltInType<double>();
  this->addBuiltInType<AlgebraicNumber>();
  this->addBuiltInType<Polynomial<Rational> >();
  this->addBuiltInType<Polynomial<AlgebraicNumber> >();
  this->addBuiltInType<Polynomial<ElementZmodP> >();
  this->addBuiltInType<PolynomialModuloPolynomial<ElementZmodP> >();
  this->addBuiltInType<RationalFraction<Rational> >();
  this->addBuiltInType<RationalFraction<AlgebraicNumber> >();
  this->addBuiltInType<RationalFraction<ElementZmodP> >();
  this->addBuiltInType<std::string>();
  this->addBuiltInType<JSData>();
  this->addBuiltInType<
    ElementUniversalEnveloping<RationalFraction<Rational> >
  >();
  this->addBuiltInType<ElementSemisimpleLieAlgebra<AlgebraicNumber> >();
  this->addBuiltInType<
    ElementTensorsGeneralizedVermas<RationalFraction<Rational> >
  >();
  this->addBuiltInType<VectorPartitionFunction>();
  this->addBuiltInType<CharacterSemisimpleLieAlgebraModule<Rational> >();
  this->addBuiltInType<SemisimpleLieAlgebra*>();
  this->addBuiltInType<LittelmannPath>();
  this->addBuiltInType<MonomialTensor<int, HashFunctions::hashFunction> >();
  this->addBuiltInType<MatrixTensor<Rational> >();
  this->addBuiltInType<Plot>();
  this->addBuiltInType<SemisimpleSubalgebras>();
  this->addBuiltInType<WeylGroupData>();
  this->addBuiltInType<ElementWeylGroup>();
  this->addBuiltInType<
    GroupRepresentation<FiniteGroup<ElementHyperoctahedralGroupR2>, Rational>
  >();
  this->addBuiltInType<ElementHyperoctahedralGroupR2>();
  this->addBuiltInType<
    GroupRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
  >();
  this->addBuiltInType<
    VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational>
  >();
  this->addBuiltInType<ElementWeylAlgebra<Rational> >();
  this->addBuiltInType<Weight<Rational> >();
  this->addBuiltInType<Weight<Polynomial<Rational> > >();
  this->addBuiltInType<ElementEllipticCurve<Rational> >();
  this->addBuiltInType<ElementEllipticCurve<ElementZmodP> >();
  this->addBuiltInType<InputBox>();
  this->addBuiltInType<Lattice>();
}

void Calculator::initialize(Calculator::Mode desiredMode) {
  STACK_TRACE("Calculator::initialize");
  this->reset();
  this->mode = desiredMode;
  this->operations.setExpectedSize(1000);
  this->namedRules.setExpectedSize(500);
  this->allBuiltInTypes.setExpectedSize(50);
  this->formatVisibleStrings.flagExpressionIsTopLevel = true;
  // Operation List is the very first operation.
  // It signifies a non-atomic expression.
  // Operation List is signified by the empty string.
  // Operation List has index 0.
  this->addOperationNoRepetitionAllowed("");
  this->addOperationNoRepetitionAllowed("=");
  this->addOperationNoRepetitionAllowed(";");
  this->addOperationNoRepetitionAllowed("if=");
  this->addOperationNoRepetitionAllowed("+");
  this->addOperationNoRepetitionAllowed("-");
  this->addOperationNoRepetitionAllowed("/");
  this->addOperationNoRepetitionAllowed("*");
  this->addOperationNoRepetitionAllowed("!");
  this->addOperationNoRepetitionAllowed("mod");
  this->addOperationNoRepetitionAllowed("and");
  this->addOperationNoRepetitionAllowed("or");
  this->addOperationNoRepetitionAllowed("\\times");
  this->addOperationNoRepetitionAllowed("\\otimes");
  this->addOperationNoRepetitionAllowed("\\oplus");
  this->addOperationNoRepetitionAllowed("\\binom");
  this->addOperationNoRepetitionAllowed("\\sqrt");
  this->addOperationNoRepetitionAllowed("\\in");
  this->addOperationNoRepetitionAllowed("[]");
  this->addOperationNoRepetitionAllowed("=:");
  this->addOperationNoRepetitionAllowed("^");
  this->addOperationNoRepetitionAllowed("\\circ");
  this->addOperationNoRepetitionAllowed("\\geq");
  this->addOperationNoRepetitionAllowed("\\leq");
  this->addOperationNoRepetitionAllowed(">");
  this->addOperationNoRepetitionAllowed("<");
  this->addOperationNoRepetitionAllowed("==");
  this->addOperationNoRepetitionAllowed("===");
  this->addOperationNoRepetitionAllowed("\\cup");
  this->addOperationNoRepetitionAllowed("\\sqcup");
  this->addOperationNoRepetitionAllowed("\\cap");
  this->addOperationNoRepetitionAllowed("Error");
  this->addOperationNoRepetitionAllowed("Sequence");
  this->addOperationNoRepetitionAllowed("Matrix");
  this->addOperationNoRepetitionAllowed("Context");
  this->addOperationNoRepetitionAllowed("|");
  this->addOperationNoRepetitionAllowed("\"");
  this->addOperationNoRepetitionAllowed("PolyVars");
  this->addOperationNoRepetitionAllowed("DiffOpVars");
  this->addOperationNoRepetitionAllowed("\\to");
  this->addOperationNoRepetitionAllowed("if");
  this->addOperationNoRepetitionAllowed("\\lim");
  this->addOperationNoRepetitionAllowed("LogBase");
  this->addOperationNoRepetitionAllowed("\\int");
  this->initializeBuiltInTypes();
  this->initializeOperationsInterpretedAsFunctionsMultiplicatively();
  this->initializePredefinedStandardOperationsWithoutHandler();
  this->initializeBuiltInsFreezeArguments();
  this->initializeFunctionsStandard();
  this->initializeFunctionsTransformingChildren();
  if (this->mode == Calculator::Mode::full) {
    this->initializeFunctionsCryptoAndEncoding();
    this->initializeFunctionsScientificBasic();
    this->initializeFunctionsVectorPartitionFunctions();
    this->initializeFunctionsSemisimpleLieAlgebras();
    this->initializeFunctionsExtra();
  }
  this->initializeAtomsThatAllowCommutingOfArguments();
  this->initializeAtomsThatFreezeArguments();
  this->initializeAtomsNotGoodForChainRule();
  this->initializeBuiltInAtomsNotInterpretedAsFunctions();
  this->initializeBuiltInAtomsWhosePowersAreInterpretedAsFunctions();
  this->initializeOperationsThatAreKnownFunctions();
  this->initializeAtomsNonCacheable();
  this->initializeArithmeticOperations();
  this->initializeApproximationFunctions();
  this->parser.initializePredefinedWordSplits();
  this->parser.initializeStringsThatSplitIfFollowedByDigit();
  this->initializeToStringHandlers();
  Expression::initializeToMathMLHandlers(*this);
  this->ruleStack.reset(*this, 100);
  this->ruleStack.addChildAtomOnTop(this->opCommandSequence());
  this->numberOfPredefinedAtoms = this->operations.size();
  // <-operations added up to this point are called ``operations''
  this->checkConsistencyAfterInitialization();
}

bool Calculator::checkPredefinedFunctionNameRepetitions() {
  STACK_TRACE("Calculator::checkPredefinedFunctionNameRepetitions");
  HashedList<std::string> ruleIds;
  for (int i = 0; i < this->operations.size(); i ++) {
    MemorySaving<Calculator::OperationHandlers>& current =
    this->operations.values[i];
    if (current.isZeroPointer()) {
      continue;
    }
    ListReferences<Function>* currentHandlers = &current.getElement().handlers;
    for (int j = 0; j < 2; j ++) {
      for (int k = 0; k < currentHandlers->size; k ++) {
        std::string& currentName = (*currentHandlers)[k].calculatorIdentifier;
        if (currentName == "") {
          continue;
        }
        if (ruleIds.contains(currentName)) {
          global.fatal
          << "Calculator identifier: "
          << currentName
          << " is not unique. ";
        }
        ruleIds.addOnTopNoRepetitionMustBeNew(currentName);
      }
      currentHandlers = &current.getElement().compositeHandlers;
    }
  }
  return true;
}

void Calculator::logPublicError(const std::string& error) {
  if (this->errorsPublic.str() != "") {
    this->errorsPublic << error;
  }
}

bool CalculatorParser::decreaseStack(int decrease) {
  (*this->syntacticStack).setSize(this->syntacticStack->size - decrease);
  return true;
}

bool CalculatorParser::decreaseStackExceptLast(int decrease) {
  if (decrease <= 0) {
    return true;
  }
  if ((*this->syntacticStack).size - decrease <= 0) {
    global.fatal << "Bad stack decrease amount. " << global.fatal;
  }
  (*this->syntacticStack)[(*this->syntacticStack).size - decrease - 1] =
  *this->syntacticStack->lastObject();
  (*this->syntacticStack).setSize((*this->syntacticStack).size - decrease);
  return true;
}

bool CalculatorParser::decreaseStackExceptLastTwo(int decrease) {
  if (decrease <= 0) {
    return true;
  }
  if ((*this->syntacticStack).size - decrease <= 0) {
    global.fatal << "Bad stack decrease amount. " << global.fatal;
  }
  (*this->syntacticStack)[(*this->syntacticStack).size - decrease - 2] = (
    *this->syntacticStack
  )[(*this->syntacticStack).size - 2];
  (*this->syntacticStack)[(*this->syntacticStack).size - decrease - 1] =
  *this->syntacticStack->lastObject();
  (*this->syntacticStack).setSize((*this->syntacticStack).size - decrease);
  return true;
}

bool CalculatorParser::canBeRegardedAsDifferentialForm(
  const std::string& input
) {
  return StringRoutines::stringBeginsWith(input, "d") && input.size() > 1;
}

bool CalculatorParser::canBeRegardedAsDifferentialForm(
  const SyntacticElement& input
) {
  SyntacticElement::Role role = input.syntacticRole;
  if (role == SyntacticElement::VARIABLE || role == SyntacticElement::LETTERS) {
    return this->canBeRegardedAsDifferentialForm(input.source);
  }
  if (role != SyntacticElement::EXPRESSION) {
    return false;
  }
  std::string variableString;
  if (!input.data.isAtomUserDefined(&variableString)) {
    return false;
  }
  return this->canBeRegardedAsDifferentialForm(variableString);
}

bool CalculatorParser::replaceOXXByEXX() {
  SyntacticElement& left = (*this->syntacticStack)[(*this->syntacticStack).size
    -
    3
  ];
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Calculator::replaceOXXByEXX]";
  }
  left.data.makeAtom(*this->owner, left.source);
  left.syntacticRole = SyntacticElement::EXPRESSION;
  return true;
}

bool CalculatorParser::isRightSeparator(unsigned char c) {
  switch (c) {
  case ' ':
  case 160:
    // &nbsp character
  case '\r':
  case '\n':
  case '\t':
  case '>':
  case '<':
  case ':':
  case ',':
  case ';':
  case '+':
  case '*':
  case '^':
  case '=':
  case '_':
  case '%':
  case '/':
  case '-':
  case '`':
  case '[':
  case '&':
  case '\\':
  case '}':
  case '{':
  case '~':
  case '$':
  case ']':
  case '(':
  case ')':
  case '.':
  case '!':
  case '|':
  case '\"':
    return true;
  default:
    return false;
  }
}

bool CalculatorParser::isLeftSeparator(unsigned char c) {
  switch (c) {
  case ' ':
  case 160:
    // &nbsp character
  case '\r':
  case '\n':
  case '\t':
  case '>':
  case '<':
  case ':':
  case ',':
  case ';':
  case '+':
  case '*':
  case '^':
  case '&':
  case '=':
  case '_':
  case '/':
  case '-':
  case '`':
  case '[':
  case '%':
  case '}':
  case '{':
  case '~':
  case '$':
  case ']':
  case '(':
  case ')':
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
  case '.':
  case '!':
  case '|':
  case '\"':
    return true;
  default:
    return false;
  }
}

const HashedList<std::string>& CalculatorParser::whitespaceContainer() {
  static HashedList<std::string> result;
  if (result.size > 0) {
    return result;
  }
  result.addOnTop("\t");
  result.addOnTop("\n");
  // Vertical tab.
  result.addOnTop("\v");
  // Form feed.
  result.addOnTop("\f");
  // Carriage return.
  result.addOnTop("\r");
  // The following are 2 bytes each.
  // Next line (nel); has 2 bytes.
  result.addOnTop("\u0085");
  // Non-breaking space; has 2 bytes.
  result.addOnTop("\u00a0");
  // The following are 3 bytes each.
  // En quad.
  result.addOnTop("\u2000");
  // Mutton quad.
  result.addOnTop("\u2001");
  // En space.
  result.addOnTop("\u2002");
  // Em space.
  result.addOnTop("\u2003");
  // Three-per-em space.
  result.addOnTop("\u2004");
  // Four-per-em space.
  result.addOnTop("\u2005");
  // Six-per-em space.
  result.addOnTop("\u2006");
  // Figure space.
  result.addOnTop("\u2007");
  // Punctuation space.
  result.addOnTop("\u2008");
  // Thin space.
  result.addOnTop("\u2009");
  // Hair space.
  result.addOnTop("\u200A");
  // Line separator.
  result.addOnTop("\u2028");
  // Paragraph separator.
  result.addOnTop("\u2029");
  // Narrow no-break space.
  result.addOnTop("\u202f");
  // Medium mathematical space.
  result.addOnTop("\u205f");
  // ideographic space.
  result.addOnTop("\u3000");
  return result;
}

bool CalculatorParser::isInterpretedAsEmptySpace(const std::string& input) {
  if (input.size() != 1) {
    return false;
  }
  return this->isInterpretedAsEmptySpace(static_cast<unsigned char>(input[0]));
}

bool CalculatorParser::isInterpretedAsEmptySpace(unsigned char input) {
  switch (input) {
  case '\n':
    // numerical value: 10
  case '\r':
    // numerical value: 13
  case '\t':
  case 160:
    // &nbsp
    return true;
  default:
    return false;
  }
  // return false;
}

void CalculatorParser::initializePredefinedWordSplits() {
  STACK_TRACE("CalculatorParser::initializePredefinedWordSplits");
  List<std::string> splitVariables = List<std::string>({"x", "y"});
  this->predefinedWordSplits.setKeyValue("xy", List<std::string>({"x", "y"}));
  this->predefinedWordSplits.setKeyValue("yx", List<std::string>({"y", "x"}));
  this->predefinedWordSplits.setKeyValue("ab", List<std::string>({"a", "b"}));
  this->predefinedWordSplits.setKeyValue("ba", List<std::string>({"b", "a"}));
  this->predefinedWordSplits.setKeyValue(
    "xdx", List<std::string>({"x", "dx"})
  );
  this->addTrigonometricSplit("int", splitVariables);
  this->addTrigonometricSplit("sin", splitVariables);
  this->addTrigonometricSplit("cos", splitVariables);
  this->addTrigonometricSplit("tan", splitVariables);
  this->addTrigonometricSplit("cot", splitVariables);
  this->addTrigonometricSplit("sec", splitVariables);
  this->addTrigonometricSplit("csc", splitVariables);
}

void CalculatorParser::extractSyntacticElementsDefaultOutput(
  const std::string& input
) {
  STACK_TRACE("CalculatorParser::extractSyntacticElementsDefaultOutput");
  ListReferences<SyntacticElement> contiguousWords;
  this->initializeStatic();
  this->extractContiguousWords(input, contiguousWords);
  *this->toBeParsed = contiguousWords;
  SyntacticElement currentElement;
  currentElement.data.reset(*this->owner);
  currentElement.syntacticRole = SyntacticElement::END_PROGRAM;
  (*this->toBeParsed).addOnTop(currentElement);
}

bool CalculatorParser::shouldSplitOutsideQuotes(
  const std::string& left, char right
) {
  if (left.size() == 0) {
    return false;
  }
  char leftLastChar = left[left.size() - 1];
  bool rightIsDigit = MathRoutines::isDigit(right);
  bool leftIsDigit = MathRoutines::isDigit(leftLastChar);
  if (leftIsDigit && rightIsDigit) {
    return false;
  }
  char c2 = static_cast<char>(12 * 16 + 2);
  char nbsp = static_cast<char>(160);
  if (leftLastChar == c2 && right == nbsp) {
    // Do not separate 0xc2 0xa0:
    // that is a white space that will be separated later.
    return false;
  }
  if (
    this->isLeftSeparator(static_cast<unsigned char>(left[0])) ||
    this->isRightSeparator(static_cast<unsigned char>(right)) ||
    left == " "
  ) {
    return true;
  }
  if (rightIsDigit) {
    if (
      leftLastChar == '\\' ||
      this->stringsThatSplitIfFollowedByDigit.contains(left)
    ) {
      return true;
    }
  }
  return false;
}

void CalculatorParser::parseConsumeQuote(
  const std::string& input,
  unsigned int& indexOfLast,
  List<SyntacticElement>& output
) {
  SyntacticElement quoteStart;
  SyntacticElement quoteEnd;
  SyntacticElement content;
  quoteStart.syntacticRole = SyntacticElement::QUOTE;
  quoteEnd = quoteStart;
  content.syntacticRole = SyntacticElement::EXPRESSION;
  output.addOnTop(quoteStart);
  bool previousIsUnescapedBackslash = false;
  std::string consumed;
  for (indexOfLast ++; indexOfLast < input.size(); indexOfLast ++) {
    char current = input[indexOfLast];
    if (current == '"' && !previousIsUnescapedBackslash) {
      content.data.makeAtom(*this->owner, consumed);
      output.addOnTop(content);
      output.addOnTop(quoteEnd);
      return;
    }
    if (current == '\\') {
      previousIsUnescapedBackslash = !previousIsUnescapedBackslash;
    } else {
      previousIsUnescapedBackslash = false;
    }
    consumed.push_back(current);
  }
  content.data.makeAtom(*this->owner, consumed);
  output.addOnTop(content);
}

bool CalculatorParser::parseNoEmbeddingInCommand(
  const std::string& input, Expression& output
) {
  return this->parse(input, true, output);
}

bool CalculatorParser::parseEmbedInCommandSequence(
  const std::string& input, Expression& output
) {
  return this->parse(input, true, output);
}

bool CalculatorParser::trimNonStandardWhiteSpaceFromEnd(
  std::string& inputOutput
) {
  int size = static_cast<int>(inputOutput.size());
  const HashedList<std::string> whiteSpaces = this->whitespaceContainer();
  for (int i = 0; i < 3; i ++) {
    if (i >= size) {
      return false;
    }
    std::string end = inputOutput.substr(size - i - 1);
    if (whiteSpaces.contains(end)) {
      inputOutput.resize(size - i - 1);
      return true;
    }
  }
  return false;
}

SyntacticElement::Role CalculatorParser::syntacticRoleFromCharacter(
  char input
) {
  if (MathRoutines::isDigit(input)) {
    return SyntacticElement::DIGITS_RAW;
  }
  if (MathRoutines::isLatinLetter(input)) {
    return SyntacticElement::LETTERS;
  }
  if (input < 0) {
    return SyntacticElement::LETTERS;
  }
  int inputUnsigned = static_cast<int>(static_cast<unsigned char>(input));
  return CalculatorParser::singleCharacterSyntacticRoles[inputUnsigned];
}

SyntacticElement CalculatorParser::makeFromSingleCharacter(char input) {
  SyntacticElement result;
  result.source = input;
  result.syntacticRole = this->syntacticRoleFromCharacter(input);
  return result;
}

void CalculatorParser::extractContiguousWords(
  const std::string& input, ListReferences<SyntacticElement>& output
) {
  STACK_TRACE("CalculatorParser::extractContiguousWords");
  output.clear();
  if (input.size() == 0) {
    return;
  }
  output.addOnTop(this->makeFromSingleCharacter(input[0]));
  SyntacticElement* previous = &output[0];
  for (unsigned i = 1; i < input.size(); i ++) {
    char current = input[i];
    SyntacticElement maybeNext = this->makeFromSingleCharacter(current);
    if ((
        previous->syntacticRole == SyntacticElement::DIGITS_RAW &&
        maybeNext.syntacticRole == SyntacticElement::DIGITS_RAW
      ) || (
        previous->syntacticRole == SyntacticElement::LETTERS &&
        maybeNext.syntacticRole == SyntacticElement::LETTERS &&
        !previous->endsOnWhitespace()
      )
    ) {
      previous->source.push_back(current);
    } else {
      output.addOnTop(maybeNext);
      previous = &output.lastObject();
    }
  }
}

void CalculatorParser::stringToSyntacticElements(
  const std::string& input, List<SyntacticElement>& output
) {
  STACK_TRACE("CalculatorParser::stringToSyntacticElements");
  std::string current;
  output.reserve(static_cast<signed>(input.size()));
  output.setSize(0);
  char lookAheadChar;
  SyntacticElement currentElement;
  for (unsigned i = 0; i < input.size(); i ++) {
    char currentCharacter = input[i];
    if (currentCharacter == '"') {
      this->parseConsumeQuote(input, i, output);
      continue;
    }
    current.push_back(currentCharacter);
    if (i + 1 < input.size()) {
      lookAheadChar = input[i + 1];
    } else {
      lookAheadChar = ' ';
    }
    bool nonStandardWhitespaceTrimmedFromEnd =
    this->trimNonStandardWhiteSpaceFromEnd(current);
    if (current == "") {
      continue;
    }
    if (this->isInterpretedAsEmptySpace(current)) {
      current = " ";
    }
    if (!nonStandardWhitespaceTrimmedFromEnd) {
      if (!this->shouldSplitOutsideQuotes(current, lookAheadChar)) {
        continue;
      }
    }
    if (this->keyWordsToSyntacticRoles.contains(current)) {
      currentElement.data.reset(*this->owner);
      currentElement.syntacticRole =
      this->keyWordsToSyntacticRoles.getValue(
        current, SyntacticElement::UNKNOWN
      );
      currentElement.source = current;
      output.addOnTop(currentElement);
    } else if (MathRoutines::hasDecimalDigitsOnly(current)) {
      currentElement.data.assignValue(*this->owner, current);
      currentElement.syntacticRole = SyntacticElement::DIGITS;
      output.addOnTop(currentElement);
    } else {
      currentElement.syntacticRole = SyntacticElement::LETTERS;
      currentElement.source = current;
      currentElement.data.makeAtom(*this->owner, current);
      output.addOnTop(currentElement);
    }
    current = "";
  }
}

int CalculatorParser::getOperationIndexNoFail(const std::string& operation) {
  return this->owner->operations.getIndexNoFail(operation);
}

bool CalculatorParser::
replaceEXXSequenceXBy_Expression_with_E_instead_of_sequence() {
  SyntacticElement& sequenceElement = (*this->syntacticStack)[(
      *this->syntacticStack
    ).size -
    2
  ];
  SyntacticElement& functionExpressionlt = (*this->syntacticStack)[(
      *this->syntacticStack
    ).size -
    5
  ];
  Expression newExpression;
  newExpression.reset(*this->owner);
  newExpression.setExpectedSize(sequenceElement.data.size());
  newExpression.addChildOnTop(functionExpressionlt.data);
  if (sequenceElement.data.isAtom()) {
    newExpression.addChildOnTop(sequenceElement.data);
  } else {
    for (int i = 1; i < sequenceElement.data.size(); i ++) {
      newExpression.addChildOnTop(sequenceElement.data[i]);
    }
  }
  functionExpressionlt.data = newExpression;
  functionExpressionlt.syntacticRole = SyntacticElement::EXPRESSION;
  return this->decreaseStackSetCharacterRanges(4);
}

bool CalculatorParser::replaceXEXByEContainingOE(int inputOpIndex) {
  SyntacticElement& outputElement = (*this->syntacticStack)[(
      *this->syntacticStack
    ).size -
    3
  ];
  SyntacticElement& inputElement = (*this->syntacticStack)[(
      *this->syntacticStack
    ).size -
    2
  ];
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Calculator::replaceXEXByEContainingOE]";
  }
  outputElement.data.reset(*this->owner, 2);
  outputElement.data.addChildAtomOnTop(inputOpIndex);
  outputElement.data.addChildOnTop(inputElement.data);
  outputElement.syntacticRole = SyntacticElement::EXPRESSION;
  return this->decreaseStackSetCharacterRanges(2);
}

bool CalculatorParser::isBoundVariableInContext(int inputOperation) {
  return this->boundVariablesInContext.contains(inputOperation);
}

bool CalculatorParser::isNonBoundVariableInContext(int inputOperation) {
  return this->nonBoundVariablesInContext.contains(inputOperation);
}

bool CalculatorParser::setLastRuleName(const std::string& ruleName) {
  if (!this->flagLogSyntaxRules) {
    return true;
  }
  this->lastRuleName = ruleName;
  return true;
}

bool CalculatorParser::replaceXXVXdotsXbyE_BOUND_XdotsX(int numberOfXs) {
  SyntacticElement& element = (*this->syntacticStack)[(*this->syntacticStack).
    size -
    numberOfXs -
    1
  ];
  int boundVariable = element.data.data;
  if (this->isNonBoundVariableInContext(boundVariable)) {
    std::stringstream out;
    out
    << "Syntax error. In the same syntactic scope, the string "
    << this->owner->operations.keys[boundVariable]
    << " is first used to denote a non-bound variable "
    << "but later to denote a bound variable. This is not allowed. ";
    element.errorString = out.str();
    element.syntacticRole = SyntacticElement::ERROR;
    this->decreaseStackSetCharacterRanges(numberOfXs);
    this->replaceXXYByY();
    return true;
  }
  if (!this->isBoundVariableInContext(boundVariable)) {
    this->boundVariablesInContext.addOnTopNoRepetition(boundVariable);
  }
  element.data.reset(*this->owner, 2);
  element.data.addChildAtomOnTop(this->owner->opBind());
  element.data.addChildAtomOnTop(boundVariable);
  element.syntacticRole = SyntacticElement::EXPRESSION;
  this->decreaseStackSetCharacterRanges(numberOfXs);
  this->replaceXXYByY();
  return true;
}

bool CalculatorParser::replaceVXdotsXbyE_NONBOUND_XdotsX(int numberOfXs) {
  SyntacticElement& element = (*this->syntacticStack)[(*this->syntacticStack).
    size -
    1 -
    numberOfXs
  ];
  int boundVariable = element.data.data;
  if (this->isBoundVariableInContext(boundVariable)) {
    element.data.reset(*this->owner, 2);
    element.data.addChildAtomOnTop(this->owner->opBind());
    element.data.addChildAtomOnTop(boundVariable);
  } else {
    element.data.makeAtom(*this->owner, boundVariable);
    if (!this->isNonBoundVariableInContext(boundVariable)) {
      this->nonBoundVariablesInContext.addOnTop(boundVariable);
    }
  }
  element.syntacticRole = SyntacticElement::EXPRESSION;
  return true;
}

bool CalculatorParser::replaceElementsByMatrixStart(int numberOfXs) {
  SyntacticElement& currentElement = (*this->syntacticStack)[(
      *this->syntacticStack
    ).size -
    numberOfXs
  ];
  currentElement.dataList.setExpectedSize(10);
  currentElement.dataList.setSize(1);
  currentElement.dataList.lastObject()->makeSequence(*this->owner);
  currentElement.syntacticRole = SyntacticElement::MATRIX_UNDER_CONSTRUCTION;
  return this->decreaseStackSetCharacterRanges(numberOfXs - 1);
}

bool CalculatorParser::replaceMatrixXByE() {
  SyntacticElement& matrixElement = (*this->syntacticStack)[(
      *this->syntacticStack
    ).size -
    2
  ];
  Matrix<Expression> matrix;
  int numberOfColumns = 0;
  for (int i = 0; i < matrixElement.dataList.size; i ++) {
    numberOfColumns =
    MathRoutines::maximum(
      matrixElement.dataList[i].size() - 1, numberOfColumns
    );
  }
  if (matrixElement.dataList.size > 1) {
    if (matrixElement.dataList.lastObject()->size() - 1 == 0) {
      // We have an empty last row.
      // This is obtained for example when parsing the following.
      // \begin{array}{cc} 1&1\\ 2&3\\ \end{array}
      // where the last \\ creates an empty last row (in our set of parsing
      // rules).
      // We trim that last row.
      matrixElement.dataList.setSize(matrixElement.dataList.size - 1);
    }
  }
  int numberOfRows = matrixElement.dataList.size;
  if (numberOfColumns > 0 && numberOfRows > 0) {
    matrix.initialize(numberOfRows, numberOfColumns);
    for (int i = 0; i < numberOfRows; i ++) {
      for (int j = 0; j < numberOfColumns; j ++) {
        if (j + 1 >= matrixElement.dataList[i].size()) {
          matrix.elements[i][j].assignValue(*this->owner, 0);
          continue;
        }
        matrix.elements[i][j] = matrixElement.dataList[i][j + 1];
      }
    }
    matrixElement.data.assignMatrixExpressions(
      matrix, *this->owner, true, true
    );
  } else {
    matrixElement.data.makeMatrix(nullptr, *this->owner);
  }
  matrixElement.dataList.setSize(0);
  matrixElement.syntacticRole = SyntacticElement::EXPRESSION;
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "matrix X to expression";
  }
  return this->decreaseStackSetCharacterRanges(1);
}

bool CalculatorParser::replaceMatrixEXByMatrixNewRow() {
  SyntacticElement& matrixElement = (*this->syntacticStack)[(
      *this->syntacticStack
    ).size -
    3
  ];
  SyntacticElement& element = (*this->syntacticStack)[(*this->syntacticStack).
    size -
    2
  ];
  matrixElement.dataList.lastObject()->addChildOnTop(element.data);
  matrixElement.dataList.setSize(matrixElement.dataList.size + 1);
  matrixElement.dataList.lastObject()->makeSequence(*this->owner);
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Calculator::replaceMatrixEXByMatrixNewRow]";
  }
  return this->decreaseStackSetCharacterRanges(2);
}

bool CalculatorParser::replaceMatrixEXByMatrix() {
  SyntacticElement& matrixElement = (*this->syntacticStack)[(
      *this->syntacticStack
    ).size -
    3
  ];
  SyntacticElement& element = (*this->syntacticStack)[(*this->syntacticStack).
    size -
    2
  ];
  matrixElement.dataList.lastObject()->addChildOnTop(element.data);
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceMatrixEXByMatrix]";
  }
  return this->decreaseStackSetCharacterRanges(2);
}

std::string CalculatorParser::toStringIsCorrectAsciiCalculatorString(
  const std::string& input
) {
  std::stringstream out;
  HashedList<unsigned char, HashFunctions::hashFunction> badCharacters;
  for (unsigned i = 0; i < input.size(); i ++) {
    unsigned char next = static_cast<unsigned char>(input[i]);
    if (!this->isStandardCalculatorCharacter(next)) {
      badCharacters.addOnTopNoRepetition(next);
    }
  }
  if (badCharacters.size > 0) {
    out << "Non-ascii characters detected in your input, ";
    List<std::string> hexCodes;
    for (int i = 0; i < badCharacters.size; i ++) {
      hexCodes.addOnTop(StringRoutines::convertByteToHex(badCharacters[i]));
    }
    out << "with byte values: " << hexCodes.toStringCommaDelimited() << ". ";
    out
    << "Perhaps you copy+pasted from webpage/pdf file "
    << "or are using non-English keyboard setup? ";
  }
  return out.str();
}

bool CalculatorParser::isStandardCalculatorCharacter(unsigned char input) {
  if (10 <= input && input <= 126) {
    return true;
  }
  if (this->isInterpretedAsEmptySpace(input)) {
    return true;
  }
  return false;
}

bool CalculatorParser::allowsApplyFunctionInPreceding(
  SyntacticElement::Role role
) {
  return
  role != SyntacticElement::LEFT_CURLY_BRACE &&
  role != SyntacticElement::UNDERSCORE &&
  role != SyntacticElement::CIRC &&
  role != SyntacticElement::LEFT_RIGHT_CURLY_BRACE &&
  role != SyntacticElement::DOLLAR;
}

bool CalculatorParser::replaceYBySequenceY() {
  this->replaceYXdotsXBySequenceYXdotsX(0);
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceYBySequenceY]";
  }
  return true;
}

bool CalculatorParser::replaceYXdotsXBySequenceYXdotsX(int numberOfXs) {
  SyntacticElement& main = (*this->syntacticStack)[(*this->syntacticStack).size
    -
    numberOfXs -
    1
  ];
  Expression newExpression;
  newExpression.reset(*this->owner, 2);
  newExpression.addChildAtomOnTop(this->owner->opSequence());
  newExpression.addChildOnTop(main.data);
  main.data = newExpression;
  main.syntacticRole = SyntacticElement::SEQUENCE;
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceYXdotsXBySequenceYXdotsX]";
  }
  return true;
}

bool CalculatorParser::replaceVbyVdotsVAccordingToPredefinedWordSplitsPart2(
  const std::string& currentVariable
) {
  STACK_TRACE(
    "CalculatorParser::replaceVbyVdotsVAccordingToPredefinedWordSplitsPart2"
  );
  if (currentVariable.size() <= 1) {
    return false;
  }
  List<std::string> split;
  HashedList<std::string> splitVariableSet;
  for (char character : currentVariable) {
    std::string element(1, character);
    split.addOnTop(element);
    splitVariableSet.addOnTopNoRepetition(element);
  }
  if (split.size > 4) {
    return false;
  }
  HashedList<std::string> splitWithoutAlphabetEnd = splitVariableSet;
  splitWithoutAlphabetEnd.removeFirstOccurenceSwapWithLast("u");
  splitWithoutAlphabetEnd.removeFirstOccurenceSwapWithLast("v");
  splitWithoutAlphabetEnd.removeFirstOccurenceSwapWithLast("x");
  splitWithoutAlphabetEnd.removeFirstOccurenceSwapWithLast("y");
  splitWithoutAlphabetEnd.removeFirstOccurenceSwapWithLast("z");
  if (splitWithoutAlphabetEnd.size == 0) {
    return this->replaceVByVDotsVWith(split);
  }
  HashedList<std::string> splitWithoutAlphabetStart = splitVariableSet;
  splitWithoutAlphabetStart.removeFirstOccurenceSwapWithLast("a");
  splitWithoutAlphabetStart.removeFirstOccurenceSwapWithLast("b");
  splitWithoutAlphabetStart.removeFirstOccurenceSwapWithLast("c");
  if (splitWithoutAlphabetStart.size == 0) {
    return this->replaceVByVDotsVWith(split);
  }
  return false;
}

bool CalculatorParser::replaceVByVDotsVWith(const List<std::string>& variables)
{
  SyntacticElement newElement;
  this->popTopSyntacticStack();
  for (int i = 0; i < variables.size; i ++) {
    newElement.data.makeAtom(*this->owner, variables[i]);
    newElement.source = variables[i];
    newElement.syntacticRole = SyntacticElement::VARIABLE;
    (*this->syntacticStack).addOnTop(newElement);
  }
  return true;
}

bool CalculatorParser::replaceVbyVdotsVAccordingToPredefinedWordSplits(
  const std::string& currentVariable
) {
  STACK_TRACE(
    "CalculatorParser::replaceVbyVdotsVAccordingToPredefinedWordSplits"
  );
  if (!this->predefinedWordSplits.contains(currentVariable)) {
    return
    this->replaceVbyVdotsVAccordingToPredefinedWordSplitsPart2(
      currentVariable
    );
  }
  List<std::string>& split =
  this->predefinedWordSplits.getValueCreateEmpty(currentVariable);
  if (!this->flagEncounteredSplit) {
    this->flagEncounteredSplit = true;
    *(this->owner)
    << "Predefined symbol replacement: replacing "
    << currentVariable
    << " with the sequence of symbols "
    << split.toStringCommaDelimited()
    << ". If you do not want such replacements to take "
    << "place you should add the %DontUsePredefinedWordSplits option "
    << "at the start of your input. "
    << "The predefined symbol replacements are used to guard "
    << "the user from accidental typos such as confusing "
    << "x y (the product of x and y) with xy "
    << "(a single variable whose name contains the letters x and y). ";
  }
  return this->replaceVByVDotsVWith(split);
}

bool CalculatorParser::replaceEXdotsXbySsXdotsX(int numberOfDots) {
  SyntacticElement& left = (*this->syntacticStack)[(*this->syntacticStack).size
    -
    numberOfDots -
    1
  ];
  bool found = false;
  for (int i = (*this->syntacticStack).size - numberOfDots - 2; i >= 0; i --) {
    SyntacticElement& current = (*this->syntacticStack)[i];
    if (
      current.totalBoundVariablesInherited >= 0 &&
      current.totalNonBoundVariablesInherited >= 0
    ) {
      this->nonBoundVariablesInContext.setSize(
        current.totalNonBoundVariablesInherited
      );
      this->boundVariablesInContext.setSize(
        current.totalBoundVariablesInherited
      );
      found = true;
      break;
    }
  }
  if (!found) {
    this->nonBoundVariablesInContext.clear();
    this->boundVariablesInContext.clear();
  }
  Expression newExpression;
  newExpression.reset(*this->owner);
  newExpression.addChildAtomOnTop(this->owner->opCommandSequence());
  newExpression.addChildOnTop(left.data);
  left.data = newExpression;
  left.syntacticRole = SyntacticElement::SEQUENCE_STATEMENTS;
  left.source = "";
  return true;
}

bool CalculatorParser::replaceSsSsXdotsXbySsXdotsX(int numberOfDots) {
  SyntacticElement& left = (*this->syntacticStack)[(*this->syntacticStack).size
    -
    numberOfDots -
    2
  ];
  SyntacticElement& right = (*this->syntacticStack)[(*this->syntacticStack).
    size -
    numberOfDots -
    1
  ];
  if (!left.data.startsWith(this->owner->opCommandSequence())) {
    global.fatal
    << "replaceSsSsXdotsXbySsXdotsX called "
    << "but left expression is not EndStatement."
    << global.fatal;
  }
  left.data.setExpectedSize(left.data.size() + right.data.size() - 1);
  for (int i = 1; i < right.data.size(); i ++) {
    left.data.addChildOnTop(right.data[i]);
  }
  left.syntacticRole = SyntacticElement::SEQUENCE_STATEMENTS;
  (*this->syntacticStack).popIndexShiftDown((*this->syntacticStack).size -
    numberOfDots -
    1
  );
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceSsSsXdotsXbySsXdotsX]";
  }
  return true;
}

bool CalculatorParser::replaceEOEXByEX(int operationIndex) {
  SyntacticElement& left = (*this->syntacticStack)[(*this->syntacticStack).size
    -
    4
  ];
  SyntacticElement& right = (*this->syntacticStack)[(*this->syntacticStack).
    size -
    2
  ];
  left.data.makeXOX(*this->owner, operationIndex, left.data, right.data);
  left.syntacticRole = SyntacticElement::EXPRESSION;
  return this->decreaseStackExceptLast(2);
}

bool CalculatorParser::replaceEXEXBy_OofEE_X(int operation) {
  SyntacticElement& middle = (*this->syntacticStack)[(*this->syntacticStack).
    size -
    3
  ];
  SyntacticElement& left = (*this->syntacticStack)[(*this->syntacticStack).size
    -
    4
  ];
  SyntacticElement& right = (*this->syntacticStack)[(*this->syntacticStack).
    size -
    2
  ];
  Expression newExpression;
  newExpression.reset(*this->owner, 3);
  newExpression.addChildAtomOnTop(operation);
  newExpression.addChildOnTop(left.data);
  newExpression.addChildOnTop(right.data);
  left.data = newExpression;
  middle = *(*this->syntacticStack).lastObject();
  if (this->flagLogSyntaxRules) {
    this->lastRuleName = "[Rule: Calculator::replaceEXEXBy_OofEE_X]";
  }
  return this->decreaseStackExceptLast(2);
}

bool CalculatorParser::isSeparatorFromTheLeftForInterval(
  SyntacticElement::Role input
) {
  return
  input == SyntacticElement::COMMA ||
  input == SyntacticElement::LEFT_PARENTHESIS ||
  input == SyntacticElement::LEFT_BRACKET ||
  input == SyntacticElement::COLON ||
  input == SyntacticElement::SEMICOLON ||
  input == SyntacticElement::EQUALS ||
  input == SyntacticElement::EQUALS_EQUALS ||
  input == SyntacticElement::DOUBLE_BACKSLASH ||
  input == SyntacticElement::CUP ||
  input == SyntacticElement::CAP ||
  input == SyntacticElement::END ||
  input == SyntacticElement::AMPERSAND ||
  input == SyntacticElement::END_PROGRAM;
}

bool CalculatorParser::isSeparatorFromTheRightGeneral(
  SyntacticElement::Role role
) {
  return
  role == SyntacticElement::RIGHT_CURLY_BRACE ||
  role == SyntacticElement::RIGHT_PARENTHESIS ||
  role == SyntacticElement::RIGHT_BRACKET ||
  role == SyntacticElement::COMMA ||
  role == SyntacticElement::COLON ||
  role == SyntacticElement::SEMICOLON ||
  role == SyntacticElement::MATRIX_UNDER_CONSTRUCTION ||
  role == SyntacticElement::EQUALS ||
  role == SyntacticElement::DOUBLE_BACKSLASH ||
  role == SyntacticElement::END ||
  role == SyntacticElement::AMPERSAND ||
  role == SyntacticElement::END_PROGRAM;
}

bool CalculatorParser::isSeparatorFromTheLeftForDefinition(
  SyntacticElement::Role role
) {
  return
  role == SyntacticElement::START_FILLER ||
  role == SyntacticElement::LEFT_CURLY_BRACE ||
  role == SyntacticElement::LEFT_PARENTHESIS ||
  role == SyntacticElement::SEMICOLON ||
  role == SyntacticElement::COMMA ||
  role == SyntacticElement::PERCENT ||
  role == SyntacticElement::SEQUENCE_STATEMENTS ||
  role == SyntacticElement::WHITE_SPACE ||
  role == SyntacticElement::OR ||
  role == SyntacticElement::AND;
}

bool CalculatorParser::isSeparatorFromTheRightForDefinition(
  SyntacticElement::Role role
) {
  return
  role == SyntacticElement::RIGHT_CURLY_BRACE ||
  role == SyntacticElement::RIGHT_PARENTHESIS ||
  role == SyntacticElement::SEMICOLON ||
  role == SyntacticElement::COMMA ||
  role == SyntacticElement::END_PROGRAM ||
  role == SyntacticElement::OR ||
  role == SyntacticElement::AND;
}

bool CalculatorParser::allowsTensorInPreceding(
  const SyntacticElement& lookAhead
) {
  return
  this->allowsTimesInPreceding(lookAhead) ||
  lookAhead.syntacticRole == SyntacticElement::TENSOR_PRODUCT;
}

bool CalculatorParser::allowsTimesInNext(const SyntacticElement& preceding) {
  if (
    preceding.syntacticRole == SyntacticElement::LEFT_RIGHT_CURLY_BRACE ||
    preceding.syntacticRole == SyntacticElement::UPPER_CARET ||
    preceding.syntacticRole == SyntacticElement::UNDERSCORE ||
    preceding.syntacticRole == SyntacticElement::SQRT
  ) {
    return false;
  }
  return true;
}

bool CalculatorParser::allowsTimesInPreceding(
  const SyntacticElement& preceding, const SyntacticElement& lookAhead
) {
  if (preceding.syntacticRole != SyntacticElement::EXPRESSION) {
    return this->allowsTimesInPreceding(lookAhead);
  }
  if (!preceding.data.isBuiltInAtom()) {
    return this->allowsTimesInPreceding(lookAhead);
  }
  if (lookAhead.syntacticRole == SyntacticElement::BACKSLASH) {
    return false;
  }
  return
  lookAhead.syntacticRole == SyntacticElement::PLUS ||
  lookAhead.syntacticRole == SyntacticElement::MINUS ||
  lookAhead.syntacticRole == SyntacticElement::STAR ||
  lookAhead.syntacticRole == SyntacticElement::DIVISION_SIGN ||
  lookAhead.syntacticRole == SyntacticElement::CUP ||
  lookAhead.syntacticRole == SyntacticElement::RIGHT_PARENTHESIS ||
  lookAhead.syntacticRole == SyntacticElement::RIGHT_BRACKET ||
  lookAhead.syntacticRole == SyntacticElement::RIGHT_CURLY_BRACE ||
  lookAhead.syntacticRole == SyntacticElement::EQUALS ||
  lookAhead.syntacticRole == SyntacticElement::GREATER_THAN_LIKE ||
  lookAhead.syntacticRole == SyntacticElement::LESS_THAN_LIKE ||
  lookAhead.syntacticRole == SyntacticElement::COMMA ||
  lookAhead.syntacticRole == SyntacticElement::SEMICOLON ||
  lookAhead.syntacticRole == SyntacticElement::COLON ||
  lookAhead.syntacticRole == SyntacticElement::AMPERSAND ||
  lookAhead.syntacticRole == SyntacticElement::MATRIX_UNDER_CONSTRUCTION ||
  lookAhead.syntacticRole == SyntacticElement::MATRIX_END ||
  lookAhead.syntacticRole == SyntacticElement::BACKSLASH ||
  lookAhead.syntacticRole == SyntacticElement::OR ||
  lookAhead.syntacticRole == SyntacticElement::AND ||
  lookAhead.syntacticRole == SyntacticElement::END_PROGRAM;
}

bool CalculatorParser::allowsTimesInPreceding(
  const SyntacticElement& lookAhead
) {
  SyntacticElement::Role role = lookAhead.syntacticRole;
  return
  role == SyntacticElement::PLUS ||
  role == SyntacticElement::MINUS ||
  role == SyntacticElement::STAR ||
  role == SyntacticElement::DIVISION_SIGN ||
  this->owner->knownOperationsInterpretedAsFunctionsMultiplicatively.contains(
    lookAhead.source
  ) ||
  role == SyntacticElement::FRAC ||
  role == SyntacticElement::EXPRESSION ||
  role == SyntacticElement::DIGITS ||
  role == SyntacticElement::CUP ||
  role == SyntacticElement::LEFT_PARENTHESIS ||
  role == SyntacticElement::LEFT_BRACKET ||
  role == SyntacticElement::PIPE ||
  role == SyntacticElement::RIGHT_PARENTHESIS ||
  role == SyntacticElement::RIGHT_BRACKET ||
  role == SyntacticElement::RIGHT_CURLY_BRACE ||
  role == SyntacticElement::EQUALS ||
  role == SyntacticElement::GREATER_THAN_LIKE ||
  role == SyntacticElement::LESS_THAN_LIKE ||
  role == SyntacticElement::VARIABLE ||
  role == SyntacticElement::LETTERS ||
  role == SyntacticElement::COMMA ||
  role == SyntacticElement::SEMICOLON ||
  role == SyntacticElement::COLON ||
  role == SyntacticElement::AMPERSAND ||
  role == SyntacticElement::MATRIX_END ||
  role == SyntacticElement::OR ||
  role == SyntacticElement::AND ||
  role == SyntacticElement::DOUBLE_BACKSLASH ||
  role == SyntacticElement::SQRT ||
  role == SyntacticElement::END_PROGRAM;
}

bool CalculatorParser::allowsPowerInPreceding(
  SyntacticElement::Role lookAhead
) {
  return lookAhead != SyntacticElement::LEFT_RIGHT_CURLY_BRACE;
}

bool CalculatorParser::allowsPowerInNext(SyntacticElement::Role lookBehind) {
  return lookBehind != SyntacticElement::UNDERSCORE;
}

bool CalculatorParser::allowsLimitProcessInPreceding(
  SyntacticElement::Role lookAhead
) {
  return
  lookAhead == SyntacticElement::AND ||
  lookAhead == SyntacticElement::OR ||
  lookAhead == SyntacticElement::LEFT_PARENTHESIS ||
  lookAhead == SyntacticElement::LEFT_BRACKET ||
  lookAhead == SyntacticElement::RIGHT_PARENTHESIS ||
  lookAhead == SyntacticElement::RIGHT_BRACKET ||
  lookAhead == SyntacticElement::RIGHT_CURLY_BRACE ||
  lookAhead == SyntacticElement::COMMA ||
  lookAhead == SyntacticElement::SEMICOLON ||
  lookAhead == SyntacticElement::COLON ||
  lookAhead == SyntacticElement::AMPERSAND ||
  lookAhead == SyntacticElement::MATRIX_UNDER_CONSTRUCTION ||
  lookAhead == SyntacticElement::BACKSLASH ||
  lookAhead == SyntacticElement::END_PROGRAM;
}

bool CalculatorParser::allowsAndInPreceding(SyntacticElement::Role lookAhead) {
  return
  lookAhead == SyntacticElement::AND ||
  lookAhead == SyntacticElement::OR ||
  lookAhead == SyntacticElement::LEFT_PARENTHESIS ||
  lookAhead == SyntacticElement::LEFT_BRACKET ||
  lookAhead == SyntacticElement::RIGHT_PARENTHESIS ||
  lookAhead == SyntacticElement::RIGHT_BRACKET ||
  lookAhead == SyntacticElement::RIGHT_CURLY_BRACE ||
  lookAhead == SyntacticElement::COMMA ||
  lookAhead == SyntacticElement::SEMICOLON ||
  lookAhead == SyntacticElement::COLON ||
  lookAhead == SyntacticElement::AMPERSAND ||
  lookAhead == SyntacticElement::MATRIX_UNDER_CONSTRUCTION ||
  lookAhead == SyntacticElement::BACKSLASH ||
  lookAhead == SyntacticElement::END_PROGRAM;
}

bool CalculatorParser::allowsOrInPreceding(SyntacticElement::Role lookAhead) {
  return
  lookAhead == SyntacticElement::AND ||
  lookAhead == SyntacticElement::OR ||
  lookAhead == SyntacticElement::LEFT_PARENTHESIS ||
  lookAhead == SyntacticElement::LEFT_BRACKET ||
  lookAhead == SyntacticElement::RIGHT_PARENTHESIS ||
  lookAhead == SyntacticElement::RIGHT_BRACKET ||
  lookAhead == SyntacticElement::RIGHT_CURLY_BRACE ||
  lookAhead == SyntacticElement::COMMA ||
  lookAhead == SyntacticElement::SEMICOLON ||
  lookAhead == SyntacticElement::COLON ||
  lookAhead == SyntacticElement::AMPERSAND ||
  lookAhead == SyntacticElement::MATRIX_UNDER_CONSTRUCTION ||
  lookAhead == SyntacticElement::BACKSLASH ||
  lookAhead == SyntacticElement::END_PROGRAM;
}

bool CalculatorParser::allowsInInPreceding(SyntacticElement::Role lookAhead) {
  return this->allowsIfInPreceding(lookAhead);
}

bool CalculatorParser::allowsIfInPreceding(SyntacticElement::Role lookAhead) {
  return
  lookAhead == SyntacticElement::RIGHT_PARENTHESIS ||
  lookAhead == SyntacticElement::RIGHT_BRACKET ||
  lookAhead == SyntacticElement::RIGHT_CURLY_BRACE ||
  lookAhead == SyntacticElement::COMMA ||
  lookAhead == SyntacticElement::SEMICOLON ||
  lookAhead == SyntacticElement::PLUS ||
  lookAhead == SyntacticElement::MINUS ||
  lookAhead == SyntacticElement::DIVISION_SIGN ||
  lookAhead == SyntacticElement::STAR ||
  lookAhead == SyntacticElement::COLON ||
  lookAhead == SyntacticElement::AMPERSAND ||
  lookAhead == SyntacticElement::MATRIX_UNDER_CONSTRUCTION ||
  lookAhead == SyntacticElement::BACKSLASH ||
  lookAhead == SyntacticElement::END_PROGRAM;
}

bool CalculatorParser::allowsPlusInPreceding(SyntacticElement::Role role) {
  return
  role == SyntacticElement::PLUS ||
  role == SyntacticElement::MINUS ||
  role == SyntacticElement::COMMA ||
  role == SyntacticElement::EQUALS ||
  role == SyntacticElement::LESS_THAN_LIKE ||
  role == SyntacticElement::GREATER_THAN_LIKE ||
  role == SyntacticElement::RIGHT_PARENTHESIS ||
  role == SyntacticElement::SEMICOLON ||
  role == SyntacticElement::RIGHT_BRACKET ||
  role == SyntacticElement::RIGHT_CURLY_BRACE ||
  role == SyntacticElement::COLON ||
  role == SyntacticElement::CHOOSE ||
  role == SyntacticElement::OR ||
  role == SyntacticElement::AND ||
  role == SyntacticElement::SQRT ||
  role == SyntacticElement::END_PROGRAM ||
  role == SyntacticElement::AMPERSAND ||
  role == SyntacticElement::MATRIX_END ||
  role == SyntacticElement::DOUBLE_BACKSLASH;
}

bool CalculatorParser::allowsDivideInPreceding(
  const SyntacticElement& lookAhead
) {
  return this->allowsTimesInPreceding(lookAhead);
}

bool CalculatorParser::extractExpressionsFromPreprocessed(
  Expression& outputExpression, std::string* outputErrors
) {
  STACK_TRACE("CalculatorParser::extractExpressionsFromPreprocessed");
  std::stringstream errorLog;
  (*this->syntacticStack).reserve((*this->toBeParsed).size +
    this->numberOfEmptyTokensStart
  );
  (*this->syntacticStack).setSize(this->numberOfEmptyTokensStart);
  for (int i = 0; i < this->numberOfEmptyTokensStart; i ++) {
    (*this->syntacticStack)[i] = this->getStartFillerElement();
  }
  this->parsingLog = "";
  this->nonBoundVariablesInContext.clear();
  this->boundVariablesInContext.clear();
  const int totalTimesOneRuleCanBeCalled = 1000;
  int counterReport = 0;
  int symbolsToIssueReport = 100;
  int minMillisecondsPerReport = 200;
  this->owner->statistics.startParsing = global.getElapsedMilliseconds();
  this->owner->statistics.lastStopwatchParsing =
  this->owner->statistics.startParsing;
  ProgressReport report;
  bool currentLogEvaluation = this->owner->flagLogEvaluation;
  for (
    this->counterInToBeParsed = 0; this->counterInToBeParsed < (
      *this->toBeParsed
    ).size; this->counterInToBeParsed ++
  ) {
    counterReport ++;
    if (counterReport >= symbolsToIssueReport) {
      counterReport = 0;
      int64_t currentMilliseconds = global.getElapsedMilliseconds();
      if (
        currentMilliseconds - this->owner->statistics.lastStopwatchParsing >
        minMillisecondsPerReport
      ) {
        this->owner->statistics.lastStopwatchParsing = currentMilliseconds;
        std::stringstream reportStream;
        reportStream
        << "Processed "
        << this->counterInToBeParsed
        << " out of "
        << (*this->toBeParsed).size
        << " syntactic elements. ";
        report.report(reportStream.str());
      }
    }
    (*this->syntacticStack).addOnTop((*this->toBeParsed)[
        this->counterInToBeParsed
      ]
    );
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "next token";
      this->logParsingOperation();
    }
    int totalTimesRulesCanBeAppliedWithoutStackDecrease = 0;
    int minStackSize = this->syntacticStack->size;
    while (this->applyOneRule()) {
      if (currentLogEvaluation != this->owner->flagLogEvaluation) {
        this->owner->logTime(this->owner->statistics.lastStopwatchParsing);
        currentLogEvaluation = this->owner->flagLogEvaluation;
      }
      this->logParsingOperation();
      if (this->syntacticStack->size < minStackSize) {
        totalTimesRulesCanBeAppliedWithoutStackDecrease = 0;
        minStackSize = this->syntacticStack->size;
      } else {
        totalTimesRulesCanBeAppliedWithoutStackDecrease ++;
      }
      if (
        totalTimesRulesCanBeAppliedWithoutStackDecrease >
        totalTimesOneRuleCanBeCalled
      ) {
        global.fatal
        << "This may be a programming error: "
        << "CalculatorParser::applyOneRule called more than "
        << totalTimesOneRuleCanBeCalled
        << " times without advancing to the "
        << "next syntactic element in the syntactic soup. "
        << "If this is indeed an expression which requires that "
        << "many application of a single parsing rule, "
        << "then you should modify function Calculator::extractExpressions. "
        << this->parsingLog
        << global.fatal;
      }
    }
  }
  bool success = false;
  if ((*this->syntacticStack).size == this->numberOfEmptyTokensStart) {
    errorLog
    << "Non-meaningful/empty input detected "
    << "(spacebar, enter characters only?).";
  } else if ((*this->syntacticStack).size == this->numberOfEmptyTokensStart + 1
  ) {
    SyntacticElement& result = (*this->syntacticStack)[
      this->numberOfEmptyTokensStart
    ];
    if (
      result.errorString == "" &&
      result.syntacticRole == SyntacticElement::EXPRESSION
    ) {
      outputExpression = result.data;
      success = true;
    } else if (result.errorString != "") {
      errorLog << "Syntax error with message: " << result.errorString;
    } else {
      errorLog
      << "Syntax error: your command "
      << "simplifies to a single syntactic element "
      << "but it is not an expression. <br>";
      errorLog
      << "It simplifies to:<br> "
      << this->toStringSyntacticStackHTMLTable(false, false);
    }
  } else {
    errorLog
    << "Syntax error: your command "
    << "does not simplify to a single syntactic element. <br>";
    errorLog
    << "Instead it simplifies to:<br> "
    << this->toStringSyntacticStackHTMLTable(false, false);
  }
  if (outputErrors != nullptr) {
    *outputErrors = errorLog.str();
  }
  return success;
}

void CalculatorParser::logParsingOperation() {
  if (!this->flagLogSyntaxRules) {
    return;
  }
  std::stringstream out;
  out
  << "["
  << this->lastRuleName
  << "]"
  << "<br>"
  << this->toStringSyntacticStackHTMLTable(false, false);
  this->parsingLog.addOnTop(out.str());
  this->lastRuleName = "";
}

bool CalculatorParser::applyOneRule() {
  STACK_TRACE("CalculatorParser::applyOneRule");
  if (this->syntacticStack->size <= this->numberOfEmptyTokensStart) {
    return false;
  }
  SyntacticElement& lastExpression = (*this->syntacticStack)[(
      *this->syntacticStack
    ).size -
    1
  ];
  SyntacticElement::Role lastRole = lastExpression.syntacticRole;
  SyntacticElement& secondToLastExpression = (*this->syntacticStack)[(
      *this->syntacticStack
    ).size -
    2
  ];
  SyntacticElement::Role secondToLastRole =
  secondToLastExpression.syntacticRole;
  if (
    secondToLastRole == SyntacticElement::QUOTE_UNDER_CONSTRUCTION &&
    lastRole == SyntacticElement::QUOTE
  ) {
    secondToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    Expression quoteContent;
    quoteContent.makeAtom(*this->owner, secondToLastExpression.source);
    secondToLastExpression.data.makeOX(
      *this->owner, this->owner->opQuote(), quoteContent
    );
    this->lastRuleName = "finalize quote";
    return this->popTopSyntacticStack();
  }
  if (
    secondToLastRole == SyntacticElement::QUOTE_UNDER_CONSTRUCTION &&
    lastRole != SyntacticElement::BACKSLASH
  ) {
    secondToLastExpression.source += lastExpression.source;
    this->lastRuleName = "grow quote";
    return this->popTopSyntacticStack();
  }
  if (
    secondToLastRole != SyntacticElement::BACKSLASH &&
    lastRole == SyntacticElement::QUOTE
  ) {
    lastExpression.syntacticRole = SyntacticElement::QUOTE_UNDER_CONSTRUCTION;
    this->lastRuleName = "start quote";
    lastExpression.source = "";
    return true;
  }
  if (lastExpression.syntacticRole == SyntacticElement::RAW) {
    int index =
    this->keyWordsToSyntacticRoles.getIndex(lastExpression.source);
    if (index >= 0) {
      lastExpression.syntacticRole =
      this->keyWordsToSyntacticRoles.values[index];
      lastExpression.data.reset(*this->owner);
      this->lastRuleName = "unknown syntax element";
      return true;
    }
  }
  if (lastExpression.syntacticRole == SyntacticElement::DIGITS_RAW) {
    if (secondToLastExpression.syntacticRole != SyntacticElement::LETTERS) {
      lastExpression.syntacticRole = SyntacticElement::DIGITS;
      this->lastRuleName = "raw digits to digits";
      return true;
    } else {
      secondToLastExpression.source += lastExpression.source;
      this->lastRuleName = "letters[no white space]digits to letters";
      return this->popTopSyntacticStack();
    }
  }
  SyntacticElement& thirdToLastExpression = (*this->syntacticStack)[(
      *this->syntacticStack
    ).size -
    3
  ];
  SyntacticElement::Role thirdToLastRole = thirdToLastExpression.syntacticRole;
  if (
    thirdToLastRole == SyntacticElement::QUOTE_UNDER_CONSTRUCTION &&
    secondToLastRole == SyntacticElement::BACKSLASH
  ) {
    thirdToLastExpression.source +=
    secondToLastExpression.source + lastExpression.source;
    this->lastRuleName = "quote absorbs backslash anything";
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (
    secondToLastRole == SyntacticElement::BACKSLASH &&
    lastRole == SyntacticElement::LETTERS &&
    !StringRoutines::stringBeginsWith(lastExpression.source, "\\")
  ) {
    secondToLastExpression.source += lastExpression.source;
    this->lastRuleName = "backslash letters to letters";
    secondToLastExpression.syntacticRole = lastExpression.syntacticRole;
    return this->popTopSyntacticStack();
  }
  if (
    thirdToLastRole != SyntacticElement::PERCENT &&
    secondToLastExpression.syntacticRole == SyntacticElement::LETTERS &&
    lastExpression.syntacticRole == SyntacticElement::LETTERS &&
    this->keyWordsWithDigits.contains(
      secondToLastExpression.source + lastExpression.source
    )
  ) {
    secondToLastExpression.source += lastExpression.source;
    this->lastRuleName = "merge known letter combinations";
    return this->popTopSyntacticStack();
  }
  if (
    thirdToLastRole != SyntacticElement::PERCENT &&
    secondToLastExpression.syntacticRole == SyntacticElement::LETTERS
  ) {
    this->trimNonStandardWhiteSpaceFromEnd(secondToLastExpression.source);
    int keywordIndex =
    this->keyWordsToSyntacticRoles.getIndex(secondToLastExpression.source);
    if (keywordIndex < 0) {
      // The letters are a new variable.
      secondToLastExpression.syntacticRole = SyntacticElement::VARIABLE;
      secondToLastExpression.data.makeAtom(
        *this->owner,
        this->owner->addOperationNoRepetitionOrReturnIndexFirst(
          secondToLastExpression.source
        )
      );
      this->lastRuleName = "letters to variable";
    } else {
      // The letters are a keyword.
      secondToLastExpression.syntacticRole =
      this->keyWordsToSyntacticRoles.values[keywordIndex];
      this->lastRuleName = "letters to keyword";
    }
    return true;
  }
  if (
    secondToLastRole == SyntacticElement::DIGITS &&
    lastRole != SyntacticElement::DIGITS &&
    lastRole != SyntacticElement::DOT
  ) {
    secondToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    Rational value;
    value.assignString(secondToLastExpression.source);
    secondToLastExpression.data.assignValue(*this->owner, value);
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "digits to integer";
    }
    return true;
  }
  if (
    lastRole == SyntacticElement::WHITE_SPACE &&
    lastExpression.source == " " &&
    secondToLastRole == SyntacticElement::BACKSLASH &&
    this->syntacticStack->size >= this->numberOfEmptyTokensStart + 2
  ) {
    this->popTopSyntacticStack();
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "[Rule: remove \\ ]";
    }
    return this->popTopSyntacticStack();
  }
  if (
    lastRole == SyntacticElement::WHITE_SPACE ||
    lastRole == SyntacticElement::TILDE
  ) {
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "remove white space";
    }
    return this->popTopSyntacticStack();
  }
  if (
    lastRole == SyntacticElement::LEFT_PARENTHESIS ||
    lastRole == SyntacticElement::LEFT_CURLY_BRACE
  ) {
    (*this->syntacticStack)[(*this->syntacticStack).size - 1].
    totalNonBoundVariablesInherited =
    this->nonBoundVariablesInContext.size;
    (*this->syntacticStack)[(*this->syntacticStack).size - 1].
    totalBoundVariablesInherited =
    this->boundVariablesInContext.size;
  }
  SyntacticElement& fourthToLastExpression = (*this->syntacticStack)[(
      *this->syntacticStack
    ).size -
    4
  ];
  SyntacticElement::Role fourthToLastRole =
  fourthToLastExpression.syntacticRole;
  SyntacticElement& fifthToLastE = (*this->syntacticStack)[(
      *this->syntacticStack
    ).size -
    5
  ];
  SyntacticElement::Role fifthToLastRole = fifthToLastE.syntacticRole;
  SyntacticElement& sixthToLastExpression = (*this->syntacticStack)[(
      *this->syntacticStack
    ).size -
    6
  ];
  SyntacticElement::Role sixthToLastRole = sixthToLastExpression.syntacticRole;
  SyntacticElement& seventhToLastExpression = (*this->syntacticStack)[(
      *this->syntacticStack
    ).size -
    7
  ];
  SyntacticElement& eighthToLastE = (*this->syntacticStack)[(
      *this->syntacticStack
    ).size -
    8
  ];
  SyntacticElement::Role seventhToLastRole =
  seventhToLastExpression.syntacticRole;
  SyntacticElement::Role eighthToLastRole = eighthToLastE.syntacticRole;
  if (
    secondToLastRole == SyntacticElement::PERCENT &&
    lastRole == SyntacticElement::LETTERS &&
    this->modifyableFlags.contains(lastExpression.source)
  ) {
    const CalculatorParser::FlagDescription& flagDescription =
    this->modifyableFlags.getValueNoFail(lastExpression.source);
    *flagDescription.flagLocation = true;
    if (flagDescription.description.empty()) {
      this->owner->comments
      << "User set flag <b style='color:blue'>"
      << lastExpression.source
      << "</b> to true.<br>";
    } else {
      this->owner->comments << flagDescription.description;
    }
    this->lastRuleName = "modifyable flag";
    return this->decreaseStack(2);
  }
  if (
    secondToLastRole == SyntacticElement::PERCENT &&
    lastRole == SyntacticElement::LETTERS &&
    lastExpression.source == "NoLogarithmExponentShortcut"
  ) {
    this->owner->atomsWhoseExponentsAreInterpretedAsFunctions.
    removeFirstOccurenceSwapWithLast(std::string("\\log"));
    this->lastRuleName = "turn off logarithm exponent shortcut";
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (
    thirdToLastRole == SyntacticElement::PERCENT &&
    lastRole == SyntacticElement::SEMICOLON
  ) {
    this->lastRuleName = "remove code comment terminated by semicolon";
    this->popTopSyntacticStack();
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  if (thirdToLastRole == SyntacticElement::PERCENT) {
    this->lastRuleName = "remove code comment";
    return this->popBelowStackTop();
  }
  if (
    secondToLastRole == SyntacticElement::DOUBLE_BACKSLASH && (
      thirdToLastRole == SyntacticElement::COMMA ||
      thirdToLastRole == SyntacticElement::PLUS ||
      thirdToLastRole == SyntacticElement::MINUS
    )
  ) {
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "[Rule: remove \\\\ from X\\\\]";
    }
    return this->popBelowStackTop();
  }
  if (
    secondToLastRole == SyntacticElement::INTEGRAL_SIGN &&
    lastRole == SyntacticElement::EXPRESSION
  ) {
    secondToLastExpression.data.makeAtom(
      *this->owner, this->owner->opIntegral()
    );
    secondToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "integral sign expression to integral expression";
    return true;
  }
  if (
    thirdToLastRole == SyntacticElement::INTEGRAL_SIGN &&
    secondToLastRole == SyntacticElement::EXPRESSION
  ) {
    if (this->flagLogSyntaxRules) {
      this->lastRuleName =
      "integral expression any to expression expression any";
    }
    thirdToLastExpression.data.makeAtom(
      *this->owner, thirdToLastExpression.source
    );
    thirdToLastExpression.syntacticRole = SyntacticElement::INTEGRAL_OPERATOR;
    return true;
  }
  if (
    secondToLastRole == SyntacticElement::INTEGRAL_OPERATOR &&
    lastRole == SyntacticElement::EXPRESSION
  ) {
    secondToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName =
    "integral operator expression to expression expression";
    return true;
  }
  if (
    thirdToLastRole == SyntacticElement::INTEGRAL_OPERATOR &&
    secondToLastRole == SyntacticElement::EXPRESSION
  ) {
    thirdToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName =
    "integral operator expression any to expression expression any";
    return true;
  }
  if (
    this->isDefiniteIntegral(thirdToLastRole) &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    this->canBeRegardedAsDifferentialForm(lastExpression)
  ) {
    thirdToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "definite integral Ed definite_integral Ed";
    return true;
  }
  if ((
      fifthToLastRole == SyntacticElement::INTEGRAL_SIGN ||
      this->isDefiniteIntegral(fifthToLastRole)
    ) &&
    fourthToLastRole == SyntacticElement::EXPRESSION && (
      thirdToLastRole == SyntacticElement::PLUS ||
      thirdToLastRole == SyntacticElement::MINUS
    ) &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    this->canBeRegardedAsDifferentialForm(lastExpression)
  ) {
    this->lastRuleName = "expression plus expression to expression";
    return this->replaceEOEXByEX(this->owner->opPlus());
  }
  if (
    fourthToLastRole == SyntacticElement::INTEGRAL_SIGN &&
    thirdToLastRole == SyntacticElement::UNDERSCORE &&
    secondToLastRole == SyntacticElement::EXPRESSION
  ) {
    Expression integralAtom;
    integralAtom.makeAtom(*this->owner, "\\int");
    fourthToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opUnderscore(),
      integralAtom,
      secondToLastExpression.data
    );
    fourthToLastExpression.syntacticRole =
    SyntacticElement::INTEGRAL_WITH_SUBSCRIPT;
    return this->decreaseStackExceptLast(2);
  }
  if (
    fourthToLastRole == SyntacticElement::INTEGRAL_SIGN &&
    thirdToLastRole == SyntacticElement::UPPER_CARET &&
    secondToLastRole == SyntacticElement::EXPRESSION
  ) {
    Expression integralAtom;
    integralAtom.makeAtom(*this->owner, "\\int");
    fourthToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opPower(),
      integralAtom,
      secondToLastExpression.data
    );
    this->lastRuleName = "\\int^{*}";
    fourthToLastExpression.syntacticRole =
    SyntacticElement::INTEGRAL_WITH_SUPERSCRIPT;
    return this->decreaseStackExceptLast(2);
  }
  if (
    thirdToLastRole == SyntacticElement::INTEGRAL_WITH_SUBSCRIPT &&
    secondToLastRole == SyntacticElement::UPPER_CARET &&
    lastRole == SyntacticElement::EXPRESSION
  ) {
    thirdToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opPower(),
      thirdToLastExpression.data,
      lastExpression.data
    );
    thirdToLastExpression.syntacticRole = SyntacticElement::INTEGRAL_OPERATOR;
    this->lastRuleName = "int_{*}^{**}";
    return this->decreaseStack(2);
  }
  if (
    fourthToLastRole == SyntacticElement::INTEGRAL_WITH_SUBSCRIPT &&
    thirdToLastRole == SyntacticElement::UPPER_CARET &&
    secondToLastRole == SyntacticElement::EXPRESSION
  ) {
    fourthToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opPower(),
      fourthToLastExpression.data,
      secondToLastExpression.data
    );
    fourthToLastExpression.syntacticRole = SyntacticElement::INTEGRAL_OPERATOR;
    this->lastRuleName = "int_{*} ^ {**} to int_{*}^{**}";
    return this->decreaseStackExceptLast(2);
  }
  if (
    thirdToLastRole == SyntacticElement::INTEGRAL_WITH_SUPERSCRIPT &&
    secondToLastRole == SyntacticElement::UNDERSCORE &&
    lastRole == SyntacticElement::EXPRESSION
  ) {
    thirdToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opUnderscore(),
      thirdToLastExpression.data,
      lastExpression.data
    );
    this->lastRuleName = "[\\int^{*}_{**}]";
    thirdToLastExpression.syntacticRole = SyntacticElement::INTEGRAL_OPERATOR;
    return this->decreaseStack(2);
  }
  if (
    fourthToLastRole == SyntacticElement::INTEGRAL_WITH_SUPERSCRIPT &&
    thirdToLastRole == SyntacticElement::UNDERSCORE &&
    secondToLastRole == SyntacticElement::EXPRESSION
  ) {
    fourthToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opUnderscore(),
      fourthToLastExpression.data,
      secondToLastExpression.data
    );
    this->lastRuleName = "\\int^{*} _ Expression to \\int^{*}_{**}";
    fourthToLastExpression.syntacticRole = SyntacticElement::INTEGRAL_OPERATOR;
    return this->decreaseStackExceptLast(2);
  }
  if (!this->owner->flagDontUsePredefinedWordSplits) {
    if (lastRole == SyntacticElement::LETTERS) {
      const std::string& currentVariable = lastExpression.source;
      bool replaced =
      this->replaceVbyVdotsVAccordingToPredefinedWordSplits(currentVariable);
      if (replaced) {
        return true;
      }
    }
  }
  if (
    secondToLastRole == SyntacticElement::EQUALS &&
    lastRole == SyntacticElement::COLON
  ) {
    secondToLastExpression.syntacticRole = SyntacticElement::EQUALS_COLON;
    secondToLastExpression.source = "=:";
    this->lastRuleName = "equal colon to equalsColon";
    return this->popTopSyntacticStack();
  }
  if (
    secondToLastRole == SyntacticElement::LEFT_CURLY_BRACE &&
    lastRole == SyntacticElement::RIGHT_CURLY_BRACE
  ) {
    secondToLastExpression.syntacticRole =
    SyntacticElement::LEFT_RIGHT_CURLY_BRACE;
    this->lastRuleName = "{} to left-right curly brace";
    return this->popTopSyntacticStack();
  }
  if (lastRole == SyntacticElement::CDOT) {
    lastExpression.syntacticRole = SyntacticElement::STAR;
    this->lastRuleName = "cdot to star";
    return true;
  }
  if (lastExpression.source == "\\varnothing") {
    lastExpression.data.makeAtom(*this->owner, this->owner->opEmptySet());
    lastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "varnothing to atom";
    return true;
  }
  if (
    lastRole == SyntacticElement::CIRC &&
    secondToLastRole != SyntacticElement::LEFT_CURLY_BRACE &&
    secondToLastRole != SyntacticElement::LEFT_PARENTHESIS &&
    secondToLastRole != SyntacticElement::UPPER_CARET
  ) {
    secondToLastExpression.syntacticRole =
    SyntacticElement::LEFT_RIGHT_CURLY_BRACE;
    this->lastRuleName = "circ to apply function";
    return true;
  }
  if (
    fifthToLastRole == SyntacticElement::LEFT_CURLY_BRACE &&
    fourthToLastRole == SyntacticElement::LEFT_CURLY_BRACE &&
    thirdToLastRole == SyntacticElement::VARIABLE &&
    secondToLastRole == SyntacticElement::RIGHT_CURLY_BRACE &&
    lastRole == SyntacticElement::RIGHT_CURLY_BRACE
  ) {
    this->lastRuleName = "{{variable}} to bound variable ";
    return this->replaceXXVXdotsXbyE_BOUND_XdotsX(2);
  }
  if (
    secondToLastRole == SyntacticElement::FONT_MODIFIER && (
      lastRole == SyntacticElement::VARIABLE ||
      lastRole == SyntacticElement::LETTERS
    ) &&
    lastExpression.source.size() < 10 &&
    lastExpression.source.size() > 0
  ) {
    secondToLastExpression.data.makeAtom(
      *this->owner,
      secondToLastExpression.source + "{" + lastExpression.source + "}"
    );
    secondToLastExpression.source = "";
    secondToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "mathbb variable to expression";
    return this->decreaseStack(1);
  }
  if (
    fourthToLastRole == SyntacticElement::FONT_MODIFIER &&
    thirdToLastRole == SyntacticElement::LEFT_CURLY_BRACE && (
      secondToLastRole == SyntacticElement::VARIABLE ||
      secondToLastRole == SyntacticElement::LETTERS
    ) &&
    lastRole == SyntacticElement::RIGHT_CURLY_BRACE &&
    secondToLastExpression.source.size() < 10 &&
    secondToLastExpression.source.size() > 0
  ) {
    fourthToLastExpression.data.makeAtom(
      *this->owner,
      fourthToLastExpression.source + "{" + secondToLastExpression.source + "}"
    );
    fourthToLastExpression.source = "";
    fourthToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "mathbf {variable} to expression";
    return this->decreaseStack(3);
  }
  if (
    secondToLastRole == SyntacticElement::VARIABLE && ((
        lastRole != SyntacticElement::RIGHT_CURLY_BRACE &&
        lastRole != SyntacticElement::WHITE_SPACE
      ) ||
      thirdToLastRole != SyntacticElement::LEFT_CURLY_BRACE ||
      fourthToLastRole != SyntacticElement::LEFT_CURLY_BRACE
    )
  ) {
    this->lastRuleName = "variable Y to expression Y";
    return this->replaceVXdotsXbyE_NONBOUND_XdotsX(1);
  }
  if (
    thirdToLastRole == SyntacticElement::VARIABLE && ((
        secondToLastRole != SyntacticElement::RIGHT_CURLY_BRACE &&
        secondToLastRole != SyntacticElement::WHITE_SPACE
      ) ||
      fourthToLastRole != SyntacticElement::LEFT_CURLY_BRACE ||
      fifthToLastRole != SyntacticElement::LEFT_CURLY_BRACE
    )
  ) {
    this->lastRuleName = "variable XY -> expression XY";
    return this->replaceVXdotsXbyE_NONBOUND_XdotsX(2);
  }
  if (
    fourthToLastRole == SyntacticElement::LEFT_CURLY_BRACE &&
    thirdToLastRole == SyntacticElement::VARIABLE &&
    secondToLastRole == SyntacticElement::RIGHT_CURLY_BRACE &&
    lastRole != SyntacticElement::RIGHT_CURLY_BRACE &&
    lastRole != SyntacticElement::WHITE_SPACE
  ) {
    this->lastRuleName = "{variable}}X -> {expression}}X";
    return this->replaceVXdotsXbyE_NONBOUND_XdotsX(2);
  }
  if ((
      secondToLastRole == SyntacticElement::LEFT_SEPARATOR ||
      secondToLastRole == SyntacticElement::RIGHT_SEPARATOR
    ) &&
    lastRole == SyntacticElement::PIPE
  ) {
    this->lastRuleName = "left or right pipe to pipe";
    return this->popBelowStackTop();
  }
  if (
    secondToLastRole == SyntacticElement::LEFT_SEPARATOR && (
      lastRole == SyntacticElement::LEFT_PARENTHESIS ||
      lastRole == SyntacticElement::LEFT_BRACKET ||
      lastRole == SyntacticElement::LEFT_CURLY_BRACE
    )
  ) {
    this->lastRuleName = "left delimiter to delimiter";
    return this->popBelowStackTop();
  }
  if (
    secondToLastRole == SyntacticElement::RIGHT_SEPARATOR && (
      lastRole == SyntacticElement::RIGHT_PARENTHESIS ||
      lastRole == SyntacticElement::RIGHT_BRACKET ||
      lastRole == SyntacticElement::RIGHT_CURLY_BRACE
    )
  ) {
    this->lastRuleName = "right delimiter to delimiter ";
    return this->popBelowStackTop();
  }
  if (
    lastRole == SyntacticElement::EQUALS &&
    secondToLastRole == SyntacticElement::EQUALS
  ) {
    this->lastRuleName = "equals equals to equalsEquals";
    secondToLastExpression.syntacticRole = SyntacticElement::EQUALS_EQUALS;
    return this->popTopSyntacticStack();
  }
  if (
    secondToLastRole == SyntacticElement::LEFT_PARENTHESIS &&
    lastRole == SyntacticElement::RIGHT_PARENTHESIS
  ) {
    secondToLastExpression.data.reset(*this->owner, 1);
    secondToLastExpression.data.addChildAtomOnTop(this->owner->opSequence());
    secondToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "left right parentheses to empty sequence";
    return this->decreaseStackSetCharacterRanges(1);
  }
  if (
    secondToLastRole == SyntacticElement::TEXT && (
      lastRole == SyntacticElement::LETTERS ||
      lastRole == SyntacticElement::VARIABLE ||
      lastRole == SyntacticElement::EXPRESSION
    ) &&
    lastExpression.data.isOperationGiven("d")
  ) {
    secondToLastExpression.data.makeAtom(
      *this->owner, this->owner->opDifferential()
    );
    secondToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "\\text d to differential";
    }
    return this->decreaseStackSetCharacterRanges(1);
  }
  if (
    thirdToLastRole == SyntacticElement::TEXT &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    secondToLastExpression.data.isOperationGiven("d")
  ) {
    thirdToLastExpression.data.makeAtom(
      *this->owner, this->owner->opDifferential()
    );
    thirdToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "text expression any to expression any";
    }
    return this->decreaseStackExceptLast(1);
  }
  if (
    thirdToLastRole == SyntacticElement::DIGITS &&
    secondToLastRole == SyntacticElement::DOT &&
    lastRole == SyntacticElement::DIGITS
  ) {
    this->rationalFromIntegerDotInteger(
      thirdToLastExpression.source,
      lastExpression.source,
      thirdToLastExpression.data
    );
    thirdToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    thirdToLastExpression.source = "";
    this->lastRuleName = "digits dot digits to rational";
    this->popTopSyntacticStack();
    return this->popTopSyntacticStack();
  }
  // There is an ambiguity on how should function application be associated.
  // Which is better: x{}y{}z = x{} (y{}z), or x{}y{}z = (x{}y){}z ?
  // In our implementation, we choose x{}y{}z= x{} (y{}z). Although this is
  // slightly harder to implement,
  // it appears to be the more natural choice.
  if (
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::LEFT_RIGHT_CURLY_BRACE &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsApplyFunctionInPreceding(lastRole) &&
    !fourthToLastExpression.data.isTypeAtom()
  ) {
    this->lastRuleName = "e circ e any to e of e any";
    Expression result(*this->owner);
    result.addChildOnTop(fourthToLastExpression.data);
    result.addChildOnTop(secondToLastExpression.data);
    fourthToLastExpression.data = result;
    fourthToLastExpression.source = "";
    return this->decreaseStackExceptLast(2);
  }
  // end of ambiguity.
  if (
    fifthToLastRole == SyntacticElement::LIM &&
    fourthToLastRole == SyntacticElement::UNDERSCORE &&
    thirdToLastRole == SyntacticElement::EXPRESSION &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsLimitProcessInPreceding(lastRole)
  ) {
    fifthToLastE.data.makeXOX(
      *this->owner,
      this->owner->opLimit(),
      thirdToLastExpression.data,
      secondToLastExpression.data
    );
    fifthToLastE.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "limit _ expression expression to expression";
    return this->decreaseStackExceptLast(3);
  }
  if (
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::UNDERSCORE &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    lastRole != SyntacticElement::UNDERSCORE
  ) {
    this->lastRuleName = "expression _ expression X to expression X";
    return this->replaceEOEXByEX(this->owner->opUnderscore());
  }
  if (
    fourthToLastRole == SyntacticElement::LEFT_CURLY_BRACE &&
    thirdToLastRole == SyntacticElement::LEFT_RIGHT_CURLY_BRACE &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    lastRole == SyntacticElement::RIGHT_CURLY_BRACE
  ) {
    return this->replaceXYYXByYY();
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION &&
    lastRole == SyntacticElement::FACTORIAL
  ) {
    secondToLastExpression.data.makeOX(
      *this->owner, this->owner->opFactorial(), secondToLastExpression.data
    );
    this->lastRuleName = "x factorial";
    return this->popTopSyntacticStack();
  }
  if (
    thirdToLastRole == SyntacticElement::BINOM &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    lastRole == SyntacticElement::EXPRESSION
  ) {
    thirdToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opBinom(),
      secondToLastExpression.data,
      lastExpression.data
    );
    thirdToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "[Rule: Calculator::replaceXEEBy_OofEE]";
    }
    return this->decreaseStackSetCharacterRanges(2);
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::UPPER_CARET &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    !this->owner->knownOperationsInterpretedAsFunctionsMultiplicatively.
    contains(fourthToLastExpression.source) &&
    this->allowsPowerInPreceding(lastRole) &&
    this->allowsPowerInNext(fifthToLastRole)
  ) {
    this->lastRuleName = "expression ^ expression X to expression X";
    return this->replaceEOEXByEX(this->owner->opPower());
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::SET_MINUS &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsPowerInPreceding(lastRole)
  ) {
    this->lastRuleName = "expression \\setminus expression to expression";
    return this->replaceEOEXByEX(this->owner->opSetMinus());
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::OR &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsOrInPreceding(lastRole)
  ) {
    this->lastRuleName = "expression or expression to expression";
    return this->replaceEOEXByEX(this->owner->opOr());
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::AND &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsAndInPreceding(lastRole)
  ) {
    this->lastRuleName = "expression and expression to expression";
    return this->replaceEOEXByEX(this->owner->opAnd());
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::PLUS &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsPlusInPreceding(lastRole)
  ) {
    this->lastRuleName = "expression + expression X to expression X";
    return this->replaceEOEXByEX(this->owner->opPlus());
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::OPLUS &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsPlusInPreceding(lastRole)
  ) {
    this->lastRuleName = "expression \\oplus expression X to expression X";
    return this->replaceEOEXByEX(this->owner->opDirectSum());
  }
  if (
    fifthToLastRole == SyntacticElement::PIPE &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::PLUS &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    lastRole == SyntacticElement::PIPE
  ) {
    this->lastRuleName = "|expression + expression| to |expression|";
    return this->replaceEOEXByEX(this->owner->opPlus());
  }
  if (
    fifthToLastRole == SyntacticElement::EXPRESSION &&
    fourthToLastRole == SyntacticElement::UPPER_CARET &&
    thirdToLastRole == SyntacticElement::MINUS &&
    secondToLastRole == SyntacticElement::EXPRESSION && (
      lastRole == SyntacticElement::EXPRESSION ||
      lastRole == SyntacticElement::VARIABLE ||
      lastRole == SyntacticElement::LETTERS
    )
  ) {
    if (this->flagLogSyntaxRules) {
      this->lastRuleName =
      "expression power minus expression x to expression x";
    }
    Expression exponent;
    exponent.makeOX(
      *this->owner, this->owner->opMinus(), secondToLastExpression.data
    );
    fifthToLastE.data.makeXOX(
      *this->owner, this->owner->opPower(), fifthToLastE.data, exponent
    );
    return this->decreaseStackExceptLast(3);
  }
  if (
    thirdToLastRole == SyntacticElement::MINUS &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsPlusInPreceding(lastRole)
  ) {
    this->lastRuleName = "expression - expression X to expression X";
    return this->replaceEOEXByEX(this->owner->opMinus());
  }
  if (
    thirdToLastRole == SyntacticElement::MINUS &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    lastRole == SyntacticElement::PIPE &&
    fifthToLastRole == SyntacticElement::PIPE
  ) {
    this->lastRuleName = "|expression - expression| to |expression|";
    return this->replaceEOEXByEX(this->owner->opMinus());
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::MINUS &&
    this->allowsPlusInPreceding(lastRole)
  ) {
    thirdToLastExpression.data.makeOX(
      *this->owner, this->owner->opMinus(), secondToLastExpression.data
    );
    thirdToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "minus expression to minusexpression";
    return this->decreaseStackExceptLast(1);
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::MINUS &&
    lastRole == SyntacticElement::PIPE &&
    fourthToLastRole == SyntacticElement::PIPE
  ) {
    thirdToLastExpression.data.makeOX(
      *this->owner, this->owner->opMinus(), secondToLastExpression.data
    );
    thirdToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "|- expression| to |expression|";
    return this->decreaseStackExceptLast(1);
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::MOD &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsTimesInPreceding(lastExpression)
  ) {
    this->lastRuleName = "expression mod expression X to expression X";
    return this->replaceEXEXBy_OofEE_X(this->owner->opMod());
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::IN &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsInInPreceding(lastRole)
  ) {
    fourthToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opIn(),
      fourthToLastExpression.data,
      secondToLastExpression.data
    );
    fourthToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "expression in expression";
    return this->decreaseStackExceptLast(2);
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::CHOOSE &&
    fourthToLastRole == SyntacticElement::EXPRESSION
  ) {
    return this->replaceEXEXBy_OofEE_X(this->owner->opBinom());
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::TENSOR_PRODUCT &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsTensorInPreceding(lastExpression)
  ) {
    fourthToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opTensor(),
      fourthToLastExpression.data,
      secondToLastExpression.data
    );
    fourthToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "tensor product";
    return this->decreaseStackExceptLast(2);
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::EXPRESSION &&
    fourthToLastRole == SyntacticElement::FRAC
  ) {
    fourthToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opDivide(),
      thirdToLastExpression.data,
      secondToLastExpression.data
    );
    fourthToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "fraction";
    return this->decreaseStackExceptLast(2);
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::TIMES_X_SIGN &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsTimesInPreceding(lastExpression)
  ) {
    fourthToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opCrossProduct(),
      fourthToLastExpression.data,
      secondToLastExpression.data
    );
    fourthToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "cross product";
    return this->decreaseStackExceptLast(2);
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION && (
      thirdToLastRole == SyntacticElement::STAR ||
      thirdToLastRole == SyntacticElement::CDOT
    ) &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsTimesInPreceding(secondToLastExpression, lastExpression)
  ) {
    fourthToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opTimes(),
      fourthToLastExpression.data,
      secondToLastExpression.data
    );
    fourthToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "expression times expression";
    return this->decreaseStackExceptLast(2);
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::DIVISION_SIGN &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsDivideInPreceding(lastExpression)
  ) {
    fourthToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opDivide(),
      fourthToLastExpression.data,
      secondToLastExpression.data
    );
    fourthToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "expression divided by expression";
    return this->decreaseStackExceptLast(2);
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::TO &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsLimitProcessInPreceding(lastRole)
  ) {
    this->lastRuleName = "expression \\to expression X to expression X";
    return this->replaceEOEXByEX(this->owner->opLimitProcess());
  }
  if (
    this->owner->atomsWhoseExponentsAreInterpretedAsFunctions.contains(
      fifthToLastE.source
    ) &&
    fourthToLastRole == SyntacticElement::UPPER_CARET &&
    thirdToLastRole == SyntacticElement::EXPRESSION &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsTimesInPreceding(lastExpression)
  ) {
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "triglike power expression expression";
    }
    Expression trigLikeFunction;
    trigLikeFunction.makeAtom(*this->owner, fifthToLastE.source);
    Expression base(*this->owner);
    base.addChildOnTop(trigLikeFunction);
    base.addChildOnTop(secondToLastExpression.data);
    fifthToLastE.data.makeXOX(
      *this->owner, this->owner->opPower(), base, thirdToLastExpression.data
    );
    fifthToLastE.source = "";
    fifthToLastE.syntacticRole = SyntacticElement::EXPRESSION;
    return this->decreaseStackExceptLast(3);
  }
  if (
    fifthToLastRole == SyntacticElement::LOGARITHM_BASE &&
    fourthToLastRole == SyntacticElement::UNDERSCORE &&
    thirdToLastRole == SyntacticElement::EXPRESSION &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsTimesInPreceding(lastExpression)
  ) {
    fifthToLastE.data.makeXOX(
      *this->owner,
      this->owner->opLogBase(),
      thirdToLastExpression.data,
      secondToLastExpression.data
    );
    fifthToLastE.syntacticRole = SyntacticElement::EXPRESSION;
    fifthToLastE.source = "";
    this->lastRuleName = "logarithm _ expression expression to expression";
    return this->decreaseStackExceptLast(3);
  }
  if (
    seventhToLastRole == SyntacticElement::SQRT &&
    sixthToLastRole == SyntacticElement::LEFT_BRACKET &&
    fifthToLastRole == SyntacticElement::EXPRESSION &&
    fourthToLastRole == SyntacticElement::RIGHT_BRACKET &&
    thirdToLastRole == SyntacticElement::EXPRESSION
  ) {
    seventhToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opSqrt(),
      fifthToLastE.data,
      thirdToLastExpression.data
    );
    seventhToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "sqrt [n] {e} to nth root of e";
    return this->decreaseStackExceptLastTwo(4);
  }
  if (
    this->allowsTimesInNext(fourthToLastExpression) &&
    thirdToLastRole == SyntacticElement::EXPRESSION &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsTimesInPreceding(secondToLastExpression, lastExpression)
  ) {
    if (this->flagLogSyntaxRules) {
      this->lastRuleName =
      "(expression expression any) to (expression times expression any)";
    }
    thirdToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opTimes(),
      thirdToLastExpression.data,
      secondToLastExpression.data
    );
    Expression impliedFunctionApplication;
    if (
      CalculatorBasics::timesToFunctionApplication(
        *this->owner, thirdToLastExpression.data, impliedFunctionApplication
      )
    ) {
      thirdToLastExpression.data = impliedFunctionApplication;
      if (this->flagLogSyntaxRules) {
        this->lastRuleName =
        "(expression expression any) to (expression of expression any)";
      }
    }
    thirdToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    thirdToLastExpression.source = "";
    return this->decreaseStackExceptLast(1);
  }
  if ((lastExpression.source == ">" && secondToLastExpression.source == "=") ||
    (lastExpression.source == "=" && secondToLastExpression.source == ">")
  ) {
    secondToLastExpression.syntacticRole = SyntacticElement::GREATER_THAN_LIKE;
    secondToLastExpression.source = "\\geq";
    this->lastRuleName = "equals greater than to geq";
    return this->popTopSyntacticStack();
  }
  if (
    thirdToLastRole == SyntacticElement::PIPE &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    lastRole == SyntacticElement::PIPE
  ) {
    thirdToLastExpression.data.makeOX(
      *this->owner, this->owner->opAbsoluteValue(), secondToLastExpression.data
    );
    thirdToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "| expression | to expression";
    return this->decreaseStackSetCharacterRanges(2);
  }
  if (
    thirdToLastRole == SyntacticElement::LEFT_PARENTHESIS &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    lastRole == SyntacticElement::RIGHT_PARENTHESIS
  ) {
    thirdToLastExpression.data = secondToLastExpression.data;
    thirdToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "( expression ) to expression";
    return this->decreaseStackSetCharacterRanges(2);
  }
  if (
    thirdToLastRole == SyntacticElement::LEFT_CURLY_BRACE &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    lastRole == SyntacticElement::RIGHT_CURLY_BRACE
  ) {
    thirdToLastExpression.data = secondToLastExpression.data;
    thirdToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "{ expression } to expression";
    return this->decreaseStackSetCharacterRanges(2);
  }
  if (
    fourthToLastRole == SyntacticElement::LEFT_CURLY_BRACE &&
    thirdToLastRole == SyntacticElement::EXPRESSION &&
    secondToLastRole == SyntacticElement::RIGHT_CURLY_BRACE
  ) {
    fourthToLastExpression.data = secondToLastExpression.data;
    fourthToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "{ expression } any to expression any";
    return this->decreaseStackExceptLast(2);
  }
  if (
    thirdToLastRole == SyntacticElement::LEFT_PARENTHESIS &&
    secondToLastRole == SyntacticElement::CIRC &&
    lastRole == SyntacticElement::RIGHT_PARENTHESIS
  ) {
    secondToLastExpression.data.makeAtom(*this->owner, this->owner->opCirc());
    secondToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "( circ sign ) to ( circ )";
    return true;
  }
  if (
    thirdToLastRole == SyntacticElement::LEFT_CURLY_BRACE &&
    secondToLastRole == SyntacticElement::CIRC &&
    lastRole == SyntacticElement::RIGHT_CURLY_BRACE
  ) {
    secondToLastExpression.data.makeAtom(*this->owner, this->owner->opCirc());
    secondToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "{ circ sign } to { circ }";
    return true;
  }
  if (
    secondToLastRole == SyntacticElement::UPPER_CARET && (
      lastRole == SyntacticElement::CIRC || (
        lastRole == SyntacticElement::LETTERS &&
        lastExpression.source == "\\circ"
      )
    )
  ) {
    lastExpression.data.makeAtom(*this->owner, this->owner->opCirc());
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "^ \\circ to ^ atom(circ)";
    }
    lastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    return true;
  }
  if (this->owner->flagUseBracketsForIntervals) {
    if (
      thirdToLastRole == SyntacticElement::LEFT_PARENTHESIS &&
      secondToLastRole == SyntacticElement::EXPRESSION &&
      lastRole == SyntacticElement::RIGHT_BRACKET
    ) {
      return
      this->replaceXEXByEContainingOE(this->owner->opIntervalRightClosed());
    }
    if (
      thirdToLastRole == SyntacticElement::LEFT_BRACKET &&
      secondToLastRole == SyntacticElement::EXPRESSION &&
      lastRole == SyntacticElement::RIGHT_PARENTHESIS
    ) {
      return
      this->replaceXEXByEContainingOE(this->owner->opIntervalLeftClosed());
    }
    if (
      thirdToLastRole == SyntacticElement::LEFT_BRACKET &&
      secondToLastRole == SyntacticElement::EXPRESSION &&
      lastRole == SyntacticElement::RIGHT_BRACKET
    ) {
      return this->replaceXEXByEContainingOE(this->owner->opIntervalClosed());
    }
    if (secondToLastExpression.data.isSequenceNElements(2)) {
      if (
        this->isSeparatorFromTheLeftForInterval(fourthToLastRole) &&
        thirdToLastRole == SyntacticElement::LEFT_PARENTHESIS &&
        secondToLastRole == SyntacticElement::SEQUENCE &&
        lastRole == SyntacticElement::RIGHT_PARENTHESIS
      ) {
        return this->replaceXEXByEContainingOE(this->owner->opIntervalOpen());
      }
    }
  }
  if (
    secondToLastRole == SyntacticElement::EQUALS_EQUALS &&
    lastRole == SyntacticElement::EQUALS
  ) {
    this->lastRuleName = "double equals to triple equals";
    secondToLastExpression.syntacticRole =
    SyntacticElement::EQUALS_EQUALS_EQUALS;
    secondToLastExpression.source = "==";
    return this->popTopSyntacticStack();
  }
  if (
    this->isSeparatorFromTheRightGeneral(lastRole) &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::EQUALS_EQUALS_EQUALS &&
    fourthToLastRole == SyntacticElement::EXPRESSION
  ) {
    this->lastRuleName = "expression === expression X to expression X";
    return this->replaceEOEXByEX(this->owner->opEqualEqualEqual());
  }
  if (
    this->isSeparatorFromTheLeftForDefinition(fifthToLastRole) &&
    this->isSeparatorFromTheRightForDefinition(lastRole) &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::EQUALS_EQUALS &&
    fourthToLastRole == SyntacticElement::EXPRESSION
  ) {
    this->lastRuleName = "expression == expression X to expression X";
    return this->replaceEOEXByEX(this->owner->opEqualEqual());
  }
  if (
    this->isSeparatorFromTheLeftForDefinition(fifthToLastRole) &&
    this->isSeparatorFromTheRightForDefinition(lastRole) &&
    secondToLastRole == SyntacticElement::EXPRESSION && (
      thirdToLastRole == SyntacticElement::GREATER_THAN_LIKE ||
      thirdToLastRole == SyntacticElement::LESS_THAN_LIKE
    ) &&
    fourthToLastRole == SyntacticElement::EXPRESSION
  ) {
    this->lastRuleName = "expression > expression X to expression X";
    int operation =
    this->owner->operations.getIndexNoFail(thirdToLastExpression.source);
    return this->replaceEOEXByEX(operation);
  }
  if (
    this->isSeparatorFromTheLeftForDefinition(fifthToLastRole) &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::EQUALS &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    this->isSeparatorFromTheRightForDefinition(lastRole)
  ) {
    this->lastRuleName = "expression = expression X to expression X";
    return this->replaceEOEXByEX(this->owner->opDefine());
  }
  if (
    this->isSeparatorFromTheLeftForDefinition(fifthToLastRole) &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::EQUALS_COLON &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    this->isSeparatorFromTheRightForDefinition(lastRole)
  ) {
    fourthToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opIsDenotedBy(),
      fourthToLastExpression.data,
      secondToLastExpression.data
    );
    fourthToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "y =: x to isDenotedBy";
    return this->decreaseStackExceptLast(2);
  }
  if (
    lastRole == SyntacticElement::SEQUENCE &&
    lastExpression.data.size() == 0 &&
    lastExpression.data.data == this->owner->opList()
  ) {
    lastExpression.syntacticRole = SyntacticElement::MAKE_SEQUENCE;
    this->lastRuleName = "sequence to makesequence";
    return true;
  }
  if (
    thirdToLastRole == SyntacticElement::SQRT &&
    secondToLastRole == SyntacticElement::EXPRESSION
  ) {
    thirdToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opSqrt(),
      this->owner->expressionTwo(),
      secondToLastExpression.data
    );
    thirdToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "sqrt of expression";
    return this->decreaseStackExceptLast(1);
  }
  if (
    fourthToLastRole == SyntacticElement::SQRT &&
    thirdToLastRole == SyntacticElement::EXPRESSION
  ) {
    fourthToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opSqrt(),
      this->owner->expressionTwo(),
      thirdToLastExpression.data
    );
    fourthToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "sqrt of expression any";
    return this->decreaseStackExceptLastTwo(1);
  }
  if (
    fourthToLastRole == SyntacticElement::SQRT &&
    thirdToLastRole == SyntacticElement::LEFT_RIGHT_CURLY_BRACE &&
    secondToLastRole == SyntacticElement::EXPRESSION
  ) {
    this->lastRuleName = "sqrt any e B by sqrt of e B";
    fourthToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opSqrt(),
      this->owner->expressionTwo(),
      secondToLastExpression.data
    );
    fourthToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    return this->decreaseStackExceptLast(2);
  }
  if (
    sixthToLastRole == SyntacticElement::SQRT &&
    fifthToLastRole == SyntacticElement::LEFT_BRACKET &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::RIGHT_BRACKET &&
    secondToLastRole == SyntacticElement::EXPRESSION
  ) {
    sixthToLastExpression.data.makeXOX(
      *this->owner,
      this->owner->opSqrt(),
      fourthToLastExpression.data,
      secondToLastExpression.data
    );
    sixthToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "sqrt [expression] expression to nth root";
    return this->decreaseStackExceptLast(4);
  }
  if (
    this->owner->knownOperationsInterpretedAsFunctionsMultiplicatively.contains
    (thirdToLastExpression.source) &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsTimesInPreceding(lastExpression)
  ) {
    Expression operation;
    operation.makeAtom(*this->owner, thirdToLastExpression.source);
    thirdToLastExpression.data.reset(*this->owner);
    thirdToLastExpression.data.addChildOnTop(operation);
    thirdToLastExpression.data.addChildOnTop(secondToLastExpression.data);
    thirdToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    thirdToLastExpression.source = "";
    if (this->flagLogSyntaxRules) {
      this->lastRuleName =
      "known operation expression to known operation of expression";
    }
    return this->decreaseStackExceptLast(1);
  }
  if (
    this->owner->knownOperationsInterpretedAsFunctionsMultiplicatively.contains
    (fourthToLastExpression.source) &&
    thirdToLastRole == SyntacticElement::UPPER_CARET &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    this->isSeparatorFromTheRightGeneral(lastRole)
  ) {
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "[Rule: Calculator::replaceC1C2Eby_C2ofC1E]";
    }
    Expression newExpression;
    newExpression.reset(*this->owner, 3);
    newExpression.addChildAtomOnTop(this->owner->opPower());
    newExpression.addChildAtomOnTop(fourthToLastExpression.source);
    newExpression.addChildOnTop(secondToLastExpression.data);
    fourthToLastExpression.data = newExpression;
    fourthToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    return this->decreaseStackExceptLast(2);
  }
  if (
    this->owner->knownOperationsInterpretedAsFunctionsMultiplicatively.contains
    (fourthToLastExpression.source) &&
    thirdToLastRole == SyntacticElement::LEFT_RIGHT_CURLY_BRACE &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsApplyFunctionInPreceding(lastRole)
  ) {
    fourthToLastExpression.data.reset(*this->owner);
    fourthToLastExpression.data.addChildAtomOnTop(
      fourthToLastExpression.source
    );
    fourthToLastExpression.data.addChildOnTop(secondToLastExpression.data);
    fourthToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    fourthToLastExpression.source = "";
    this->lastRuleName = "known operator expression to operator of expression";
    return this->decreaseStackExceptLast(2);
  }
  if (
    this->owner->knownOperationsInterpretedAsFunctionsMultiplicatively.contains
    (secondToLastExpression.source) &&
    secondToLastExpression.syntacticRole != SyntacticElement::EXPRESSION &&
    this->isSeparatorFromTheRightGeneral(lastRole)
  ) {
    secondToLastExpression.data.makeAtom(
      *this->owner, secondToLastExpression.source
    );
    secondToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName = "known operation to atom ";
    secondToLastExpression.source = "";
    return true;
  }
  if (
    fourthToLastRole == SyntacticElement::LOGARITHM &&
    thirdToLastRole == SyntacticElement::UNDERSCORE &&
    secondToLastRole == SyntacticElement::EXPRESSION
  ) {
    fourthToLastExpression.syntacticRole = SyntacticElement::LOGARITHM_BASE;
    this->lastRuleName = "logarithm to logarithm base";
    return true;
  }
  if (lastRole == SyntacticElement::LETTERS) {
    int autocorrectionIndex =
    CalculatorParser::keyWordAutocorrections.getIndex(lastExpression.source);
    if (autocorrectionIndex >= 0) {
      lastExpression.source =
      this->keyWordAutocorrections.values[autocorrectionIndex];
      this->lastRuleName = "keyword autocorrection";
      return true;
    }
  }
  if (
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::CUP &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsTimesInPreceding(lastExpression)
  ) {
    this->lastRuleName = "expression \\cup expression X to expression X";
    return this->replaceEOEXByEX(this->owner->opUnion());
  }
  if (
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::CAP &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsTimesInPreceding(lastExpression)
  ) {
    this->lastRuleName = "expression \\cap expression X to expression X";
    return this->replaceEOEXByEX(this->owner->opIntersection());
  }
  if (
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::SQ_CUP &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsTimesInPreceding(lastExpression)
  ) {
    this->lastRuleName = "expression \\sqcup expression X to expression X";
    return this->replaceEOEXByEX(this->owner->opUnionNoRepetition());
  }
  if (
    thirdToLastRole == SyntacticElement::IF &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    this->allowsIfInPreceding(lastRole)
  ) {
    return this->replaceOXXByEXX();
  }
  if (
    fourthToLastRole == SyntacticElement::SEQUENCE &&
    thirdToLastRole == SyntacticElement::COMMA &&
    secondToLastRole == SyntacticElement::EXPRESSION && (
      lastRole == SyntacticElement::COMMA ||
      lastRole == SyntacticElement::RIGHT_PARENTHESIS ||
      lastRole == SyntacticElement::RIGHT_CURLY_BRACE || (
        this->owner->flagUseBracketsForIntervals &&
        lastRole == SyntacticElement::RIGHT_BRACKET
      )
    )
  ) {
    fourthToLastExpression.data.addChildOnTop(secondToLastExpression.data);
    return this->decreaseStackExceptLast(2);
  }
  if ((
      fourthToLastRole != SyntacticElement::MAKE_SEQUENCE ||
      thirdToLastRole != SyntacticElement::LEFT_RIGHT_CURLY_BRACE
    ) && (
      thirdToLastRole != SyntacticElement::LEFT_BRACKET ||
      this->owner->flagUseBracketsForIntervals
    ) &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    lastRole == SyntacticElement::COMMA
  ) {
    secondToLastExpression.data.makeOX(
      *this->owner, this->owner->opSequence(), secondToLastExpression.data
    );
    secondToLastExpression.syntacticRole = SyntacticElement::SEQUENCE;
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "expression comma to sequence";
    }
    return true;
  }
  if (
    thirdToLastRole == SyntacticElement::MAKE_SEQUENCE &&
    secondToLastRole == SyntacticElement::LEFT_RIGHT_CURLY_BRACE &&
    lastRole == SyntacticElement::EXPRESSION
  ) {
    thirdToLastExpression.data.makeOX(
      *this->owner, this->owner->opSequence(), lastExpression.data
    );
    thirdToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "makeSequence {} expression to expression";
    }
    return this->decreaseStackSetCharacterRanges(2);
  }
  if (
    fourthToLastRole == SyntacticElement::MAKE_SEQUENCE &&
    thirdToLastRole == SyntacticElement::LEFT_RIGHT_CURLY_BRACE &&
    secondToLastRole == SyntacticElement::EXPRESSION
  ) {
    fourthToLastExpression.data.makeOX(
      *this->owner, this->owner->opSequence(), secondToLastExpression.data
    );
    fourthToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "makeSequence {} expression X to expression X";
    }
    return this->decreaseStackExceptLast(2);
  }
  if (
    thirdToLastRole == SyntacticElement::SEQUENCE &&
    secondToLastRole == SyntacticElement::COMMA && (
      lastRole == SyntacticElement::RIGHT_PARENTHESIS ||
      lastRole == SyntacticElement::RIGHT_CURLY_BRACE ||
      lastRole == SyntacticElement::RIGHT_BRACKET
    )
  ) {
    this->lastRuleName = "replace xy by y";
    return this->replaceXYByY();
  }
  if (
    fifthToLastRole == SyntacticElement::EXPRESSION &&
    fourthToLastRole == SyntacticElement::LEFT_RIGHT_CURLY_BRACE &&
    thirdToLastRole == SyntacticElement::LEFT_PARENTHESIS &&
    secondToLastRole == SyntacticElement::SEQUENCE &&
    lastRole == SyntacticElement::RIGHT_PARENTHESIS
  ) {
    return this->replaceEXXSequenceXBy_Expression_with_E_instead_of_sequence();
  }
  if (
    fifthToLastRole == SyntacticElement::EXPRESSION &&
    fourthToLastRole == SyntacticElement::LEFT_RIGHT_CURLY_BRACE &&
    thirdToLastRole == SyntacticElement::LEFT_CURLY_BRACE &&
    secondToLastRole == SyntacticElement::SEQUENCE &&
    lastRole == SyntacticElement::RIGHT_CURLY_BRACE
  ) {
    return this->replaceEXXSequenceXBy_Expression_with_E_instead_of_sequence();
  }
  if (
    secondToLastRole == SyntacticElement::SEQUENCE && ((
        thirdToLastRole == SyntacticElement::LEFT_PARENTHESIS &&
        lastRole == SyntacticElement::RIGHT_PARENTHESIS
      ) || (
        thirdToLastRole == SyntacticElement::LEFT_CURLY_BRACE &&
        lastRole == SyntacticElement::RIGHT_CURLY_BRACE
      ) || (
        this->owner->flagUseBracketsForIntervals && ((
            thirdToLastRole == SyntacticElement::LEFT_BRACKET &&
            lastRole == SyntacticElement::RIGHT_PARENTHESIS
          ) || (
            thirdToLastRole == SyntacticElement::LEFT_PARENTHESIS &&
            lastRole == SyntacticElement::RIGHT_BRACKET
          ) || (
            thirdToLastRole == SyntacticElement::LEFT_BRACKET &&
            lastRole == SyntacticElement::RIGHT_BRACKET
          )
        )
      )
    )
  ) {
    secondToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    if (this->flagLogSyntaxRules) {
      this->lastRuleName =
      "left delimiter sequence right delimiter to expression";
    }
    return true;
  }
  if (
    fourthToLastRole == SyntacticElement::BEGIN &&
    thirdToLastRole == SyntacticElement::LEFT_CURLY_BRACE &&
    secondToLastRole == SyntacticElement::MATRIX_ENVIRONMENT &&
    lastRole == SyntacticElement::RIGHT_CURLY_BRACE
  ) {
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "begin{matrix} to empty matrix";
    }
    return this->replaceElementsByMatrixStart(4);
  }
  if (
    fifthToLastRole == SyntacticElement::BEGIN &&
    fourthToLastRole == SyntacticElement::LEFT_CURLY_BRACE &&
    thirdToLastRole == SyntacticElement::ARRAY_ENVIRONMENT &&
    secondToLastRole == SyntacticElement::RIGHT_CURLY_BRACE &&
    lastRole == SyntacticElement::EXPRESSION
  ) {
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "begin{array} to empty matrix";
    }
    return this->replaceElementsByMatrixStart(5);
  }
  if (
    fourthToLastRole == SyntacticElement::END &&
    thirdToLastRole == SyntacticElement::LEFT_CURLY_BRACE && (
      secondToLastRole == SyntacticElement::ARRAY_ENVIRONMENT ||
      secondToLastRole == SyntacticElement::MATRIX_ENVIRONMENT
    ) &&
    lastRole == SyntacticElement::RIGHT_CURLY_BRACE
  ) {
    fourthToLastExpression.syntacticRole = SyntacticElement::MATRIX_END;
    this->lastRuleName = "end matrix environment";
    return this->decreaseStackSetCharacterRanges(3);
  }
  if (
    secondToLastRole == SyntacticElement::BACKSLASH &&
    lastRole == SyntacticElement::BACKSLASH
  ) {
    secondToLastExpression.syntacticRole = SyntacticElement::DOUBLE_BACKSLASH;
    secondToLastExpression.source = "\\\\";
    this->lastRuleName = "backslash backslash to double backslash";
    return this->popTopSyntacticStack();
  }
  if (
    thirdToLastRole == SyntacticElement::MATRIX_UNDER_CONSTRUCTION &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    lastRole == SyntacticElement::AMPERSAND
  ) {
    return this->replaceMatrixEXByMatrix();
  }
  if (
    thirdToLastRole == SyntacticElement::MATRIX_UNDER_CONSTRUCTION &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    lastRole == SyntacticElement::MATRIX_END
  ) {
    thirdToLastExpression.dataList.lastObject()->addChildOnTop(
      secondToLastExpression.data
    );
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "matrix expression matrixEnd to matrix matrixEnd";
    }
    return this->decreaseStackExceptLast(1);
  }
  if (
    thirdToLastRole == SyntacticElement::MATRIX_UNDER_CONSTRUCTION &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    lastRole == SyntacticElement::DOUBLE_BACKSLASH
  ) {
    return this->replaceMatrixEXByMatrixNewRow();
  }
  if (
    secondToLastRole == SyntacticElement::MATRIX_UNDER_CONSTRUCTION &&
    lastRole == SyntacticElement::MATRIX_END
  ) {
    return this->replaceMatrixXByE();
  }
  if (!this->owner->flagUseBracketsForIntervals) {
    if (
      fifthToLastRole == SyntacticElement::LEFT_BRACKET &&
      fourthToLastRole == SyntacticElement::EXPRESSION &&
      thirdToLastRole == SyntacticElement::COMMA &&
      secondToLastRole == SyntacticElement::EXPRESSION &&
      lastRole == SyntacticElement::RIGHT_BRACKET
    ) {
      fifthToLastE.data.makeXOX(
        *this->owner,
        this->owner->opLieBracket(),
        fourthToLastExpression.data,
        secondToLastExpression.data
      );
      fifthToLastE.syntacticRole = SyntacticElement::EXPRESSION;
      if (this->flagLogSyntaxRules) {
        this->lastRuleName = "[a, b] to lie bracket of a, b";
      }
      return this->decreaseStackSetCharacterRanges(4);
    }
  }
  if (
    this->isSeparatorFromTheLeftForDefinition(eighthToLastRole) &&
    seventhToLastRole == SyntacticElement::EXPRESSION &&
    sixthToLastRole == SyntacticElement::COLON &&
    fifthToLastRole == SyntacticElement::IF &&
    fourthToLastRole == SyntacticElement::EXPRESSION &&
    thirdToLastRole == SyntacticElement::EQUALS &&
    secondToLastRole == SyntacticElement::EXPRESSION &&
    this->isSeparatorFromTheRightForDefinition(lastRole)
  ) {
    Expression result(*this->owner);
    result.addChildAtomOnTop(this->owner->opDefineConditional());
    result.addChildOnTop(seventhToLastExpression.data);
    result.addChildOnTop(fourthToLastExpression.data);
    result.addChildOnTop(secondToLastExpression.data);
    seventhToLastExpression.data = result;
    seventhToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    if (this->flagLogSyntaxRules) {
      this->lastRuleName = "expression : if expression equals expression";
    }
    return this->decreaseStackExceptLast(5);
  }
  if (
    thirdToLastRole == SyntacticElement::SEQUENCE_STATEMENTS &&
    secondToLastRole == SyntacticElement::EXPRESSION && (
      lastRole == SyntacticElement::RIGHT_PARENTHESIS ||
      lastRole == SyntacticElement::RIGHT_CURLY_BRACE
    )
  ) {
    this->replaceEXdotsXbySsXdotsX(1);
    this->lastRuleName = "Commands;Expression)";
    return true;
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION &&
    lastRole == SyntacticElement::SEMICOLON
  ) {
    this->replaceEXdotsXBySs(1);
    this->lastRuleName = "expression semicolon";
    return true;
  }
  if (
    secondToLastRole == SyntacticElement::EXPRESSION &&
    lastRole == SyntacticElement::END_PROGRAM
  ) {
    this->replaceEXdotsXbySsXdotsX(1);
    this->lastRuleName = "expression endprogram to sequence statements";
    return true;
  }
  if ((
      thirdToLastRole == SyntacticElement::LEFT_PARENTHESIS ||
      thirdToLastRole == SyntacticElement::LEFT_CURLY_BRACE
    ) &&
    secondToLastRole == SyntacticElement::SEQUENCE_STATEMENTS && (
      lastRole == SyntacticElement::RIGHT_PARENTHESIS ||
      lastRole == SyntacticElement::RIGHT_CURLY_BRACE
    )
  ) {
    thirdToLastExpression.data = secondToLastExpression.data;
    thirdToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    this->lastRuleName =
    "left delimiter sequence statements right delimiter to expression";
    return this->decreaseStack(2);
  }
  if (
    secondToLastRole == SyntacticElement::SEQUENCE_STATEMENTS &&
    lastRole == SyntacticElement::SEQUENCE_STATEMENTS
  ) {
    return this->replaceSsSsXdotsXbySsXdotsX(0);
  }
  if (
    thirdToLastRole == SyntacticElement::SEQUENCE_STATEMENTS &&
    secondToLastRole == SyntacticElement::SEQUENCE_STATEMENTS && (
      lastRole == SyntacticElement::END_PROGRAM ||
      lastRole == SyntacticElement::RIGHT_PARENTHESIS ||
      lastRole == SyntacticElement::RIGHT_CURLY_BRACE
    )
  ) {
    return this->replaceSsSsXdotsXbySsXdotsX(1);
  }
  if (
    secondToLastRole == SyntacticElement::SEQUENCE_STATEMENTS &&
    lastRole == SyntacticElement::END_PROGRAM
  ) {
    this->lastRuleName = "sequence statements end program finalized";
    secondToLastExpression.syntacticRole = SyntacticElement::EXPRESSION;
    return this->popTopSyntacticStack();
  }
  if (lastRole == SyntacticElement::END_PROGRAM) {
    this->lastRuleName = "remove end program";
    return this->decreaseStackSetCharacterRanges(1);
  }
  if (
    lastRole == SyntacticElement::LETTERS && lastExpression.endsOnWhitespace()
  ) {
    this->trimNonStandardWhiteSpaceFromEnd(lastExpression.source);
    if (lastExpression.source.empty()) {
      this->popTopSyntacticStack();
    }
    this->lastRuleName = "trim non-standard whitespace";
    return true;
  }
  return false;
}

bool CalculatorParser::isDefiniteIntegral(SyntacticElement::Role role) {
  return
  role == SyntacticElement::INTEGRAL_WITH_SUBSCRIPT ||
  role == SyntacticElement::INTEGRAL_WITH_SUPERSCRIPT ||
  role == SyntacticElement::INTEGRAL_OPERATOR;
}
