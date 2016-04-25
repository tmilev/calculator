//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1General0_General.h"
#include "vpfHeader2Math0_General.h"
#include "vpfHeader2Math9DrawingVariables.h"
ProjectInformationInstance ProjectInfoVpf5_2cpp(__FILE__, "Calculator javascript/html/graphics. ");

std::string CreateJavaScriptVectors(Vectors<double>& inputVectors, const std::string& arrayName, bool useVar)
{ MacroRegisterFunctionWithName("CreateStaticJavaScriptVectorsArrayWithProjection");
  if (inputVectors.size==0)
    return "";
  std::stringstream out;
  out << "\n";
  if (useVar)
    out << "var ";
  out << arrayName << "=new Array(" << inputVectors.size << ");\n";
  int theDimension=inputVectors[0].size;
  for (int i=0; i<inputVectors.size; i++)
  { out << arrayName << "[" << i << "]=[";
    for (int j=0; j<theDimension; j++)
    { out << inputVectors[i][j];
      if(j!=theDimension-1)
        out << ",";
     }
    out <<  "];\n";
  }
  return out.str();
}

std::string CreateStaticJavaScriptVectorsArrayWithProjection
(Vectors<double>& inputVectors, const std::string& arrayName, const std::string& projectionsName)
{ MacroRegisterFunctionWithName("CreateStaticJavaScriptVectorsArrayWithProjection");
  if (inputVectors.size==0)
    return "";
  std::stringstream out;
  out << "\nvar " << arrayName << "=new Array(" << inputVectors.size << ");\n";
  out << "var " << projectionsName << "=new Array(" << inputVectors.size << ");\n";

  int theDimension=inputVectors[0].size;
  for (int i=0; i<inputVectors.size; i++)
  { out << arrayName << "[" << i << "]=[";
    for (int j=0; j<theDimension; j++)
    { out << inputVectors[i][j];
      if(j!=theDimension-1)
        out << ",";
     }
    out <<  "];\n";
  }
  for (int i=0; i<inputVectors.size; i++)
  { ////////////////////
//    out << projectionsName << "[" << i << "]=new Array(2);\n";
    out << projectionsName << "[" << i << "]=[0,0];\n";
  }
  return out.str();
}

std::string CreateStaticJavaScriptListVectorsWithProjection
(List<Vectors<double> >& inputVectors, const std::string& arrayName, const std::string& projectionsName)
{ MacroRegisterFunctionWithName("CreateStaticJavaScriptListVectorsWithProjection");
  if (inputVectors.size==0)
    return "";
  std::stringstream out;
  out << "\nvar " << arrayName << "=new Array(" << inputVectors.size << ");\n";
  out << "var " << projectionsName << "=new Array(" << inputVectors.size << ");\n";
  for (int i=0; i<inputVectors.size; i++)
  { std::stringstream projNamesWithIndex, arrayNameWithIndex;
    arrayNameWithIndex << arrayName << "[" << i << "]";
    projNamesWithIndex << projectionsName << "[" << i << "]";
    out << CreateStaticJavaScriptVectorsArrayWithProjection(inputVectors[i], arrayNameWithIndex.str(), projNamesWithIndex.str());
  }
  return out.str();
}

std::string CreateJavaScriptListVectors(List<Vectors<double> >& inputVectors, const std::string& arrayName)
{ MacroRegisterFunctionWithName("CreateStaticJavaScriptListVectors");
  if (inputVectors.size==0)
    return "";
  std::stringstream out;
  out << "\nvar " << arrayName << "=new Array(" << inputVectors.size << ");\n";
  for (int i=0; i<inputVectors.size; i++)
  { std::stringstream projNamesWithIndex, arrayNameWithIndex;
    arrayNameWithIndex << arrayName << "[" << i << "]";
    out << CreateJavaScriptVectors(inputVectors[i], arrayNameWithIndex.str(), false);
  }
  return out.str();
}

std::string CreateStaticJavaScriptTextArray(List<std::string>& theLabels, const std::string& arrayName)
{ MacroRegisterFunctionWithName("CreateStaticJavaScriptVectorsArrayWithProjection");
  if (theLabels.size==0)
    return "";
  std::stringstream out;
  out << "var " << arrayName << "=new Array(" << theLabels.size << ");\n";
  for (int i=0; i<theLabels.size; i++)
    out << arrayName << "[" << i << "]=\"" << theLabels[i] << "\";\n";
  return out.str();
}

