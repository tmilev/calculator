//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader6_1InnerTypedFns_h_already_included
#define vpfHeader6_1InnerTypedFns_h_already_included

static ProjectInformationInstance ProjectInfoVpf6_1Header
(__FILE__, "Declaration of inner typed and other calculator functions. ");

class CommandListInnerTypedFunctions
{
public:
  static bool innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerDivideRFOrPolyOrRatByRFOrPoly
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyRatOrPolyByRatOrPoly
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerAddUEToAny
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyLRObyLRO
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyLRObyLSPath
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerAddEltTensorToEltTensor
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerAddRatOrPolyToRatOrPoly
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerAddPlotToPlot
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerRatPowerRat
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerElementUEPowerRatOrPolyOrRF
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerAddSequenceToSequence
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyRationalBySequence
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplySequenceByRational
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyMatrixSequenceByMatrixSequence
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyMatrixRationalOrRationalByMatrixRational
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerAddMatrixRationalToMatrixRational
  (CommandList& theCommands, const Expression& input, Expression& output)
;
  static bool innerAddRatToRat
  (CommandList& theCommands, const Expression& input, Expression& output)
;

static bool innerMultiplyRatByRat
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyDoubleOrRatByDoubleOrRat
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyCoxeterEltByCoxeterElt
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyCharacterByCharacter
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyCharSSLieAlgByCharSSLieAlg
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerDivideRatByRat
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerDoubleOrRatPowerDoubleOrRat
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerDoubleOrRatTimesDoubleOrRat
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerDoubleOrRatPlusDoubleOrRat
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerAddRatOrPolyOrRFToRatOrPolyOrRF
  (CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyAnyByUE
  (CommandList& theCommands, const Expression& input, Expression& output)
;

static bool innerTensorEltTensorByEltTensor
(CommandList& theCommands, const Expression& input, Expression& output)
;
static bool innerMultiplyAnyByEltTensor
(CommandList& theCommands, const Expression& input, Expression& output)
;


};

#endif
