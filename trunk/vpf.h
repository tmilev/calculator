//*********************************************************************************************************
//*********************************************************************************************************
//Vector partition function - a mathematical library  with focus on
//1) symbolic computation (with the goal of becoming a complete computer algebra system);
//2) computations in the area of Lie representation theory;
//3) computations with vector partition functions.
//
//CopyRight (C) 2009-2012: Todor Milev
//email: todor.milev@gmail.com
//
//C++, javascript: Todor Milev
//svn repository and help with C++ templates: Thomas Bliem
//javascript autocomplete code: Madan Chaudhary
//servers: Steven Jackson @ UMass, the Jacobs University IT staff.
//
//We want to thank the following projects and tools:
//user interface: jsmath, dojo, LaTeX, javascript, google chrome browser/firefox;
//server: apache web server, linux (ubuntu & openSUSE);
//development: sourceforge, C++, gcc, code::blocks, svn.
//
//This is free software. You are welcome to use, modify and redistribute this code
//and the resulting program any way you wish, as long as you provide the same rights
//as those given to you to any future recipients of your modifications (in case you
//decide to pass on those modifications).
//This code is licensed under the
//
//Library General Public License version 3.0
//
//or a later version of the License at your choice
//(the license terms of the LGPL were briefly summarized in the preceding sentence).
//The text of the GNU Library General Public License
//can be found at <http://www.gnu.org/licenses/>, and is also available at the source repository
//http://vectorpartition.svn.sourceforge.net/viewvc/vectorpartition/trunk/  .
//
//Todor Milev would like to thank http://www.cplusplus.com/forum/ for the valuable
//advice and help with C++. Many thanks to helios, Disch, Grey Wolf, jsmith,
//Hammurabi and Duoas!
//*********************************************************************************************************
//*********************************************************************************************************
//DISCLAIMER
//
//THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.
//EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES
//PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND,
//EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
//THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.
//SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,
//REPAIR OR CORRECTION.
//*********************************************************************************************************
//*********************************************************************************************************

#ifndef vpf_h_already_included
#define vpf_h_already_included
//This header file includes the various sub-headers.
//The split into sub-headers is needed to speed up compilation times.
#include "vpfHeader1.h"
#include "vpfHeader1_2.h"
#include "vpfHeader1_3.h"
#include "vpfHeader1_4SemisimpleLieAlgebras.h"
#include "vpfHeader2.h"
static ProjectInformationInstance ProjectInfoVpfH(__FILE__, "Wrapper header file (includes everything).");
#endif