std::string DrawingVariables::GetHtmlFromDrawOperationsCreateDivWithUniqueName(int theDimension)
{ MacroRegisterFunctionWithName("DrawingVariables::GetHtmlFromDrawOperationsCreateDivWithUniqueName");
  if (theDimension<2)
    return "<br><b>Pictures of dimension less than two are not drawn.</b><br>";
  std::stringstream out;
  this->NumHtmlGraphics++;
  int timesCalled=this->NumHtmlGraphics;
  std::string theInitFunctionName = "drawConeInit" + std::to_string(timesCalled);
  std::string theDrawFunctionName = "drawAll" + std::to_string(timesCalled);
  std::string theCanvasId = "idCanvasCone" + std::to_string(timesCalled);
  std::string theSurfaceName = "surf" + std::to_string(timesCalled);
  std::string Points1ArrayName="pts1" + std::to_string(timesCalled);
  std::string Points2ArrayName="pts2" + std::to_string(timesCalled);
  std::string filledShapes="FC" + std::to_string(timesCalled);
  std::string circArrayName="circ" + std::to_string(timesCalled);
  std::string txtArrayName="txt" + std::to_string(timesCalled);
  std::string basisCircles = "basisCircles" + std::to_string(timesCalled);
  std::string projName = "proj" + std::to_string(timesCalled);
  std::string projBasisCircles= "projCirc" + std::to_string(timesCalled);
  std::string eiBasis= "eiBasis" + std::to_string(timesCalled);
  std::string cloneVector= "cloneVector" + std::to_string(timesCalled);
  std::string shiftX="shiftXCone" + std::to_string(timesCalled);
  std::string shiftY="shiftYCone" + std::to_string(timesCalled);
  std::string functionConvertToXYName="convXY" + std::to_string(timesCalled);
  std::string labeledVectorsVarName ="labeledVectors" + std::to_string(timesCalled);
//  out << "<style>.leftTD {\n  "
//  <<  "text-align:left;\n  padding:0;\n  border-spacing:0; "
//  << "/* or border-collapse:collapse */\n}\n</style>\n";
  if (!this->flagPlotShowJavascriptOnly)
  { out << "<table> "
    << "<tr><td> <canvas width=\"" << this->DefaultHtmlWidth << "\" height=\"" << this->DefaultHtmlHeight << "\" style=\"border:solid 1px\" id=\"" << theCanvasId
    << "\" onmousedown=\"clickCanvasCone" << timesCalled << "(event.clientX, event.clientY);\" onmouseup=\"selectedBasisIndexCone" << timesCalled
    << "=-1;\" onmousemove=\"mouseMoveRedrawCone" <<  timesCalled << "(event.clientX, event.clientY);\" "
    << "onmousewheel=\"mouseHandleWheelCone" << timesCalled << "(event);\""
    << ">Your browser does not support the HTML5 canvas tag.</canvas></td>";
    out << "<td style=\"min-width: 300px; vertical-align:top;\"><span id=\"canvas" << timesCalled << "Notes\"></span></td></tr></table>";
  }
  List<List<std::string> > textEbasisNamesUserInput;
  List<List<std::string> > textEbasisNamesReadOnly;
  if (this->flagIncludeExtraHtmlDescriptions)
  {

  out << "<br>The projection plane (drawn on the screen) is spanned by the following two vectors<br> \n";
  textEbasisNamesUserInput.SetSize(2);
  textEbasisNamesReadOnly.SetSize(2);
  for (int i=0; i<2; i++)
  { textEbasisNamesReadOnly[i].SetSize(theDimension);
    for (int j=0; j<theDimension; j++)
    { std::stringstream tmpStream;
      tmpStream << "textEbasisReadOnly" << timesCalled << "_" << i << "_" << j;
      textEbasisNamesReadOnly[i][j]=tmpStream.str();
      out << "<span id=\"" << textEbasisNamesReadOnly[i][j] << "\"> </span>\n";
    }
    out << "<br>";
  }
  for (int i=0; i<2; i++)
  { textEbasisNamesUserInput[i].SetSize(theDimension);
    for (int j=0; j<theDimension; j++)
    { std::stringstream tmpStream, tmpStream2;
      tmpStream << "textEbasisUserInput" << timesCalled << "_" << i << "_" << j;
      textEbasisNamesUserInput[i][j]=tmpStream.str();
      out << "<textarea rows=\"1\" cols=\"2\" id=\"" << textEbasisNamesUserInput[i][j] << "\">";
      if (this->flagFillUserDefinedProjection)
        out << this->FillUserDefinedProjection[i][j].GetDoubleValue();
      out << "</textarea>\n";
    }
    out << "<br>";
  }
//  stOutput << " got to here pt 1";
  out << "<button onclick=\"startProjectionPlaneUser" << timesCalled
  << "();\">Change to basis</button><br>";
  out <<"<button onclick=\"snapShotLaTeX" << timesCalled << "();\">LaTeX snapshot</button>\n"
  << "<p>\n"
  << "<span id=\"snapShotLateXspan" << timesCalled << "\"> </span>\n"
  << "</p>\n"
  << "<script type=\"text/javascript\">"
  << "function snapShotLaTeX" << timesCalled << "(){\n"
  << "theText=document.getElementById(\"snapShotLateXspan" << timesCalled << "\");\n"
  << "theText.innerHTML=\"\\\\documentclass{article} \\\\usepackage{auto-pst-pdf}<br>\\n%\\\\usepackage{pst-plot}<br>\\n\\\\begin{document}<br>\\n\";\n"
  << "theText.innerHTML+=\"\\\\psset{xunit=0.01cm, yunit=0.01cm} <br>\\n\\\\begin{pspicture}(0,0)(1,1)\";\n";
  out << "ComputeProjections" << timesCalled << "();\n";
  bool nonImplementedFound=false;
  for (int i=0; i<this->theBuffer.IndexNthDrawOperation.size; i++)
  { int currentIndex=this->theBuffer.IndexNthDrawOperation[i];
    switch(theBuffer.TypeNthDrawOperation[i])
    { case DrawOperations::typeDrawLineBetweenTwoVectors:
        out << "theText.innerHTML+=\""
        << this->GetColorPsTricksFromColorIndex(this->theBuffer.theDrawLineBetweenTwoRootsOperations[currentIndex].ColorIndex)
        << "<br>\";\n";
        out << "theText.innerHTML+=\"\\\\psline[linecolor=currentColor](\"+ "
        << functionConvertToXYName << "( " << Points1ArrayName << "[" << currentIndex<< "])[0]"
        << "+\",\"+"
        << functionConvertToXYName << "( " << Points1ArrayName << "["
        << currentIndex << "])[1]*-1 +\")\";\n ";
        out << "theText.innerHTML+=\"(\"+"
        << functionConvertToXYName << "( " << Points2ArrayName << "["
        << currentIndex << "])[0]+\",\"+"
        << " " << functionConvertToXYName << "( " << Points2ArrayName << "["
        << currentIndex << "])[1]*-1+\")<br>\"; \n";
        break;
      case DrawOperations::typeDrawCircleAtVector:
        out << "theText.innerHTML+=\""
        << this->GetColorPsTricksFromColorIndex(this->theBuffer.theDrawCircleAtVectorOperations[currentIndex].ColorIndex)
        << "<br>\";\n";
        out << "theText.innerHTML+=\"\\\\pscircle[linecolor=currentColor](\"+ "
        << functionConvertToXYName << "( " << circArrayName << "["
        << currentIndex << "])[0]" << "+\",\"+"
        << functionConvertToXYName << "( " << circArrayName << "["
        << currentIndex << "])[1]*-1" << "+ \"){\"+"
        << (((double)this->theBuffer.theDrawCircleAtVectorOperations[currentIndex].radius)/40) << "+\"}<br>\";\n";
        break;
      case DrawOperations::typeFilledShape:
/*        out << "theText.innerHTML+=\""
        << this->GetColorPsTricksFromColorIndex
        (this->theBuffer.theParallelograms[currentIndex].ColorIndex)
        << "<br>\";\n";
        out << "theText.innerHTML+=\"\\\\pscustom[linecolor=currentColor](\"+ "
        << functionConvertToXYName << "( " << circArrayName << "["
        << currentIndex << "])[0]" << "+\",\"+"
        << functionConvertToXYName << "( " << circArrayName << "["
        << currentIndex << "])[1]*-1" << "+ \"){\"+"
        << (((double)this->theBuffer.theDrawCircleAtVectorOperations[currentIndex].radius)/40) << "+\"}<br>\";\n";*/
        nonImplementedFound=true;
        break;
      case DrawOperations::typeDrawTextAtVector:
        out << "theText.innerHTML+=\""
        << this->GetColorPsTricksFromColorIndex(this->theBuffer.theDrawTextAtVectorOperations[currentIndex].ColorIndex)
        << "<br>\";\n";
        out << "theText.innerHTML+=\"\\\\rput[b](\"+ "
        << functionConvertToXYName << "( " << txtArrayName << "["
        << currentIndex << "])[0]" << "+\",\"+"
        << functionConvertToXYName << "( " << txtArrayName << "["
        << currentIndex << "])[1]*-1" << "+ \"){\\\\color{currentColor}"
        << this->theBuffer.theDrawTextAtVectorOperations[currentIndex].theText << "}<br>\";\n";
        break;
      default: break;
    }
  }
//  out << theSurfaceName << ".stroke();\n";
  out << "theText.innerHTML+=\"\\\\end{pspicture}<br>\" ";
  if (nonImplementedFound)
    out << "+\"Not all elements in the html picture were drawn in the LaTeX version. "
    << " Fixing this is on our to-do list. \"";
  out << "+ \"\\\\end{document}\";\n"
  << "}\n"
  << " </script>";
  out << "The bilinear form of the vector space follows. The ij^th element "
  << " gives the scalar product of e_i and e_j. If you enter a degenerate or non-positive definite "
  << " symmetric bilinear form"
  << " the javascript might crash. You are expected to enter a symmetric strictly positive definite matrix. <br> \n";
  for (int i=0; i<this->theBuffer.theBilinearForm.NumRows; i++)
  { for (int j=0; j<this->theBuffer.theBilinearForm.NumCols; j++)
    { std::stringstream tmpStream;
      tmpStream << "textBilinearForm" << timesCalled << "_" << i << "_" << j;
      out << "<textarea rows=\"1\" cols=\"2\" id=\"" << tmpStream.str()
      << "\" \n onChange=\"BilinearForm"
      << timesCalled << "[" << i << "]["
      << j << "]=document.getElementById('" << tmpStream.str() << "').value;\">"
      << this->theBuffer.theBilinearForm.elements[i][j] << "</textarea>\n";
    }
    out << "<br>";
  }
  out << CGI::GetHtmlButton("button"+theCanvasId, theDrawFunctionName+"();", "redraw");
  out << "<br>The picture is drawn using javascript."
  << "<br> Left click + hold+ move the mouse on a special vector = rotates the special vector. "
  << "Special vectors, if any, should be labeled according to the author's ``artistic'' inspiration (usually dark red dots).  "
  << "<br>Moving a vector rotates ``infinitesimally'' the projection plane of your screen "
  << "<br>1) inside the projection plane "
  << "<br>2) in the plane spanned by the selected vector and its orthogonal complement relative to the projection plane. "
  << "<br>The angle change matches the motion of your mouse pointer.  "
  << " Special care must be taken if the selected vector lies "
  << "inside the projection plane or the selected vector is orthogonal to the projection plane. "
  << "If one of these cases happens, the picture might jump around a bit."
  << "<br>The mouse wheel zooms in/out. "
  << " Zooming is tested to work on Firefox and google Chrome browsers on Ubuntu."
  ;
  out << "<br>Currently, " << this->theBuffer.IndexNthDrawOperation.size << " elements are drawn. ";
  if (this->theBuffer.IndexNthDrawOperation.size>500)
    out << " The visualization is javascript/pc processor <b>intensive</b> so it will <b>not work well</b> "
    << " for graphics with lots of elements. This message is displayed only when the number of drawn elements is more than 500.";
  out << "<hr>";
  }
//  stOutput << " got to here pt 2";
  out << "<script type=\"text/javascript\">\n";
  out << "function " << cloneVector << " (inputVector){\n"
  << "output=new Array(" << theDimension << ");\n"
  << "for (i=0; i<" << theDimension << "; i++)\n"
  << "  output[i]=inputVector[i];\n"
  << "return output;\n"
  << "}\n";

  if (this->theBuffer.ProjectionsEiVectors.size!= theDimension || this->theBuffer.theBilinearForm.NumRows!=theDimension)
  { this->theBuffer.MakeMeAStandardBasis(theDimension);
    //stOutput << "made a standard basis!";
  }
  out << "var BilinearForm" << timesCalled << "= new Array(" << theDimension << ");\n";
  for (int i=0; i<theDimension; i++)
  { out << "BilinearForm" << timesCalled << "[" << i << "]=new Array(" << theDimension << ");\n";
    for (int j=0; j<theDimension; j++)
      out << "BilinearForm" << timesCalled << "[" << i << "][" << j << "]=" << this->theBuffer.theBilinearForm.elements[i][j] << ";\t";
    out << "\n";
  }
  this->theBuffer.ModifyToOrthonormalNoShiftSecond(this->theBuffer.BasisProjectionPlane[0][1], this->theBuffer.BasisProjectionPlane[0][0]);
//  (this->theBuffer.BasisProjectionPlane[0][0], this->theBuffer.BasisProjectionPlane[0][1]);
  out << "var VectorE1Cone" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE2Cone" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE1ConeGoal" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE2ConeGoal" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE1ConeStart" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE2ConeStart" << timesCalled << "= new Array(" << theDimension << ");\n";

  for (int i=0; i<theDimension; i++)
  { out << "VectorE2Cone" << timesCalled << "[" << i << "]=" << this->theBuffer.BasisProjectionPlane[0][1][i] << ";\t";
    out << "VectorE1Cone" << timesCalled << "[" << i << "]=" << this->theBuffer.BasisProjectionPlane[0][0][i] << ";\n";
  }
  out << "var frameCount" << timesCalled << "=0;\n";
  out << "var frameCountGoesUp" << timesCalled << "=true;\n";
  int numFramesUserPlane=100;
//  stOutput << " got to here pt 3";
  if (this->flagIncludeExtraHtmlDescriptions)
  {
  out << "function startProjectionPlaneUser" << timesCalled << "(){\n" << " frameCount" << timesCalled << "=0;\n";
  for (int i=0; i<theDimension; i++)
    out << "  VectorE1ConeStart" << timesCalled << "[" << i << "]=VectorE1Cone" << timesCalled << "[" << i << "];\n"
    << "  VectorE2ConeStart" << timesCalled << "[" << i << "]=VectorE2Cone" << timesCalled << "[" << i << "];\n"
    << "  VectorE1ConeGoal" << timesCalled << "[" << i << "]=document.getElementById(\""
    << textEbasisNamesUserInput[0][i] << "\").value;\n"
    << "  VectorE2ConeGoal" << timesCalled << "[" << i << "]=document.getElementById(\""
    << textEbasisNamesUserInput[1][i] << "\").value;\n";
  out << "  changeProjectionPlaneUser" << timesCalled << "();\n"
  << "\n}\n";
  }
  out << "function changeProjectionPlaneUser" << timesCalled << "(){\n"
  << "  frameCount" << timesCalled << "++;\n"
  << "  if (frameCount" << timesCalled << "> " << numFramesUserPlane << ")\n"
  << "    return;\n"
  << "  for (i=0; i<" << theDimension << "; i++)\n"
  << "  { "
  << "VectorE1Cone" << timesCalled << "[i]=VectorE1ConeGoal" << timesCalled << "[i]*"
  << "(frameCount" << timesCalled << "/" << numFramesUserPlane << ")"
  << "+ VectorE1ConeStart" << timesCalled << "[i]*"
  << "(1-frameCount" << timesCalled << "/" << numFramesUserPlane << ");\n"
  << "    VectorE2Cone" << timesCalled << "[i]=VectorE2ConeGoal" << timesCalled << "[i]*"
  << "(frameCount" << timesCalled << "/" << numFramesUserPlane << ")"
  << "+ VectorE2ConeStart" << timesCalled << "[i]*"
  << "(1-frameCount" << timesCalled << "/" << numFramesUserPlane << ");\n"
  << "  }\n"
  << "  MakeVectorE1AndE2orthonormal" << timesCalled << "();\n"
  << "  ComputeProjections" << timesCalled << "();\n"
  << "  " << theDrawFunctionName << "();\n"
  << "  window.setTimeout(\"changeProjectionPlaneUser" << timesCalled << "()\",100);\n"
  << "}";
//    stOutput << " got to here pt 4";

  if (this->theBuffer.BasisProjectionPlane.size>2)
  { out << "BasisProjectionPlane" << timesCalled << "=new Array(" << this->theBuffer.BasisProjectionPlane.size << ");\n";
    for (int j=0; j<this->theBuffer.BasisProjectionPlane.size; j++)
    { out << "BasisProjectionPlane" << timesCalled << "[" << j << "]=new Array(2);\n";
      for (int k=0; k<2; k++)
      { out << "BasisProjectionPlane" << timesCalled << "[" << j << "][" << k << "]=new Array(" << theDimension << ");\n";
        for (int l=0; l<theDimension; l++)
          out << "BasisProjectionPlane" << timesCalled << "[" << j << "][" << k << "][" << l << "]="
          << this->theBuffer.BasisProjectionPlane[j][k][l] << ";\t";
        out << "\n";
      }
      out << "\n";
    }
    out << "function changeProjectionPlaneOnTimer" << timesCalled << "(){\n"
    << "if(frameCountGoesUp" << timesCalled << ")\n"
    << "  frameCount" << timesCalled << "++;\n"
    << "else\n"
    << "  frameCount" << timesCalled << "--;\n"
    << "if (frameCount" << timesCalled << "==" << this->theBuffer.BasisProjectionPlane.size-1 << " || "
    << "frameCount" << timesCalled << "==0)\n"
    << "{ \n"
    << "  frameCountGoesUp" << timesCalled << "=! frameCountGoesUp" << timesCalled << ";\n"
    << "}\n"
    << "if (frameCount" << timesCalled << ">= " << this->theBuffer.BasisProjectionPlane.size << ")\n"
    << "  return;"
    << "for (i=0; i<" << theDimension << "; i++)\n"
    << "{ VectorE1Cone" << timesCalled << "[i]=BasisProjectionPlane" << timesCalled
    << "[frameCount" << timesCalled << "][0][i];\n"
    << "  VectorE2Cone" << timesCalled << "[i]=BasisProjectionPlane" << timesCalled
    << "[frameCount" << timesCalled << "][1][i];\n"
    << "\n}\n";
    out << theDrawFunctionName << "();\n";
    out << "window.setTimeout(\"changeProjectionPlaneOnTimer" << timesCalled << "()\",100);\n" << "}\n";
  }
//    stOutput << " got to here pt 5";

  out << CreateStaticJavaScriptVectorsArrayWithProjection
  (this->theBuffer.labeledVectors, labeledVectorsVarName, "proj"+labeledVectorsVarName);
  out << CreateStaticJavaScriptTextArray
  (this->theBuffer.labelsOfLabeledVectors, "labels"+labeledVectorsVarName);
  out << CreateJavaScriptListVectors(this->theBuffer.toBeHighlightedWhenLabeledVectorHovered, "highlight"+labeledVectorsVarName);
//    stOutput << " got to here pt 6";
  out << "var selectedLabels" << timesCalled << "= new Array(" << this->theBuffer.labeledVectors.size << ");\n";
  out << "var " << projName << "= new Array(" << theDimension << ");\n";
  out << "var " << eiBasis << "= new Array(" << theDimension << ");\n";
  for (int i=0; i<theDimension; i++)
    out << projName << "[" << i << "]= new Array(2);\n";
  out << "var " << basisCircles << "=new Array(" << this->theBuffer.BasisToDrawCirclesAt.size << ");\n";
  out << "var " << projBasisCircles << "=new Array(" << this->theBuffer.BasisToDrawCirclesAt.size << ");\n";
  for (int i=0; i<this->theBuffer.BasisToDrawCirclesAt.size; i++)
  { out << basisCircles << "[" << i << "]=[";
    for (int j=0; j<theDimension; j++)
    { out << this->theBuffer.BasisToDrawCirclesAt[i][j];
      if(j!=theDimension-1)
        out << ",";
     }
    out <<  "];\n";
  }
  for (int i=0; i<theDimension; i++)
  { ////////////////////
    out << eiBasis << "[" << i << "]=[";
    for (int j=0; j<theDimension; j++)
    { out << ((i==j)? 1 :0);
      if(j!=theDimension-1)
        out << ",";
     }
    out <<  "];\n";
    //////////////////
    out << projBasisCircles << "[" << i << "]= new Array(2);\n";
  }
  out << "var " << Points2ArrayName << "=new Array(" << this->theBuffer.theDrawLineBetweenTwoRootsOperations.size << ");\n";
  out << "var " << circArrayName << "=new Array(" << this->theBuffer.theDrawCircleAtVectorOperations.size << ");\n";
  out << "var " << txtArrayName << "=new Array(" << this->theBuffer.theDrawTextAtVectorOperations.size << ");\n";
  out << "var " << filledShapes << "=new Array(" << this->theBuffer.theShapes.size << ");\n";
  for (int i=0; i<this->theBuffer.theShapes.size; i++)
  { out << filledShapes << "[" << i << "]=[";
    DrawFilledShapeOperation& currentShape=this->theBuffer.theShapes[i];
    for (int j=0; j<currentShape.theCorners.size; j++)
    { out << currentShape.theCorners[j].ToStringSquareBracketsBasicType();
      if (j!=currentShape.theCorners.size-1)
        out << ", ";
    }
    out << "];\n";
  }
  out << "var " << Points1ArrayName << "=[ ";
  for (int i=0; i<this->theBuffer.theDrawLineBetweenTwoRootsOperations.size; i++)
  { Vector<double>& current1=theBuffer.theDrawLineBetweenTwoRootsOperations[i].v1;
    out << current1.ToStringSquareBracketsBasicType();
    if (i<this->theBuffer.theDrawLineBetweenTwoRootsOperations.size-1)
      out << ", ";
  }
  out << "];\n";
  out << "var " << Points2ArrayName << "=[";
  for (int i=0; i<this->theBuffer.theDrawLineBetweenTwoRootsOperations.size; i++)
  { Vector<double>& current2=theBuffer.theDrawLineBetweenTwoRootsOperations[i].v2;
    out << current2.ToStringSquareBracketsBasicType();
    if (i<this->theBuffer.theDrawLineBetweenTwoRootsOperations.size-1)
      out << ", ";
  }
  out << "];\n";
  for (int i=0; i<this->theBuffer.theDrawCircleAtVectorOperations.size; i++)
  { Vector<double>& current1=theBuffer.theDrawCircleAtVectorOperations[i].theVector;
    out << circArrayName << "[" << i << "]=[";
    for (int j=0; j<theDimension; j++)
    { out << current1[j];
      if (j!=theDimension-1)
        out << ",";
    }
    out << "];\n";
  }
  for (int i=0; i<this->theBuffer.theDrawTextAtVectorOperations.size; i++)
  { Vector<double>& current1=theBuffer.theDrawTextAtVectorOperations[i].theVector;
    out << txtArrayName << "[" << i << "]=[";
    for (int j=0; j<theDimension; j++)
    { out << current1[j];
      if (j!=theDimension-1)
        out << ",";
    }
    out << "];\n";
  }
//  stOutput << " got to here pt 8";
  out << "var " << shiftX << "=" << this->theBuffer.centerX[0] << ";\n";
  out << "var " << shiftY << "=" << this->theBuffer.centerY[0] << ";\n";
  out << "var GraphicsUnitCone" << timesCalled << "=" << this->theBuffer.GraphicsUnit[0] << ";\n";
  out << "function " << functionConvertToXYName << "(vector){\n"
  << "  resultX=" << shiftX << ";\n"
  << "  resultY=" << shiftY << ";\n"
  << "  for (i=0; i<" << theDimension << "; i++){\n"
  << "    resultX+=vector[i]*" << projName << "[i][0];\n"
  << "    resultY+=vector[i]*" << projName << "[i][1];\n"
  << "  }\n"
  << "  result=[resultX, resultY];\n"
  << "  return result;\n"
  << "}\n";
  out << "var " << theSurfaceName << "=0;\n";
  out << "function drawHighlights" << timesCalled << "() {\n"
  << "for (var i=0; i<" << this->theBuffer.labeledVectors.size << "; i++)\n"
  << "  if (selectedLabels" << timesCalled << "[i])\n"
  << "    for (var j=0; j<highlight" << labeledVectorsVarName << "[i].length; j++)\n"
  << "    { " << theSurfaceName << ".strokeStyle=\"#555555\";\n "
  << "      " << theSurfaceName << ".beginPath();\n "
  << "      " << theSurfaceName << ".arc("
  << functionConvertToXYName << "(highlight" << labeledVectorsVarName << "[i][j])[0],"
  << functionConvertToXYName << "(highlight" << labeledVectorsVarName << "[i][j])[1], 7"
  << ", 0, 2*Math.PI);\n "
  << "      " << theSurfaceName << ".stroke();\n"
  << "    }\n"
  << "}\n";
  out << "function " << theDrawFunctionName << "(){\n"
  << "  if (" << theSurfaceName << "==0)\n"
  << "    " << theInitFunctionName << "();\n"
  << "ComputeProjections" << timesCalled << "();\n";
  out << theSurfaceName << ".fillStyle=\"#FFFFFF\";\n";
  out << theSurfaceName << ".fillRect(0,0," << this->DefaultHtmlWidth  << " ," << this->DefaultHtmlHeight << ");\n";
//  stOutput << " got to here pt 9";
  for (int i=0; i<this->theBuffer.IndexNthDrawOperation.size; i++)
  { int currentIndex=this->theBuffer.IndexNthDrawOperation[i];
    switch(theBuffer.TypeNthDrawOperation[i])
    { case DrawOperations::typeDrawLineBetweenTwoVectors:
        out << theSurfaceName << ".beginPath();\n ";
        out << theSurfaceName << ".strokeStyle=\""
        << this->GetColorHtmlFromColorIndex(this->theBuffer.theDrawLineBetweenTwoRootsOperations[currentIndex].ColorIndex)
        << "\"; ";
        out << theSurfaceName << ".lineWidth="
        << FloatingPoint::DoubleToString
        (this->theBuffer.theDrawLineBetweenTwoRootsOperations[currentIndex].lineWidth)
        << "; ";
        out << theSurfaceName << ".moveTo("
        << functionConvertToXYName << "( " << Points1ArrayName << "[" << currentIndex << "])[0],"
        << functionConvertToXYName << "( " << Points1ArrayName << "["
        << currentIndex << "])[1]); ";
        out << theSurfaceName << ".lineTo("
        << functionConvertToXYName << "( " << Points2ArrayName << "["
        << currentIndex << "])[0],"
        << " " << functionConvertToXYName << "( " << Points2ArrayName << "["
        << currentIndex << "])[1]); ";
        out << theSurfaceName << ".stroke();\n";
        break;
      case DrawOperations::typeFilledShape:
        if (theDimension!=2)
          break;
        out << theSurfaceName << ".beginPath();\n ";
        out << theSurfaceName << ".fillStyle=\""
        << this->GetColorHtmlFromColorIndex(this->theBuffer.theShapes[currentIndex].ColorFillIndex)
        << "\";\n";
        out << theSurfaceName << ".lineWidth="
        << FloatingPoint::DoubleToString
        (this->theBuffer.theShapes[currentIndex].lineWidth)
        << ";\n ";
        out << theSurfaceName << ".moveTo("
        << functionConvertToXYName << "( " << filledShapes << "[" << currentIndex << "][0])[0],"
        << functionConvertToXYName << "( " << filledShapes << "[" << currentIndex << "][0])[1]);\n ";
        out << "for (var i=1; i< " << filledShapes << "[" << currentIndex << "].length; i++)\n";
        out << "  " << theSurfaceName << ".lineTo("
        << functionConvertToXYName << "( " << filledShapes << "[" << currentIndex << "][i])[0],"
        << functionConvertToXYName << "( " << filledShapes << "[" << currentIndex << "][i])[1]);\n ";
        out << theSurfaceName << ".lineTo("
        << functionConvertToXYName << "( " << filledShapes << "[" << currentIndex << "][0])[0],"
        << functionConvertToXYName << "( " << filledShapes << "[" << currentIndex << "][0])[1]);\n ";
        out << theSurfaceName << ".fill();\n";
        out << theSurfaceName << ".strokeStyle=\""
        << this->GetColorHtmlFromColorIndex(this->theBuffer.theShapes[currentIndex].ColorIndex)
        << "\";\n";
        out << theSurfaceName << ".stroke();\n";
        break;
      case DrawOperations::typeDrawCircleAtVector:
        out << theSurfaceName << ".strokeStyle=\""
        << this->GetColorHtmlFromColorIndex(this->theBuffer.theDrawCircleAtVectorOperations[currentIndex].ColorIndex)
        << "\"; ";
        out << theSurfaceName << ".beginPath(); ";
        out << theSurfaceName << ".arc("
        << functionConvertToXYName << "( " << circArrayName << "["
        << currentIndex << "])[0],"
        << functionConvertToXYName << "( " << circArrayName << "["
        << currentIndex << "])[1],"
        <<  this->theBuffer.theDrawCircleAtVectorOperations[currentIndex].radius << ", 0, 2*Math.PI); ";
        out << theSurfaceName << ".stroke();\n";
        break;
      case DrawOperations::typeDrawTextAtVector:
        out << theSurfaceName << ".strokeStyle=\""
        << this->GetColorHtmlFromColorIndex(this->theBuffer.theDrawTextAtVectorOperations[currentIndex].ColorIndex)
        << "\"; ";
        out << theSurfaceName << ".strokeText(\"" << this->theBuffer.theDrawTextAtVectorOperations[currentIndex].theText << "\", "
        << functionConvertToXYName << "( " << txtArrayName << "[" << currentIndex<< "])[0],"
        << functionConvertToXYName << "( " << txtArrayName << "[" << currentIndex << "])[1]);\n";
      default: break;
    }
  }
//    stOutput << " got to here pt 10C";

  out << "drawHighlights" << timesCalled << "();\n";
  out << "}\n";
  out << "function " << theInitFunctionName << "(){\n"
  << theSurfaceName << " = document.getElementById(\"" << theCanvasId << "\").getContext(\"2d\");\n"
  << theDrawFunctionName << "();\n";
  if (this->theBuffer.BasisProjectionPlane.size>2)
    out << "window.setTimeout(\"changeProjectionPlaneOnTimer" << timesCalled << "()\",100);\n";
  out << " }\n";
  out << "var selectedBasisIndexCone" << timesCalled << "=-1;\n";
  out << "var xShiftPointer" << timesCalled << "=0;\n";
  out << "var yShiftPointer" << timesCalled << "=0;\n";
  out << "var clickTolerance=5;\n"
  << "function ptsWithinClickToleranceCone" << timesCalled << "(x1, y1, x2, y2)\n"
  << "{ if (x1-x2>clickTolerance || x2-x1>clickTolerance || y1-y2>clickTolerance || y2-y1>clickTolerance )\n    return false;\n  return true;\n}";

  out << "function MultiplyVector" << timesCalled << "(output, coeff)"
  << "{ for (var i=0; i<output.length; i++)\n"
  << "  output[i]*=coeff;\n"
  << "}\n"
  << "function AddVectorTimes" << timesCalled << "(output, inputVector, coeff)\n"
  << "{ for (var i=0; i<output.length; i++)\n"
  << "  output[i]+=inputVector[i]*coeff;\n"
  << "}\n"
  << "function ScaleToUnitLength" << timesCalled << "(vector)\n"
  << "{ MultiplyVector" << timesCalled << "(vector, 1/Math.sqrt(getScalarProduct" << timesCalled << "(vector,vector)));\n"
  << "}\n";
  out << "function RotateOutOfPlane" << timesCalled << "(input, orthoBasis1, orthoBasis2, oldTanSquared, newTanSquared, newX, newY, oldX, oldY)"
  << "{ var projection= " << cloneVector << "(orthoBasis1);\n"
  << "  var vComponent= " << cloneVector << "(input);\n"
  << "  var scal1= getScalarProduct" << timesCalled << "(orthoBasis1, input);\n"
  << "  var scal2= getScalarProduct" << timesCalled << "(orthoBasis2, input);\n"
  << "  MultiplyVector" << timesCalled << "(projection, scal1);\n"
  << "  AddVectorTimes" << timesCalled << "(projection, orthoBasis2, scal2);\n"
  << "  AddVectorTimes" << timesCalled << "(vComponent, projection, -1);\n"
  << "  var oldAngle=Math.atan(Math.sqrt(oldTanSquared));\n"
  << "  var newAngle=Math.atan(Math.sqrt(newTanSquared));\n"
  << "  if (isNaN(oldAngle) || isNaN(newAngle))\n"
  << "    return input;\n"
  << "  var angleChange=-oldAngle+newAngle;\n"
  << "  if (newX*oldX<0 && newY*oldY<0)\n"
//  <<  " {
//  << "    angleChange=oldAngle+newAngle;\n"
  << "    angleChange*=-1;\n"
//  << "  };"
//  << "  topBound=Math.PI/2;\n"
//  << "  bottomBound=-Math.PI/2;\n"
//  << "  while (angleChange>=topBound || angleChange<bottomBound)\n"
//  << "    if(angleChange>=topBound)\n"
//  << "      angleChange-=Math.PI;\n"
//  << "    else"
//  << "      angleChange+=Math.PI;\n"
  << "  projection=" << cloneVector << "(orthoBasis1);\n"
  << "  MultiplyVector" << timesCalled << "(projection, Math.cos(angleChange)*scal1-Math.sin(angleChange)*scal2);\n"
  << "  AddVectorTimes" << timesCalled << "(projection, orthoBasis2, Math.sin(angleChange)*scal1+Math.sin(angleChange)*scal2);\n"
  << "  var result= vComponent;\n"
  << "  AddVectorTimes" << timesCalled << "(result, projection, 1);\n"
  << "  return result;\n"
  << "}\n";
  out << "function MakeVectorE1AndE2orthonormal" << timesCalled << "(){\n"
  << "  AddVectorTimes" << timesCalled << "(VectorE2Cone" << timesCalled
  << ", VectorE1Cone" << timesCalled << ", -getScalarProduct"
  << timesCalled << "(VectorE1Cone" << timesCalled << ", VectorE2Cone" << timesCalled
  << ")/getScalarProduct" << timesCalled
  << "(VectorE1Cone" << timesCalled << ", VectorE1Cone" << timesCalled << "));\n"
  << "  ScaleToUnitLength" << timesCalled << "(VectorE1Cone" << timesCalled << ");\n"
  << "  ScaleToUnitLength" << timesCalled << "(VectorE2Cone" << timesCalled << ");\n"
  << "}\n";
  out << "function ComputeProjections" << timesCalled << "(){\n"
  << " for (var i=0; i<" << theDimension << "; i++)\n"
  << "  { " << projName << "[i][0]=GraphicsUnitCone"
  << timesCalled << "*getScalarProduct" << timesCalled
  << "(VectorE1Cone" << timesCalled << ","  << eiBasis << "[i]);\n"
  << "    " << projName << "[i][1]=-GraphicsUnitCone" << timesCalled
  << "*getScalarProduct" << timesCalled
  << "(VectorE2Cone" << timesCalled << ", " << eiBasis << "[i]);\n"
  << "  }\n";
//    stOutput << " got to here pt 11C";
  if (this->flagIncludeExtraHtmlDescriptions)
  { for (int j=0; j<theDimension; j++)
      out << "  document.getElementById(\"" << textEbasisNamesReadOnly[0][j] << "\").innerHTML="
      << "VectorE1Cone" << timesCalled << "[" << j << "];\n"
      << "  document.getElementById(\"" << textEbasisNamesReadOnly[1][j] << "\").innerHTML="
      << "VectorE2Cone" << timesCalled << "[" << j << "];\n";
  }
  out
  << "  for (var i=0; i<" << this->theBuffer.BasisToDrawCirclesAt.size << "; i++)\n"
  << "  { " << projBasisCircles << "[i][0]=GraphicsUnitCone" << timesCalled
  << "*getScalarProduct" << timesCalled
  << "(VectorE1Cone" << timesCalled << ", " << basisCircles << "[i]);\n"
  << "    " << projBasisCircles << "[i][1]=-GraphicsUnitCone" << timesCalled
  << "*getScalarProduct" << timesCalled
  << "(VectorE2Cone" << timesCalled << ", " << basisCircles << "[i]);\n"
  << "  }\n";
  out << "  for (var i=0; i<" << this->theBuffer.labeledVectors.size << "; i++)\n"
  << "  { proj" << labeledVectorsVarName << "[i][0]=GraphicsUnitCone" << timesCalled
  << "*getScalarProduct" << timesCalled
  << "(VectorE1Cone" << timesCalled << ", " << labeledVectorsVarName << "[i]);\n"
  << "    proj" << labeledVectorsVarName << "[i][1]=-GraphicsUnitCone" << timesCalled
  << "*getScalarProduct" << timesCalled
  << "(VectorE2Cone" << timesCalled << ", " << labeledVectorsVarName << "[i]);\n"
  << "  }\n";
  out << "}\n";
  out << "\nfunction getScalarProduct" << timesCalled << "(root1, root2)\n"
  << "{ var result=0;\n"
  << "  for (var i=0; i<" << theDimension << "; i++)\n"
  << "    for (var j=0; j<" << theDimension << "; j++)\n"
  << "      result+= root1[i]*root2[j]*BilinearForm" << timesCalled << "[i][j];\n"
  << "  return result;\n"
  << "}\n";
  out << "function getAngleFromXandY" << timesCalled << "(x, y, neighborX, neighborY)\n"
  << "{ var result;\n"
  << "  if (x!=0)\n"
  << "   result= Math.atan(y/x);\n"
  << "  else\n"
  << "    if (y>0)\n"
  << "      result= Math.PI/2;\n"
  << "    else\n"
  << "      result= -Math.PI/2;\n"
  << "  return result;\n"
  << "}\n";
  out << "function getAngleChange" << timesCalled << "(newX, newY, oldX, oldY)\n"
  << "{ var result=getAngleFromXandY" << timesCalled
  << "(newX, newY, oldX, oldY)-getAngleFromXandY" << timesCalled << "(oldX, oldY, newX, newY);\n"
  << "  topBound=Math.PI/2;\n"
  << "  bottomBound=-Math.PI/2;\n"
  << "  while (result>topBound || result< bottomBound)\n"
  << "    if (result>topBound)\n"
  << "      result-=Math.PI;\n"
  << "    else\n"
  << "      result+=Math.PI;\n"
  << "  return result;\n"
  << "}\n";
  out << "\nfunction changeBasis" << timesCalled <<  "(selectedIndex, newX, newY)\n"
  << "{ if (newX==0 && newY==0)\n"
  << "    return;\n"
  << "  var selectedRoot=" << basisCircles << "[selectedIndex];\n"
  << "  var selectedRootLength=getScalarProduct" << timesCalled << "(selectedRoot,selectedRoot);\n"
  << "  var projectionSelected=" << projBasisCircles << "[selectedIndex];\n"
  << "  var oldX=projectionSelected[0]/GraphicsUnitCone" << timesCalled << ";\n"
  << "  var oldY=-projectionSelected[1]/GraphicsUnitCone" << timesCalled << ";\n"
  << "  newX/=GraphicsUnitCone" << timesCalled << ";\n"
  << "  newY/=GraphicsUnitCone" << timesCalled << ";\n"
  << "  var epsilon=0.000000015;\n"
  << "  if (newX*newX+newY*newY>0.003)\n"
  << "  { var AngleChange= -getAngleChange" << timesCalled << "(newX, newY, oldX, oldY);\n"
  << "    var NewVectorE1=" << cloneVector << "(VectorE1Cone" << timesCalled << ");\n"
  << "    var NewVectorE2= " << cloneVector << "(VectorE2Cone" << timesCalled << ");\n"
  << "    MultiplyVector" << timesCalled << "(NewVectorE1, Math.cos(AngleChange));\n"
  << "    AddVectorTimes" << timesCalled << "(NewVectorE1, VectorE2Cone" << timesCalled << ", Math.sin(AngleChange));\n"
  << "    MultiplyVector" << timesCalled << "(NewVectorE2, Math.cos(AngleChange));\n"
  << "    AddVectorTimes" << timesCalled << "(NewVectorE2, VectorE1Cone" << timesCalled << ", -Math.sin(AngleChange));\n"
  << "    VectorE1Cone" << timesCalled << "=NewVectorE1;\n"
  << "    VectorE2Cone" << timesCalled << "=NewVectorE2;\n"
  << "  }\n"
  << "  if (newX*newX+newY*newY>0.0001)\n"
  << "  { var RootTimesE1=getScalarProduct" << timesCalled << "(selectedRoot, VectorE1Cone" << timesCalled << ");\n"
  << "    var RootTimesE2=getScalarProduct" << timesCalled << "(selectedRoot, VectorE2Cone" << timesCalled << ");\n"
  << "    var vOrthogonal=" << cloneVector << "(selectedRoot);\n"
  << "    var vProjection=" << cloneVector << "(VectorE1Cone" << timesCalled << ");\n"
  << "    MultiplyVector" << timesCalled << "(vProjection, RootTimesE1);\n"
  << "    AddVectorTimes" << timesCalled << "(vProjection, VectorE2Cone" << timesCalled << ", RootTimesE2" << ");\n"
  << "    AddVectorTimes" << timesCalled << "(vOrthogonal, vProjection, -1);\n"
  << "    var oldRatioProjectionOverHeightSquared = (oldX*oldX+oldY*oldY)/ (selectedRootLength-oldX*oldX-oldY*oldY);\n"
  << "    var newRatioProjectionOverHeightSquared = (newX*newX+newY*newY)/ (selectedRootLength-newX*newX-newY*newY);\n";
  if (theDimension>2)
  { out << "    if (getScalarProduct" << timesCalled << "(vOrthogonal, vOrthogonal)<epsilon && getScalarProduct" << timesCalled
    << "(vOrthogonal, vOrthogonal)>-epsilon)\n"
    << "    { vOrthogonal=" << cloneVector << "(" << eiBasis <<  "[2]);\n"
    << "    }\n";
  }
  out << "    if (getScalarProduct" << timesCalled << "(vOrthogonal, vOrthogonal)>epsilon || getScalarProduct" << timesCalled
  << "(vOrthogonal, vOrthogonal)<-epsilon)\n"
  << "    { if (oldRatioProjectionOverHeightSquared==0)\n"
  << "      { vProjection=" << cloneVector << "(VectorE1Cone" << timesCalled << ");\n"
  << "        MultiplyVector" << timesCalled << "(vProjection, -newX);\n"
  << "        AddVectorTimes" << timesCalled << "(vProjection, VectorE2Cone" << timesCalled << ", newY" << ");\n"
  << "      }\n"
  << "      ScaleToUnitLength" << timesCalled << "(vProjection);\n"
  << "      ScaleToUnitLength" << timesCalled << "(vOrthogonal);\n"
  << "      VectorE1Cone" << timesCalled << "=RotateOutOfPlane" << timesCalled << "(VectorE1Cone" << timesCalled
  << ", vProjection, vOrthogonal, oldRatioProjectionOverHeightSquared, newRatioProjectionOverHeightSquared, newX, newY, oldX, oldY);\n"
  << "      VectorE2Cone" << timesCalled << "=RotateOutOfPlane" << timesCalled << "(VectorE2Cone" << timesCalled
  << ", vProjection, vOrthogonal, oldRatioProjectionOverHeightSquared, newRatioProjectionOverHeightSquared, newX, newY, oldX, oldY);\n"
  << "    }\n"
  << "    MakeVectorE1AndE2orthonormal" << timesCalled << "();\n"
  << "    ComputeProjections" << timesCalled << "();\n"
  << "  }\n"
  << "}\n";
//      stOutput << " got to here pt x0";

  out << "function processMousePosition" << timesCalled << "(x, y){\n"
  << "  labelString=\"<table><tr>\";\n"
  << "  needRedraw=false;\n"
  << "  for (i=0; i<" << this->theBuffer.labeledVectors.size << "; i++)\n"
  << "    if (ptsWithinClickToleranceCone" << timesCalled << "(x,y,proj" << labeledVectorsVarName << "[i][0], proj" << labeledVectorsVarName << "[i][1]))\n"
  << "    { labelString+=\"<td>\"+labels" << labeledVectorsVarName << "[i]+\"</td>\";\n "
  << "      if (!selectedLabels" << timesCalled << "[i])\n"
  << "        needRedraw=true;\n"
  << "      selectedLabels" << timesCalled << "[i]=true;\n "
  << "    } else\n"
  << "    { if (selectedLabels" << timesCalled << "[i])\n"
  << "        needRedraw=true;\n"
  << "      selectedLabels" << timesCalled << "[i]=false;\n "
  << "    }\n"
  << "  if (needRedraw)\n"
  << "  { labelString+=\"</tr></table>\";"
  << "    document.getElementById(\"canvas" << timesCalled << "Notes\").innerHTML=labelString;\n"
  << "    " << theDrawFunctionName << "();\n"
  //<< "    jsMath.Process(document.getElementById(\"canvas" << timesCalled << "Notes\"));\n"
  << "  }\n"
  << "}\n";
  out << "\nfunction getPosXPosY" << timesCalled << "(cx,cy){\n"
  << "  divPosX=0;\n"
  << "  divPosY=0;\n"
  << "  thePointer= document.getElementById(\"idCanvasCone" << timesCalled << "\");\n"
  << "  while(thePointer)  {\n"
  << "    divPosX += thePointer.offsetLeft;\n"
  << "    divPosY += thePointer.offsetTop;\n"
  << "    thePointer = thePointer.offsetParent;\n  "
  << "  }\n"
  << "  posx=(cx-divPosX+document.body.scrollLeft-" << shiftX << ");\n"
  << "  posy=(cy-divPosY+document.body.scrollTop-" << shiftY << ");\n"
  << "  return [posx,posy];\n"
  << "}\n"
  ;
  out << "\nfunction clickCanvasCone" << timesCalled << "(cx,cy){\n"
  << "  posx=getPosXPosY" << timesCalled << "(cx,cy)[0];\n"
  << "  posy=getPosXPosY" << timesCalled << "(cx,cy)[1];\n"
  << "  selectedBasisIndexCone" << timesCalled <<"=-1;\n";
  if (!this->flagAllowMovingCoordinateSystemFromArbitraryClick)
    out << "if (ptsWithinClickToleranceCone" << timesCalled << "(posx, posy, 0, 0))\n"
    << "  selectedBasisIndexCone" << timesCalled << "=-2;\n";
  else
  { out << "selectedBasisIndexCone" << timesCalled << "=-2;\n";
    out
    << "xShiftPointer" << timesCalled << "=posx" << "; "
    << "yShiftPointer" << timesCalled << "=posy" << "; ";
  }
  out
  <<  "for (i=0; i<" << this->theBuffer.BasisToDrawCirclesAt.size << ";i++){\n"
  << "   if (ptsWithinClickToleranceCone" << timesCalled
  << "(posx, posy, " << projBasisCircles << "[i][0]" << ", " << projBasisCircles
  << "[i][1]" << "))\n"
  << "  selectedBasisIndexCone" << timesCalled << "=i;\n"
  << "}\n"
  << "}\n"
  << "function mouseMoveRedrawCone" << timesCalled << "(cx, cy){\n"
  << "  posx=getPosXPosY" << timesCalled << "(cx,cy)[0];\n"
  << "  posy=-getPosXPosY" << timesCalled << "(cx,cy)[1];\n"
  << "  processMousePosition" << timesCalled << "(posx, -posy);\n"
  << "  if (selectedBasisIndexCone" << timesCalled << "==-1)\n"
  << "    return;\n"
  << "  if (selectedBasisIndexCone" << timesCalled << "==-2){\n"
  << "    " << shiftX << "=(cx-divPosX+document.body.scrollLeft)- xShiftPointer" << timesCalled << ";\n"
  << "    " << shiftY << "=(cy-divPosY+document.body.scrollTop) - yShiftPointer" << timesCalled << ";\n"
  << "}\n"
  << "else\n"
  << "{ changeBasis" << timesCalled << "(selectedBasisIndexCone" << timesCalled << ", posx, posy);\n  }\n  "
  << theDrawFunctionName << " ();\n"
  << "}\n";
  out << "\n tempDiv=document.getElementById(\"" << theCanvasId << "\");";
  out << "\ntempDiv.addEventListener (\"DOMMouseScroll\", mouseHandleWheelCone"
  << timesCalled << ", true);\n";
  out << "function mouseHandleWheelCone" << timesCalled << "(theEvent){\n"
  << "  theEvent = theEvent ? theEvent : window.event;\n"
  << "  theEvent.preventDefault();\n"
  << "  theEvent.stopPropagation();\n"
  << "  theWheelDelta = theEvent.detail ? theEvent.detail * -1 : theEvent.wheelDelta / 40;\n"

  << "  posX=getPosXPosY" << timesCalled << "(theEvent.clientX, theEvent.clientY)[0];\n"
  << "  posY=-getPosXPosY" << timesCalled << "(theEvent.clientX, theEvent.clientY)[1];\n"

  << "  posXnew=posX/" << "GraphicsUnitCone" << timesCalled << ";\n"
  << "  posYnew=posY/" << "GraphicsUnitCone" << timesCalled << ";\n"

  << "GraphicsUnitCone" << timesCalled << "+=theWheelDelta;\n"

//  << "  alert('posX, posY, posXnew, posYnew:'+ posX  +', '+ posY+', '+ posXnew+', '+ posYnew);\n"

  << "if (GraphicsUnitCone" << timesCalled << "<=0)\n "
  << "  GraphicsUnitCone" << timesCalled << "=3;\n"

  << "  posXnew*=" << "GraphicsUnitCone" << timesCalled << ";\n"
  << "  posYnew*=" << "GraphicsUnitCone" << timesCalled << ";\n"
  << "  " << shiftX << "+=posX-posXnew;\n"
  << "  " << shiftY << "+=-posY+posYnew;\n"

//  << "for (i=0; i<" << theDimension << "; i++){\n "
//  << basisName << "[i][0]*=GraphicsUnitCone" << timesCalled << "/(GraphicsUnitCone" << timesCalled << "-theWheelDelta);\n"
//  << basisName << "[i][1]*=GraphicsUnitCone" << timesCalled << "/(GraphicsUnitCone" << timesCalled << "-theWheelDelta);\n"
//  << "}\n"
  << theDrawFunctionName << "();\n}\n";
//        stOutput << " got to here pt x1";


  out  << theInitFunctionName << "();\n"
  << "</script>\n"
  ;
//        stOutput << " got to here pt x2";
//  if (this->flagIncludeExtraHtmlDescriptions)
//    out << "INCLUDING XTRAS";
//  else
//    out << "EXCLUDIMG extras";
  return out.str();
}

