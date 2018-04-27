//*********************************************************************************************************
//*********************************************************************************************************
//Vector partition function - a mathematical library  with focus on
//1) symbolic computation (with the goal of becoming a complete computer algebra system);
//2) computations in the area of Lie representation theory;
//3) computations with vector partition functions.
//
//CopyRight (C) 2009-2018: Todor Milev
//email: todor.milev@gmail.com
//
//This code is licensed under the
//
//Apache License, Version 2.0
//
//For more details, see the file
//
//License.md
//
//*********************************************************************************************************
//*********************************************************************************************************

#ifndef vpf_h_already_included
#define vpf_h_already_included
//This header file includes the various sub-headers.
//The splitting into sub-headers is needed to speed up compilation times.
#include "vpfMacros.h"
#include "vpfHeader1General0_General.h"
#include "vpfHeader2Math0_General.h"
//#include "vpfHeader2Math2_AlgebraicNumbers.h"
#include "vpfHeader2Math1_SemisimpleLieAlgebras.h"
#include "vpfHeader2Math3_FiniteGroups.h"
#include "vpfHeader3Calculator0_Interface.h"
static ProjectInformationInstance ProjectInfoVpfH(__FILE__, "Wrapper header file (includes everything).");
#endif
