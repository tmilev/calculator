//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader6_1InnerTypedFns_h_already_included
#define vpfHeader6_1InnerTypedFns_h_already_included

static ProjectInformationInstance ProjectInfoVpf6_1Header
(__FILE__, "Declaration of inner typed and other calculator functions. ");

class CommandListInnerTypedFunctions
{
public:
  template <class theType>
  static bool innerMultiplyTypeByType(CommandList& theCommands, const Expression& input, Expression& output);
  template <class theType>
  static bool innerAddTypeToType(CommandList& theCommands, const Expression& input, Expression& output);
  template <class theType>
  static bool innerDivideTypeByType(CommandList& theCommands, const Expression& input, Expression& output);

  static bool innerAddDoubleOrRatToDoubleOrRat(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerAddRatOrPolyOrEWAToRatOrPolyOrEWA(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerAddRatOrPolyOrRFToRatOrPolyOrRF(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerAddUEToAny(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerAddEltTensorToEltTensor(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerAddRatOrPolyToRatOrPoly(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerAddAlgNumPolyToAlgNumPoly(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerAddPlotToPlot(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerAddSequenceToSequence(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerAddMatrixRationalToMatrixRational(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerAddMatrixTensorToMatrixTensor(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerAddRatToRat(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerAddEltZmodPorRatToEltZmodPorRat(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerAddAlgebraicNumberToAlgebraicNumber(CommandList& theCommands, const Expression& input, Expression& output);

  static bool innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyAlgNumPolyByAlgNumPoly(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyRatOrPolyByRatOrPoly(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyLRObyLRO(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyLRObyLSPath(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyEltZmodPorRatByEltZmodPorRat(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyRationalBySequence(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplySequenceByRational(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyMatrixSequenceByMatrixSequence(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyMatrixRationalOrRationalByMatrixRational(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyMatrixTensorOrRationalByMatrixTensor(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyAlgebraicNumberByAlgebraicNumber(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyRatByRat(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyDoubleOrRatByDoubleOrRat(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyCoxeterEltByCoxeterElt(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyCharacterByCharacter(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyCharSSLieAlgByCharSSLieAlg(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyAnyByUE(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyAnyByEltTensor(CommandList& theCommands, const Expression& input, Expression& output);

  static bool innerLieBracketRatOrUEWithRatOrUE(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerLieBracketRatPolyOrEWAWithRatPolyOrEWA(CommandList& theCommands, const Expression& input, Expression& output);

  static bool innerDivideRatByRat(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerDivideDoubleByDouble(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerDivideRFOrPolyOrRatByRFOrPoly(CommandList& theCommands, const Expression& input, Expression& output);

  static bool innerPowerRatByRat(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerPowerPolyBySmallInteger(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerPowerAlgebraicNumberBySmallInteger(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerPowerEWABySmallInteger(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerPowerElementUEbyRatOrPolyOrRF(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerPowerDoubleOrRatToDoubleOrRat(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerPowerSequenceByT(CommandList& theCommands, const Expression& input, Expression& output);

  static bool innerNChooseK(CommandList& theCommands, const Expression& input, Expression& output);

  static bool innerTensorEltTensorByEltTensor(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerTensorMatRatByMatRat(CommandList& theCommands, const Expression& input, Expression& output);
  static bool innerTensorMatByMatTensor(CommandList& theCommands, const Expression& input, Expression& output);
};

template <class theType>
bool CommandListInnerTypedFunctions::innerMultiplyTypeByType(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyTypeByType");
  if (input.children.size!=3)
    return false;
  Expression inputContextsMerged;
  if (!input.MergeContextsMyArumentsAndConvertThem<theType>(inputContextsMerged))
    return false;
//  std::cout << "Merged contexts, ready for multiplication: " << inputContextsMerged.ToString();
  theType result=inputContextsMerged[1].GetValue<theType>();
  result*=inputContextsMerged[2].GetValue<theType>();
  return output.AssignValueWithContext(result, inputContextsMerged[1].GetContext(), theCommands);
}

template <class theType>
bool CommandListInnerTypedFunctions::innerAddTypeToType(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerAddTypeToType");
  if (input.children.size!=3)
    return false;
  Expression inputContextsMerged;
  if (!input.MergeContextsMyArumentsAndConvertThem<theType>(inputContextsMerged))
    return false;
  theType result=inputContextsMerged[1].GetValue<theType>();
  result+=inputContextsMerged[2].GetValue<theType>();
  return output.AssignValueWithContext(result, inputContextsMerged[1].GetContext(), theCommands);
}

template <class theType>
bool CommandListInnerTypedFunctions::innerDivideTypeByType(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerAddTypeToType");
  if (input.children.size!=3)
    return false;
  Expression inputContextsMerged;
  if (!input.MergeContextsMyArumentsAndConvertThem<theType>(inputContextsMerged))
    return false;
  if (inputContextsMerged[2].GetValue<theType>().IsEqualToZero())
    return output.SetError("Division by zero. ", theCommands);
  theType result=inputContextsMerged[1].GetValue<theType>();
  result/=inputContextsMerged[2].GetValue<theType>();
  return output.AssignValueWithContext(result, inputContextsMerged[1].GetContext(), theCommands);
}

#endif