std::string AnimationBuffer::GetHtmlFromDrawOperationsCreateDivWithUniqueName(int theDimension)
{ if (theDimension<2)
    return "<br><b>Pictures of dimension less than two are not drawn.</b><br>";
  if (theFrames.size<1)
    return "<br><b>Empty animation</b>";
  std::stringstream out, tempStream1, tempStream2, tempStream3, tempStream4, tempStream5, tempStream6;
  std::stringstream tempStream7, tempStream8, tempStream9, tempStream10, tempStream11,
  tempStream12, tempStream13, tempStream14, tempStream15, tempStream16, tempStream4_1, tempStream11_1;
  DrawingVariables* boss=&this->theFrames[0];
  this->theBuffer=boss->theBuffer;
  boss->NumHtmlGraphics++;
  int timesCalled=boss->NumHtmlGraphics;
  tempStream1 << "drawConeInit" << timesCalled;
  std::string theInitFunctionName= tempStream1.str();
  tempStream5 << "drawAll" << timesCalled;
  std::string theDrawFunctionName= tempStream5.str();
  tempStream2 << "idCanvasCone" << timesCalled;
  std::string theCanvasId= tempStream2.str();
  tempStream3 << "surf" << timesCalled;
  std::string theSurfaceName=tempStream3.str();
  tempStream4 << "pts1" << timesCalled;
  std::string Points1ArrayName=tempStream4.str();
  tempStream4_1 << "colorsLines" << timesCalled;
  std::string Points1ArrayNameColors=tempStream4_1.str();

  tempStream10 << "pts2" << timesCalled;
  std::string Points2ArrayName=tempStream10.str();

  tempStream11 << "circ" << timesCalled;
  std::string circArrayName=tempStream11.str();
  tempStream11_1 << "circRadii" << timesCalled;
  std::string circRadiiArrayName=tempStream11_1.str();

  tempStream12 << "txt" << timesCalled;
  std::string txtArrayName=tempStream12.str();

  tempStream6 << "basisCircles" << timesCalled;
  std::string basisCircles = tempStream6.str();
  tempStream13 << "proj" << timesCalled;
  std::string projName = tempStream13.str();
  tempStream14 << "projCirc" << timesCalled;
  std::string projBasisCircles= tempStream14.str();
  tempStream15 << "eiBasis" << timesCalled;
  std::string eiBasis= tempStream15.str();
  tempStream16 << "cloneVector" << timesCalled;
  std::string cloneVector= tempStream16.str();


  tempStream7 << "shiftXCone" << timesCalled;
  std::string shiftX=tempStream7.str();
  tempStream8 << "shiftYCone" << timesCalled;
  std::string shiftY=tempStream8.str();
  tempStream9 << "convXY" << timesCalled;
  std::string functionConvertToXYName=tempStream9.str();

  out << "<canvas width=\"" << boss->DefaultHtmlWidth << "\" height=\"" << boss->DefaultHtmlHeight << "\" border=\"solid 1px\" id=\"" << theCanvasId
  << "\" onmousedown=\"clickCanvasCone" << timesCalled << "(event.clientX, event.clientY);\" onmouseup=\"selectedBasisIndexCone" << timesCalled
  << "=-1;\" onmousemove=\"mouseMoveRedrawCone" <<  timesCalled << "(event.clientX, event.clientY);\" "
  << "onmousewheel=\"mouseHandleWheelCone" << timesCalled << "(event);\"" << "></canvas>";
  out << "<br>The projection plane (drawn on the screen) is spanned by the following two vectors<br> \n";
  List<List<std::string> > textEbasisNamesUserInput;
  List<List<std::string> > textEbasisNamesReadOnly;
  textEbasisNamesUserInput.SetSize(2);
  textEbasisNamesReadOnly.SetSize(2);
  for (int i=0; i<2; i++)
  { textEbasisNamesReadOnly[i].SetSize(theDimension);
    for (int j=0; j<theDimension; j++)
    { std::stringstream tmpStream;
      tmpStream << "textEbasisReadOnly" << timesCalled << "_" << i << "_" << j;
      textEbasisNamesReadOnly[i][j]=tmpStream.str();
      out << "<span id=\"" << textEbasisNamesReadOnly[i][j] << "\"> </span>\n";
    }
    out << "<br>";
  }
  for (int i=0; i<2; i++)
  { textEbasisNamesUserInput[i].SetSize(theDimension);
    for (int j=0; j<theDimension; j++)
    { std::stringstream tmpStream, tmpStream2;
      tmpStream << "textEbasisUserInput" << timesCalled << "_" << i << "_" << j;
      textEbasisNamesUserInput[i][j]=tmpStream.str();
      out << "<textarea rows=\"1\" cols=\"2\" id=\"" << textEbasisNamesUserInput[i][j] << "\">" << "</textarea>\n";
    }
    out << "<br>";
  }
  out << "<button onclick=\"startProjectionPlaneUser" << timesCalled << "();\">Change to basis</button><br>";
  out << "The bilinear form of the vector space follows. The ij^th element "
  << " gives the scalar product of e_i and e_j. If you enter a degenerate or non-positive definite "
  << " symmetric bilinear form"
  << " the javascript might crash. You are expected to enter a symmetric strictly positive definite matrix. <br> \n";
  for (int i=0; i<this->theBuffer.theBilinearForm.NumRows; i++)
  { for (int j=0; j<this->theBuffer.theBilinearForm.NumCols; j++)
    { std::stringstream tmpStream;
      tmpStream << "textBilinearForm" << timesCalled << "_" << i << "_" << j;
      out << "<textarea rows=\"1\" cols=\"2\" id=\"" << tmpStream.str()
      << "\" \n onChange=\"BilinearForm"
      << timesCalled << "[" << i << "]["
      << j << "]=document.getElementById('" << tmpStream.str() << "').value;\">"
      << this->theBuffer.theBilinearForm.elements[i][j] << "</textarea>\n";
    }
    out << "<br>";
  }
  out << CGI::GetHtmlButton("button"+theCanvasId, theDrawFunctionName+"();", "redraw");
  out << "<br>The picture is drawn using javascript."
  << "<br> Left click + hold+ move the mouse on a special vector = rotates the special vector. "
  << "Special vectors, if any, should be labeled according to the author's ``artistic'' inspiration (usually dark red dots).  "
  << "<br>Moving a vector rotates ``infinitesimally'' the projection plane of your screen "
  << "<br>1) inside the projection plane "
  << "<br>2) in the plane spanned by the selected vector and its orthogonal complement relative to the projection plane. "
  << "<br>The angle change matches the motion of your mouse pointer.  "
  << " Special care must be taken if the selected vector lies "
  << "inside the projection plane or the selected vector is orthogonal to the projection plane. "
  << "If one of these cases happens, the picture might jump around a bit."
  << "<br>The mouse wheel zooms in/out. "
  << " Zooming is tested to work on Firefox and google Chrome browsers on Ubuntu."
  ;
  out << "<br>Currently, " << this->theBuffer.IndexNthDrawOperation.size << " elements are drawn. ";
  if (this->theBuffer.IndexNthDrawOperation.size>500)
    out << " The visualization is javascript/pc processor <b>intensive</b> so it will <b>not work well</b> "
    << " for graphics with lots of elements. This message is displayed only when the number of drawn elements is more than 500.";
  out << "\n<hr>\n";

  out << "<script type=\"text/javascript\">\n";
  out << "function " << cloneVector << " (inputVector){\n"
  << "output=new Array(" << theDimension << ");\n"
  << "for (i=0; i<" << theDimension << "; i++)\n"
  << "  output[i]=inputVector[i];\n"
  << "return output;\n"
  << "}\n";


  if (this->theBuffer.ProjectionsEiVectors.size!= theDimension || this->theBuffer.theBilinearForm.NumRows!=theDimension)
  { this->theBuffer.MakeMeAStandardBasis(theDimension);
    //stOutput << "made a standard basis!";
  }
  out << "var BilinearForm" << timesCalled << "= new Array(" << theDimension << ");\n";
  for (int i=0; i<theDimension; i++)
  { out << "BilinearForm" << timesCalled << "[" << i << "]=new Array(" << theDimension << ");\n";
    for (int j=0; j<theDimension; j++)
      out << "BilinearForm" << timesCalled << "[" << i << "][" << j << "]=" << this->theBuffer.theBilinearForm.elements[i][j] << ";\t";
    out << "\n";
  }
  this->theBuffer.ModifyToOrthonormalNoShiftSecond(this->theBuffer.BasisProjectionPlane[0][1], this->theBuffer.BasisProjectionPlane[0][0]);
//  (this->theBuffer.BasisProjectionPlane[0][0], this->theBuffer.BasisProjectionPlane[0][1]);
  out
  << "var VectorE1Cone" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE2Cone" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE1ConeGoal" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE2ConeGoal" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE1ConeStart" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE2ConeStart" << timesCalled << "= new Array(" << theDimension << ");\n";

  for (int i=0; i<theDimension; i++)
  { out << "VectorE2Cone" << timesCalled << "[" << i << "]=" << this->theBuffer.BasisProjectionPlane[0][1][i] << ";\t";
    out << "VectorE1Cone" << timesCalled << "[" << i << "]=" << this->theBuffer.BasisProjectionPlane[0][0][i] << ";\n";
  }
  out << "var frameCount" << timesCalled << "=0;\n";
  out << "var frameCountGoesUp" << timesCalled << "=true;\n";
  int numFramesUserPlane=50;
  out << "function startProjectionPlaneUser" << timesCalled << "(){\n"
  << " frameCount" << timesCalled << "=0;\n";
  for (int i=0; i<theDimension; i++)
    out
    << "  VectorE1ConeStart" << timesCalled << "[" << i << "]=VectorE1Cone" << timesCalled << "[" << i << "];\n"
    << "  VectorE2ConeStart" << timesCalled << "[" << i << "]=VectorE2Cone" << timesCalled << "[" << i << "];\n"
    << "  VectorE1ConeGoal" << timesCalled << "[" << i << "]=document.getElementById(\""
    << textEbasisNamesUserInput[0][i] << "\").value;\n"
    << "  VectorE2ConeGoal" << timesCalled << "[" << i << "]=document.getElementById(\""
    << textEbasisNamesUserInput[1][i] << "\").value;\n";
  out << "  changeProjectionPlaneUser" << timesCalled << "();\n"
  << "\n}\n";
  out << "function changeProjectionPlaneUser" << timesCalled << "(){\n"
  << "  frameCount" << timesCalled << "++;\n"
  << "  if (frameCount" << timesCalled << "> " << numFramesUserPlane << ")\n"
  << "    return;\n"
  << "  for (i=0; i<" << theDimension << "; i++)\n"
  << "  { "
  << "VectorE1Cone" << timesCalled << "[i]=VectorE1ConeGoal" << timesCalled << "[i]*"
  << "(frameCount" << timesCalled << "/" << numFramesUserPlane << ")"
  << "+ VectorE1ConeStart" << timesCalled << "[i]*"
  << "(1-frameCount" << timesCalled << "/" << numFramesUserPlane << ");\n"
  << "    VectorE2Cone" << timesCalled << "[i]=VectorE2ConeGoal" << timesCalled << "[i]*"
  << "(frameCount" << timesCalled << "/" << numFramesUserPlane << ")"
  << "+ VectorE2ConeStart" << timesCalled << "[i]*"
  << "(1-frameCount" << timesCalled << "/" << numFramesUserPlane << ");\n"
  << "  }\n"
  << "  MakeVectorE1AndE2orthonormal" << timesCalled << "();\n"
  << "  ComputeProjections" << timesCalled << "();\n"
  << "  " << theDrawFunctionName << "();\n"
  << "  window.setTimeout(\"changeProjectionPlaneUser" << timesCalled << "()\",100);\n"
  << "}";
  if (this->theBuffer.BasisProjectionPlane.size>2)
  { out << "BasisProjectionPlane" << timesCalled << "=new Array(" << this->theBuffer.BasisProjectionPlane.size << ");\n";
    for (int j=0; j<this->theBuffer.BasisProjectionPlane.size; j++)
    { out << "BasisProjectionPlane" << timesCalled << "[" << j << "]=new Array(2);\n";
      for (int k=0; k<2; k++)
      { out << "BasisProjectionPlane" << timesCalled << "[" << j << "][" << k << "]=new Array(" << theDimension << ");\n";
        for (int l=0; l<theDimension; l++)
          out << "BasisProjectionPlane" << timesCalled << "[" << j << "][" << k << "][" << l << "]="
          << this->theBuffer.BasisProjectionPlane[j][k][l] << ";\t";
        out << "\n";
      }
      out << "\n";
    }
    out << "function changeProjectionPlaneOnTimer" << timesCalled << "(){\n"
    << "if(frameCountGoesUp" << timesCalled << ")\n"
    << "  frameCount" << timesCalled << "++;\n"
    << "else\n"
    << "  frameCount" << timesCalled << "--;\n"
    << "if (frameCount" << timesCalled << "==" << this->theBuffer.BasisProjectionPlane.size-1 << " || "
    << "frameCount" << timesCalled << "==0)\n"
    << "{ \n"
    << "  frameCountGoesUp" << timesCalled << "=! frameCountGoesUp" << timesCalled << ";\n"
    << "}\n"
    << "if (frameCount" << timesCalled << ">= " << this->theBuffer.BasisProjectionPlane.size << ")\n"
    << "  return;"
    << "for (i=0; i<" << theDimension << "; i++)\n"
    << "{ VectorE1Cone" << timesCalled << "[i]=BasisProjectionPlane" << timesCalled
    << "[frameCount" << timesCalled << "][0][i];\n"
    << "  VectorE2Cone" << timesCalled << "[i]=BasisProjectionPlane" << timesCalled
    << "[frameCount" << timesCalled << "][1][i];\n"
    << "\n}\n";
    out << theDrawFunctionName << "();\n";
    out << "window.setTimeout(\"changeProjectionPlaneOnTimer" << timesCalled << "()\",100);\n"
    << "}\n";
  }

  out << "var " << projName << "= new Array(" << theDimension << ");\n";
  out << "var " << eiBasis << "= new Array(" << theDimension << ");\n";
  for (int i=0; i<theDimension; i++)
    out << projName << "[" << i << "]= new Array(2);\n";
  out  << "var " << basisCircles << "=new Array(" << this->theBuffer.BasisToDrawCirclesAt.size << ");\n";
  out  << "var " << projBasisCircles << "=new Array(" << this->theBuffer.BasisToDrawCirclesAt.size << ");\n";
  for (int i=0; i<this->theBuffer.BasisToDrawCirclesAt.size; i++)
  { out << basisCircles << "[" << i << "]=[";
    for (int j=0; j<theDimension; j++)
    { out << this->theBuffer.BasisToDrawCirclesAt[i][j];
      if(j!=theDimension-1)
        out << ",";
     }
    out <<  "];\n";
  }
  for (int i=0; i<theDimension; i++)
  { ////////////////////
    out << eiBasis << "[" << i << "]=[";
    for (int j=0; j<theDimension; j++)
    { out << ((i==j)? 1 :0);
      if(j!=theDimension-1)
        out << ",";
     }
    out <<  "];\n";
    //////////////////
    out << projBasisCircles << "[" << i << "]= new Array(2);\n";
  }
  out << "var " << Points1ArrayName << "=new Array(" << this->theFrames.size << ");\n" << "var " << Points2ArrayName << "=new Array(" << this->theFrames.size << ");\n"
  << "var " << circArrayName << "=new Array(" << this->theFrames.size << ");\n" << "var " << circRadiiArrayName << "=new Array(" << this->theFrames.size << ");\n"
  << "var " << txtArrayName << "=new Array(" << this->theFrames.size << ");\n" << "var " << Points1ArrayNameColors << "=new Array(" << this->theFrames.size << ");\n"
  ;
  for (int k=0; k<this->theFrames.size; k++)
  { DrawOperations& currentOps= this->theFrames[k].theBuffer;
    out << Points1ArrayName << "[" << k << "]" << "=new Array(" << currentOps.theDrawLineBetweenTwoRootsOperations.size << ");\n" << Points2ArrayName << "["
    << k << "]=new Array(" << currentOps.theDrawLineBetweenTwoRootsOperations.size << ");\n" << circArrayName << "[" << k << "]=new Array("
    << currentOps.theDrawCircleAtVectorOperations.size << ");\n" << circRadiiArrayName << "[" << k << "]=new Array(" << currentOps.theDrawCircleAtVectorOperations.size << ");\n"
    << txtArrayName << "[" << k << "]=new Array(" << currentOps.theDrawTextAtVectorOperations.size << ");\n" << Points1ArrayNameColors
    << "[" << k << "]=new Array(" << currentOps.theDrawLineBetweenTwoRootsOperations.size << ");\n"
    ;
    for (int i=0; i<currentOps.theDrawLineBetweenTwoRootsOperations.size; i++)
    { Vector<double>& current1=currentOps.theDrawLineBetweenTwoRootsOperations[i].v1;
      Vector<double>& current2=currentOps.theDrawLineBetweenTwoRootsOperations[i].v2;
      out << Points1ArrayName << "[" << k << "]" << "[" << i << "]=[";
      for (int j=0; j<theDimension; j++)
      { out << current1[j];
        if (j!=theDimension-1)
          out << ",";
      }
      out << "];\n";
      out << Points1ArrayNameColors << "[" << k << "]" << "[" << i << "]=\""
      << boss->GetColorHtmlFromColorIndex(currentOps.theDrawLineBetweenTwoRootsOperations[i].ColorIndex)
      << "\";";
      out << Points2ArrayName << "[" << k << "]" << "[" << i << "]=[";
      for (int j=0; j<theDimension; j++)
      { out << current2[j];
        if (j!=theDimension-1)
          out << ",";
      }
      out << "];\n";
    }
    for (int i=0; i<currentOps.theDrawCircleAtVectorOperations.size; i++)
    { Vector<double>& current1=currentOps.theDrawCircleAtVectorOperations[i].theVector;
      out << circRadiiArrayName << "[" << k << "]" << "[" << i << "]=" << currentOps.theDrawCircleAtVectorOperations[i].radius << ";\n";
      out << circArrayName << "[" << k << "]" << "[" << i << "]=[";
      for (int j=0; j<theDimension; j++)
      { out << current1[j];
        if (j!=theDimension-1)
          out << ",";
      }
      out << "];\n";
    }
    for (int i=0; i<currentOps.theDrawTextAtVectorOperations.size; i++)
    { Vector<double>& current1=currentOps.theDrawTextAtVectorOperations [i].theVector;
      out << txtArrayName << "[" << k << "]" << "[" << i << "]=[";
      for (int j=0; j<theDimension; j++)
      { out << current1[j];
        if (j!=theDimension-1)
          out << ",";
      }
      out << "];\n";
    }
  }
  out << "var " << shiftX << "=" <<
  this->theBuffer.centerX[0]
  //100
  << ";\n";
  out << "var " <<  shiftY << "=" <<
  this->theBuffer.centerY[0]
  //100
  << ";\n";
  out << "var GraphicsUnitCone" << timesCalled << "=" << this->theBuffer.GraphicsUnit[0] << ";\n";
  out << "function " << functionConvertToXYName << "(vector){\n";
  out << "resultX=" << shiftX << "; resultY=" << shiftY << ";\nfor (i=0; i<" << theDimension << "; i++){\n";
  out << "resultX+=vector[i]*" << projName << "[i][0];\n";
  out << "resultY+=vector[i]*" << projName << "[i][1];\n}\n";
  out << "result=[resultX, resultY];\n";
  out << "return result;\n";
  out << "}\n";
  out << "var globalFrameCounter=0;\n";
  int frameDelay=3000;
  out
  << "function incrementGlobalFrameCounter" << timesCalled << "(){\n"
  << "globalFrameCounter++;\n"
  << "if (globalFrameCounter>=" << this->theFrames.size << ")\n"
  << "globalFrameCounter=0;\n"
  << "  ComputeProjections" << timesCalled << "();\n"
  << "  " << theDrawFunctionName << "();\n"
  << "window.setTimeout(\"incrementGlobalFrameCounter" << timesCalled << "()\"," << frameDelay << ");\n"
  << "}\n"
  ;
  out << "var " << theSurfaceName << "=0;\n"
  << "function " << theDrawFunctionName << "(){\n"
  << "  if (" << theSurfaceName << "==0)\n"
  << "    " << theInitFunctionName << "();\n"
  << "ComputeProjections" << timesCalled << "();\n";
  out << theSurfaceName << ".fillStyle=\"#FFFFFF\";\n";
  out << theSurfaceName << ".fillRect(0,0," << boss->DefaultHtmlWidth  << ", " << boss->DefaultHtmlHeight << ");\n";
  out
  << "for (s=0; s<" << Points1ArrayName << "[globalFrameCounter].length" << "; s++ ){\n  "
  << theSurfaceName << ".beginPath();\n"
  << theSurfaceName << ".strokeStyle=" << Points1ArrayNameColors <<  "[globalFrameCounter][s];\n"
  << theSurfaceName << ".moveTo("
  << functionConvertToXYName << "( " << Points1ArrayName << "[globalFrameCounter][s])[0],"
  << functionConvertToXYName << "( " << Points1ArrayName << "[globalFrameCounter][s])[1]); "
  << theSurfaceName  << ".lineTo("
  << functionConvertToXYName << "( " << Points2ArrayName << "[globalFrameCounter][s])[0],"
  << functionConvertToXYName << "( " << Points2ArrayName << "[globalFrameCounter][s])[1]); "
  << theSurfaceName << ".stroke();"
  << "\n}\n";
  out
  << "for (s=0; s<" << circArrayName << "[globalFrameCounter].length" << "; s++ ){\n  "
  << theSurfaceName << ".beginPath(); "
  << theSurfaceName << ".strokeStyle=\"#ff0000\"; "
  << theSurfaceName << ".arc("
  << functionConvertToXYName << "( " << circArrayName <<"[globalFrameCounter]" << "["
  << "s" << "])[0],"
  << functionConvertToXYName << "( " << circArrayName <<"[globalFrameCounter]" << "["
  << "s" << "])[1], "
  <<  circRadiiArrayName << "[globalFrameCounter][s]" << ", 0, 2*Math.PI); "
  << theSurfaceName << ".stroke();"
  << "\n}\n";
  out << "}\n"
  << "function " << theInitFunctionName << "(){\n"
  << theSurfaceName << " = document.getElementById(\"" << theCanvasId << "\").getContext(\"2d\");\n"
//  << "});\n"
  << "  ComputeProjections" << timesCalled << "();\n"
  << theDrawFunctionName << "();\n";
  if (this->theBuffer.BasisProjectionPlane.size>2)
    out << "window.setTimeout(\"changeProjectionPlaneOnTimer" << timesCalled << "()\",100);\n";
  if (this->theFrames.size>1)
    out <<"window.setTimeout(\"incrementGlobalFrameCounter" << timesCalled << "()\"," << frameDelay << ");\n";
  out << " }\n";
  out << "var selectedBasisIndexCone" << timesCalled << "=-1;\n"
  << "var clickTolerance=5;\n"
  << "function ptsWithinClickToleranceCone" << timesCalled << "(x1, y1, x2, y2)\n"
  << "{ if (x1-x2>clickTolerance || x2-x1>clickTolerance || y1-y2>clickTolerance || y2-y1>clickTolerance )\n    return false;\n  return true;\n}";

  out << "function MultiplyVector" << timesCalled << "(output, coeff)"
  << "{ for (var i=0; i<output.length; i++)\n"
  << "  output[i]*=coeff;\n"
  << "}\n"
  << "function AddVectorTimes" << timesCalled << "(output, inputVector, coeff)\n"
  << "{ for (var i=0; i<output.length; i++)\n"
  << "  output[i]+=inputVector[i]*coeff;\n"
  << "}\n"
  << "function ScaleToUnitLength" << timesCalled << "(vector)\n"
  << "{ MultiplyVector" << timesCalled << "(vector, 1/Math.sqrt(getScalarProduct" << timesCalled << "(vector,vector)));\n"
  << "}\n";
  out
  << "function RotateOutOfPlane" << timesCalled << "(input, orthoBasis1, orthoBasis2, oldTanSquared, newTanSquared, newX, newY, oldX, oldY)"
  << "{ var projection= " << cloneVector << "(orthoBasis1);\n"
  << "  var vComponent= " << cloneVector << "(input);\n"
  << "  var scal1= getScalarProduct" << timesCalled << "(orthoBasis1, input);\n"
  << "  var scal2= getScalarProduct" << timesCalled << "(orthoBasis2, input);\n"
  << "  MultiplyVector" << timesCalled << "(projection, scal1);\n"
  << "  AddVectorTimes" << timesCalled << "(projection, orthoBasis2, scal2);\n"
  << "  AddVectorTimes" << timesCalled << "(vComponent, projection, -1);\n"
  << "  var oldAngle=Math.atan(Math.sqrt(oldTanSquared));\n"
  << "  var newAngle=Math.atan(Math.sqrt(newTanSquared));\n"
  << "  if (isNaN(oldAngle) || isNaN(newAngle))\n"
  << "    return input;\n"
  << "  var angleChange=-oldAngle+newAngle;\n"
  << "  if (newX*oldX<0 && newY*oldY<0)\n"
//  <<  " {
//  << "    angleChange=oldAngle+newAngle;\n"
  << "    angleChange*=-1;\n"
//  << "  };"
//  << "  topBound=Math.PI/2;\n"
//  << "  bottomBound=-Math.PI/2;\n"
//  << "  while (angleChange>=topBound || angleChange<bottomBound)\n"
//  << "    if(angleChange>=topBound)\n"
//  << "      angleChange-=Math.PI;\n"
//  << "    else"
//  << "      angleChange+=Math.PI;\n"
  << "  projection=" << cloneVector << "(orthoBasis1);\n"
  << "  MultiplyVector" << timesCalled << "(projection, Math.cos(angleChange)*scal1-Math.sin(angleChange)*scal2);\n"
  << "  AddVectorTimes" << timesCalled << "(projection, orthoBasis2, Math.sin(angleChange)*scal1+Math.sin(angleChange)*scal2);\n"
  << "  var result= vComponent;\n"
  << "  AddVectorTimes" << timesCalled << "(result, projection, 1);\n"
  << "  return result;\n"
  << "}\n";
  out << "function MakeVectorE1AndE2orthonormal" << timesCalled << "(){\n"
  << "  AddVectorTimes" << timesCalled << "(VectorE2Cone" << timesCalled
  << ", VectorE1Cone" << timesCalled << ", -getScalarProduct"
  << timesCalled << "(VectorE1Cone" << timesCalled << ", VectorE2Cone" << timesCalled
  << ")/getScalarProduct" << timesCalled
  << "(VectorE1Cone" << timesCalled << ", VectorE1Cone" << timesCalled << "));\n"
  << "  ScaleToUnitLength" << timesCalled << "(VectorE1Cone" << timesCalled << ");\n"
  << "  ScaleToUnitLength" << timesCalled << "(VectorE2Cone" << timesCalled << ");\n"
  << "}\n";
  out << "function ComputeProjections" << timesCalled << "()\n"
  << "{ for (var i=0; i<" << theDimension << "; i++)\n"
  << "  { " << projName << "[i][0]=GraphicsUnitCone" << timesCalled << "*getScalarProduct" << timesCalled
  << "(VectorE1Cone" << timesCalled << ","  << eiBasis << "[i]);\n"
  << "    " << projName << "[i][1]=-GraphicsUnitCone" << timesCalled << "*getScalarProduct" << timesCalled
  << "(VectorE2Cone" << timesCalled << ", " << eiBasis << "[i]);\n"
  << "  }\n";
  for (int j=0; j<theDimension; j++)
    out << "  document.getElementById(\"" << textEbasisNamesReadOnly[0][j] << "\").innerHTML="
    << "VectorE1Cone" << timesCalled << "[" << j << "];\n"
    << "  document.getElementById(\"" << textEbasisNamesReadOnly[1][j] << "\").innerHTML="
    << "VectorE2Cone" << timesCalled << "[" << j << "];\n";
  out
  << "  for (var i=0; i<" << this->theBuffer.BasisToDrawCirclesAt.size << "; i++)\n"
  << "  { " << projBasisCircles << "[i][0]=GraphicsUnitCone" << timesCalled << "*getScalarProduct" << timesCalled
  << "(VectorE1Cone" << timesCalled << ", " << basisCircles << "[i]);\n"
  << "    " << projBasisCircles << "[i][1]=-GraphicsUnitCone" << timesCalled << "*getScalarProduct" << timesCalled
  << "(VectorE2Cone" << timesCalled << ", " << basisCircles << "[i]);\n"
  << "  }\n"
  << "}\n";
  out << "\nfunction getScalarProduct" << timesCalled << "(root1, root2)\n"
  << "{ var result=0;\n"
  << "  for (var i=0; i<" << theDimension << "; i++)\n"
  << "    for (var j=0; j<" << theDimension << "; j++)\n"
  << "      result+= root1[i]*root2[j]*BilinearForm" << timesCalled << "[i][j];\n"
  << "  return result;\n"
  << "}\n";
  out
  << "function getAngleFromXandY" << timesCalled << "(x, y, neighborX, neighborY)\n"
  << "{ var result;\n"
  << "  if (x!=0)\n"
  << "   result= Math.atan(y/x);\n"
  << "  else\n"
  << "    if (y>0)\n"
  << "      result= Math.PI/2;\n"
  << "    else\n"
  << "      result= -Math.PI/2;\n"
  << "  return result;\n"
  << "}\n";
  out
  << "function getAngleChange" << timesCalled << "(newX, newY, oldX, oldY)\n"
  << "{ var result=getAngleFromXandY" << timesCalled
  << "(newX, newY, oldX, oldY)-getAngleFromXandY" << timesCalled << "(oldX, oldY, newX, newY);\n"
  << "  topBound=Math.PI/2;\n"
  << "  bottomBound=-Math.PI/2;\n"
  << "  while (result>topBound || result< bottomBound)\n"
  << "    if (result>topBound)\n"
  << "      result-=Math.PI;\n"
  << "    else\n"
  << "      result+=Math.PI;\n"
  << "  return result;\n"
  << "}\n";
  out << "\nfunction changeBasis" << timesCalled <<  "(selectedIndex, newX, newY)\n"
  << "{ if (newX==0 && newY==0)\n"
  << "    return;\n"
  << "  var selectedRoot=" << basisCircles << "[selectedIndex];\n"
  << "  var selectedRootLength=getScalarProduct" << timesCalled << "(selectedRoot,selectedRoot);\n"
  << "  var projectionSelected=" << projBasisCircles << "[selectedIndex];\n"
  << "  var oldX=projectionSelected[0]/GraphicsUnitCone" << timesCalled << ";\n"
  << "  var oldY=-projectionSelected[1]/GraphicsUnitCone" << timesCalled << ";\n"
  << "  newX/=GraphicsUnitCone" << timesCalled << ";\n"
  << "  newY/=GraphicsUnitCone" << timesCalled << ";\n"
  << "  var epsilon=0.000000015;\n"
  << "  if (newX*newX+newY*newY>0.003)\n"
  << "  { var AngleChange= -getAngleChange" << timesCalled << "(newX, newY, oldX, oldY);\n"
  << "    var NewVectorE1=" << cloneVector << "(VectorE1Cone" << timesCalled << ");\n"
  << "    var NewVectorE2= " << cloneVector << "(VectorE2Cone" << timesCalled << ");\n"
  << "    MultiplyVector" << timesCalled << "(NewVectorE1, Math.cos(AngleChange));\n"
  << "    AddVectorTimes" << timesCalled << "(NewVectorE1, VectorE2Cone" << timesCalled << ", Math.sin(AngleChange));\n"
  << "    MultiplyVector" << timesCalled << "(NewVectorE2, Math.cos(AngleChange));\n"
  << "    AddVectorTimes" << timesCalled << "(NewVectorE2, VectorE1Cone" << timesCalled << ", -Math.sin(AngleChange));\n"
  << "    VectorE1Cone" << timesCalled << "=NewVectorE1;\n"
  << "    VectorE2Cone" << timesCalled << "=NewVectorE2;\n"
  << "  }\n"
  << "  if (newX*newX+newY*newY>0.0001)\n"
  << "  { var RootTimesE1=getScalarProduct" << timesCalled << "(selectedRoot, VectorE1Cone" << timesCalled << ");\n"
  << "    var RootTimesE2=getScalarProduct" << timesCalled << "(selectedRoot, VectorE2Cone" << timesCalled << ");\n"
  << "    var vOrthogonal=" << cloneVector << "(selectedRoot);\n"
  << "    var vProjection=" << cloneVector << "(VectorE1Cone" << timesCalled << ");\n"
  << "    MultiplyVector" << timesCalled << "(vProjection, RootTimesE1);\n"
  << "    AddVectorTimes" << timesCalled << "(vProjection, VectorE2Cone" << timesCalled << ", RootTimesE2" << ");\n"
  << "    AddVectorTimes" << timesCalled << "(vOrthogonal, vProjection, -1);\n"
  << "    var oldRatioProjectionOverHeightSquared = (oldX*oldX+oldY*oldY)/ (selectedRootLength-oldX*oldX-oldY*oldY);\n"
  << "    var newRatioProjectionOverHeightSquared = (newX*newX+newY*newY)/ (selectedRootLength-newX*newX-newY*newY);\n";
  if (theDimension>2)
  { out
    << "    if (getScalarProduct" << timesCalled << "(vOrthogonal, vOrthogonal)<epsilon && getScalarProduct" << timesCalled
    << "(vOrthogonal, vOrthogonal)>-epsilon)\n"
    << "    { vOrthogonal=" << cloneVector << "(" << eiBasis <<  "[2]);\n"
    << "    }\n";
  }
  out
  << "    if (getScalarProduct" << timesCalled << "(vOrthogonal, vOrthogonal)>epsilon || getScalarProduct" << timesCalled
  << "(vOrthogonal, vOrthogonal)<-epsilon)\n"
  << "    { if (oldRatioProjectionOverHeightSquared==0)\n"
  << "      { vProjection=" << cloneVector << "(VectorE1Cone" << timesCalled << ");\n"
  << "        MultiplyVector" << timesCalled << "(vProjection, -newX);\n"
  << "        AddVectorTimes" << timesCalled << "(vProjection, VectorE2Cone" << timesCalled << ", newY" << ");\n"
  << "      }\n"
  << "      ScaleToUnitLength" << timesCalled << "(vProjection);\n"
  << "      ScaleToUnitLength" << timesCalled << "(vOrthogonal);\n"
  << "      VectorE1Cone" << timesCalled << "=RotateOutOfPlane" << timesCalled << "(VectorE1Cone" << timesCalled
  << ", vProjection, vOrthogonal, oldRatioProjectionOverHeightSquared, newRatioProjectionOverHeightSquared, newX, newY, oldX, oldY);\n"
  << "      VectorE2Cone" << timesCalled << "=RotateOutOfPlane" << timesCalled << "(VectorE2Cone" << timesCalled
  << ", vProjection, vOrthogonal, oldRatioProjectionOverHeightSquared, newRatioProjectionOverHeightSquared, newX, newY, oldX, oldY);\n"
  << "    }\n"
  << "    MakeVectorE1AndE2orthonormal" << timesCalled << "();\n"
  << "    ComputeProjections" << timesCalled << "();\n"
  << "  }\n"
  << "}\n";

  out << "\nfunction clickCanvasCone" << timesCalled << "(cx,cy)\n"
  << "{ divPosX=0;\n  divPosY=0;\n  thePointer= document.getElementById(\"idCanvasCone" << timesCalled << "\");\n"
  << "  while(thePointer)  {\n  divPosX += thePointer.offsetLeft;\n  divPosY += thePointer.offsetTop;\n  thePointer = thePointer.offsetParent;\n  }"
  << "\n  posx=(cx-divPosX+document.body.scrollLeft-" << shiftX << ");"
  << "\n  posy=(cy-divPosY+document.body.scrollTop-" << shiftY << ");\n  selectedBasisIndexCone" << timesCalled <<"=-1;\n"
  << "if (ptsWithinClickToleranceCone" << timesCalled << "(posx,posy,0,0))" << "\nselectedBasisIndexCone" << timesCalled << "=-2;\n"
  <<  "for (i=0; i<" << theDimension << ";i++)  {\n if (ptsWithinClickToleranceCone" << timesCalled
  << "(posx, posy, " << projBasisCircles << "[i][0]" << ", " << projBasisCircles
  << "[i][1]" << "))\n"
  << "  selectedBasisIndexCone" << timesCalled << "=i;  \n}\n}\nfunction mouseMoveRedrawCone" << timesCalled << "(cx, cy)\n"
  << "{"
  << "\n if (selectedBasisIndexCone" << timesCalled << "==-1)\n    return;\n  divPosX=0;\n  divPosY=0;\n"
  << "  thePointer= document.getElementById(\"idCanvasCone"<< timesCalled << "\");\n  while(thePointer)\n  { divPosX += thePointer.offsetLeft;\n"
  << "    divPosY += thePointer.offsetTop;\n    thePointer = thePointer.offsetParent;\n  }\n"
  << "  posx=(cx-divPosX+document.body.scrollLeft-" << shiftX << ");\n"
  << "  posy=-(cy-divPosY+document.body.scrollTop-" << shiftY << ");\n"
  << "if (selectedBasisIndexCone" << timesCalled << "==-2)\n{ shiftXCone" << timesCalled << "=(cx-divPosX+document.body.scrollLeft);\n"
  << shiftY << "=(cy-divPosY+document.body.scrollTop);\n  }  else\n"
  << "{ changeBasis" << timesCalled << "(selectedBasisIndexCone" << timesCalled << ", posx, posy);\n  }\n  "
  << theDrawFunctionName << " ();\n}\n";
  out << "\n tempDiv=document.getElementById(\"" << theCanvasId << "\");";
  out << "\ntempDiv.addEventListener (\"DOMMouseScroll\", mouseHandleWheelCone"
  << timesCalled << ", true);\n";
  out << "function mouseHandleWheelCone" << timesCalled << "(theEvent){\n"
  << "theEvent = theEvent ? theEvent : window.event;\n theEvent.preventDefault();\ntheEvent.stopPropagation();\ntheWheelDelta = theEvent.detail ? theEvent.detail * -1 : theEvent.wheelDelta / 40;\n"
  << "GraphicsUnitCone" << timesCalled << "+=theWheelDelta;\n"
  << "if (GraphicsUnitCone" << timesCalled << "==0)\n GraphicsUnitCone" << timesCalled << "=3;\n"
//  << "for (i=0; i<" << theDimension << "; i++){\n "
//  << basisName << "[i][0]*=GraphicsUnitCone" << timesCalled << "/(GraphicsUnitCone" << timesCalled << "-theWheelDelta);\n"
//  << basisName << "[i][1]*=GraphicsUnitCone" << timesCalled << "/(GraphicsUnitCone" << timesCalled << "-theWheelDelta);\n"
//  << "}\n"
  << theDrawFunctionName << "();\n}\n";
  out << theInitFunctionName << "();\n"
  << "</script>\n"
  ;
  return out.str();
}
