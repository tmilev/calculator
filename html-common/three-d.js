"use strict";
var calculatorCanvases=new Object;
var firstCriticalRunTimeError="";
var firstCanvas=undefined;

function calculatorError(x)
{ if (firstCriticalRunTimeError!="")
    return;  
  firstCriticalRunTimeError=x;
  if (firstCriticalRunTimeError!="" && firstCanvas!=undefined)
  { firstCanvas.textErrors=firstCriticalRunTimeError+" All further error messages are suppressed.";
    firstCanvas.showMessages();
  }
}

function vectorScalarVector(s,t)
{ var result=0;
  if (s.length!=t.length)
    calculatorError("Scalar product of vectors of different length: " + s + " and " + t+ ".");
  for (var i=0; i<s.length; i++)
    result+=s[i]*t[i];
  return result;
}

function vectorTimesScalar(vector, s)
{ for (var i=0;i<vector.length; i++)
    vector[i]*=s;
}

function vectorPlusVector(left, right)
{ var output=new Array(left.length);
  for (var i=0;i<left.length; i++)
    output[i]=left[i]+right[i];
  return output;
}

function vectorCrossVector(left, right)
{ return [left[1]*right[2]-left[2]*right[1], -left[0]*right[2]+left[2]*right[0], left[0]*right[1]-left[1]*right[0] ];
}

function vectorMinusVector(left, right)
{ var output=new Array(left.length);
  for (var i=0;i<left.length; i++)
    output[i]=left[i]-right[i];
  return output;
}

function vectorAddVectorTimesScalar(output, otherVector, scalar)
{ for (var i=0;i<output.length; i++)
    output[i]+=otherVector[i]*scalar;
}

function vectorLength(vector)
{ return Math.sqrt(vectorScalarVector(vector, vector));
}

function deepCopy(from, to)
{ to = new Array(from.length)
  for (var i=0; i<from.length; i++)
    if (from[i] instanceof Array)
      deepCopy(from[i], to[i]);
    else
      to[i]=from[i];
}

function vectorNormalize(vector)
{ vectorTimesScalar(vector, 1/vectorLength(vector));
}

function Point(inputLocation, inputColor)
{ this.location=inputLocation;
  this.color=inputColor;
}

function Patch(inputBase, inputEdge1, inputEdge2, inputColor)
{ this.base=inputBase;
  this.edge1=inputEdge1;
  this.edge2=inputEdge2;
  this.color=inputColor;
  this.v1=vectorPlusVector(this.base, this.edge1);
  this.v2=vectorPlusVector(this.base, this.edge2);
  this.vEnd=vectorPlusVector(this.v1, this.edge2);
}

function Contour(inputPoints, inputColor)
{ this.thePoints=inputPoints;
  this.color=inputColor;
}

function getPosXPosYObject(theObject, cx,cy)
{ var divPosX=0;
  var divPosY=0;
  var thePointer= theObject;
  while(thePointer)
  { divPosX += thePointer.offsetLeft;
    divPosY += thePointer.offsetTop;
    thePointer = thePointer.offsetParent;
  }
  return [cx-divPosX+document.body.scrollLeft,
          cy-divPosY+document.body.scrollTop];
}

function getAngleChangeMathScreen(newX, newY, oldX, oldY)
{ var result= Math.atan2(newY, newX) - Math.atan2(oldY, oldX);
  if (result>Math.PI)
    result-=Math.PI;
  else if (result< -Math.PI)
    result+=Math.PI;
  return result;
}

function calculatorGetCanvas(inputCanvas)
{ if (calculatorCanvases[inputCanvas.id]==undefined)
  { calculatorCanvases[inputCanvas.id]=
    { theIIIdObjects:
      { thePatches: [],
        theContours: [],
        thePoints: [],
      },
      surface: inputCanvas.getContext("2d"),
      canvasContainer: inputCanvas,
      canvasId: inputCanvas.id,
      screenBasisUser: [[2,1,0],[0,1,1]],
      screenNormal: [],
      screenBasisOrthonormal: [],
      zBufferColCount: 20,
      zBufferRowCount: 20,
      zBuffer: [],
      zBufferIndexStrip:[],
      bufferDeltaX: 0,
      bufferDeltaY: 0,
      boundingBoxMathScreen: [[-0.01, -0.01], [0.01, 0.01]],
      width: inputCanvas.width,
      height: inputCanvas.height,
      centerX: inputCanvas.width/2,
      centerY: inputCanvas.height/2,
      scale: 50,
      mousePosition: [],
      clickedPosition: [],
      unitRay: [],
      rayComponent: [],
      positionDelta: [],
      spanMessages: undefined,
      textMouseInfo: "",
      textProjectionInfo: "",
      textErrors: "",
      angleNormal: 0,
      oldAngleNormal: 0,
      newAngleNormal: 0,
      anglePolar: 0,
      selectedElement: "",
      selectedVector: [],
      selectedScreenBasis: [],
      selectedScreenProjectionNormalized: [],
      selectedScreenNormal: [],
      selectedPolarAngleChange: 0,
      drawPatch: function(base, edge1, edge2, color)
      { this.theIIIdObjects.thePatches.push(new Patch(base, edge1, edge2, color));
      },
      drawPoint: function (inputPoint, inputColor)
      { this.theIIIdObjects.thePoints.push(new Point(inputPoint, inputColor));
      },
      drawLine: function (leftPt, rightPt, inputColor)
      { var newContour= new Object;
        newContour.thePoints=[];
        newContour.color=inputColor;
        var numPoints=100;
        var incrementScalar=1/numPoints;
        var incrementVector=vectorMinusVector(rightPt, leftPt);
        vectorTimesScalar(incrementVector, incrementScalar);
        var currentPoint=leftPt.slice();
        for (var i=0; i<numPoints+1; i++)
        { newContour.thePoints[i]=currentPoint;
          currentPoint=vectorPlusVector(currentPoint, incrementVector);
        }
        this.theIIIdObjects.theContours.push(newContour);
      },
      computePatch: function(thePatch)
      { thePatch.normalScreen1= vectorCrossVector(this.screenNormal, thePatch.edge1);
        thePatch.normalScreen2= vectorCrossVector(this.screenNormal, thePatch.edge2);
        thePatch.normal= vectorCrossVector(thePatch.edge1, thePatch.edge2);
        thePatch.internalPoint=thePatch.base.slice();
        vectorAddVectorTimesScalar(thePatch.internalPoint, thePatch.edge1, 0.5);
        vectorAddVectorTimesScalar(thePatch.internalPoint, thePatch.edge2, 0.5);
      },
      pointIsBehindPatch: function(thePoint, thePatch)
      { if (vectorScalarVector(vectorMinusVector(thePoint, thePatch.base), thePatch.normalScreen1) *
            vectorScalarVector(vectorMinusVector(thePatch.internalPoint, thePatch.base), thePatch.normalScreen1)
            >=0)
          if (vectorScalarVector(vectorMinusVector(thePoint, thePatch.vEnd), thePatch.normalScreen1) *
              vectorScalarVector(vectorMinusVector(thePatch.internalPoint, thePatch.vEnd), thePatch.normalScreen1)
              >=0)
            if (vectorScalarVector(vectorMinusVector(thePoint, thePatch.base), thePatch.normalScreen2) *
                vectorScalarVector(vectorMinusVector(thePatch.internalPoint, thePatch.base), thePatch.normalScreen2)
                >=0)
              if (vectorScalarVector(vectorMinusVector(thePoint, thePatch.vEnd), thePatch.normalScreen2) *
                  vectorScalarVector(vectorMinusVector(thePatch.internalPoint, thePatch.vEnd), thePatch.normalScreen2)
                  >=0)
              { if (vectorScalarVector(vectorMinusVector(thePoint, thePatch.base), thePatch.normal) *
                    vectorScalarVector(this.screenNormal, thePatch.normal)<-0.00000001)
                  return true;
              }
        return false;
      },
      pointIsInForeGround: function(thePoint, containerPatch)
      { var thePatches=this.theIIIdObjects.thePatches;
        for (var i=0; i<thePatches.length; i++)
        { if (containerPatch===i)
            continue;
          if (this.pointIsBehindPatch(thePoint, thePatches[i]))
            return false;
        }
        return true;
      },
      paintMouseInfo: function()
      { if (this.selectedElement!="default" || this.selectedElement==undefined || this.selectedVector==[] || this.selectedVector==undefined)
          return;
        if (this.selectedVector.length==0 || this.selectedVector.length==undefined)
          return;
        if (true)
          return;
        var currentPt;
        this.surface.beginPath();
        this.surface.setLineDash([4,4]);
        this.surface.lineWidth=2;
        this.surface.strokeStyle="green";
        currentPt=this.coordsMathToSelectedScreen(this.selectedVector);
        this.surface.moveTo(currentPt[0], currentPt[1]);
        currentPt=this.coordsMathToSelectedScreen([0,0,0]);
        this.surface.lineTo(currentPt[0], currentPt[1]);
        this.surface.stroke();
        this.surface.beginPath();
        this.surface.setLineDash([]);
        this.surface.lineWidth=2;
        this.surface.strokeStyle="green";
        currentPt=this.coordsMathToScreen(this.selectedVector);
        this.surface.moveTo(currentPt[0], currentPt[1]);
        currentPt=this.coordsMathToScreen([0,0,0]);
        this.surface.lineTo(currentPt[0], currentPt[1]);
        this.surface.stroke();
      },
      paintOneContour: function(theContour)
      { if (theContour.thePoints.length<2)
          return;
        var theSurface=this.surface;
        var thePts=theContour.thePoints;
        //console.log("line start\n");
        var currentPt=this.coordsMathToScreen(thePts[0]);
        var oldIsInForeGround=this.pointIsInForeGround(theContour.thePoints[0],-1);
        var newIsInForeground;
        var dashIsOn=false;
        theSurface.setLineDash([]);
        theSurface.beginPath();
//        theSurface.setLineDash([]);
        theSurface.strokeStyle=theContour.color;
        theSurface.lineWidth=1;
        theSurface.moveTo(currentPt[0], currentPt[1]);
        for (var i=1; i<thePts.length; i++)
        { newIsInForeground=this.pointIsInForeGround(theContour.thePoints[i],-1);
          if (!newIsInForeground && !oldIsInForeGround && !dashIsOn)
          { if (i>1)
            { theSurface.stroke();
              theSurface.setLineDash([4,4]);
              theSurface.beginPath();
//              theSurface.setLineDash([4,4]);
              theSurface.moveTo(currentPt[0], currentPt[1]);
            }
            theSurface.setLineDash([4,4]);
            //console.log("set dash\n");
            dashIsOn=true;

          } else if (dashIsOn && newIsInForeground)
          { theSurface.stroke();
            theSurface.setLineDash([]);
            theSurface.beginPath();
//            theSurface.setLineDash([]);
            //console.log("removed dash\n");
            theSurface.moveTo(currentPt[0], currentPt[1]);
            dashIsOn=false;
          }
          currentPt=this.coordsMathToScreen(thePts[i]);
          theSurface.lineTo(currentPt[0], currentPt[1]);
          oldIsInForeGround=newIsInForeground;
        }
        theSurface.stroke();
        //console.log("line end\n");
      },
      paintOnePatch: function(thePatch)
      { var theSurface=this.surface;
        theSurface.fillStyle=thePatch.color;
        theSurface.beginPath();
        var
        theCoords=this.coordsMathToScreen(thePatch.base);
        theSurface.moveTo(theCoords[0], theCoords[1]);
        theCoords=this.coordsMathToScreen(thePatch.v1);
        theSurface.lineTo(theCoords[0], theCoords[1]);
        theCoords=this.coordsMathToScreen(thePatch.vEnd);
        theSurface.lineTo(theCoords[0], theCoords[1]);
        theCoords=this.coordsMathToScreen(thePatch.v2);
        theSurface.lineTo(theCoords[0], theCoords[1]);
        theCoords=this.coordsMathToScreen(thePatch.base);
        theSurface.lineTo(theCoords[0], theCoords[1]);
        theSurface.fill();
//        theSurface.stroke();
      },
      paintOnePoint: function(thePoint)
      { var theSurface=this.surface;
        var isInForeGround=this.pointIsInForeGround(thePoint.location, -1);
        theSurface.beginPath();
        theSurface.strokeStyle=thePoint.color;
        theSurface.fillStyle=thePoint.color;
        var theCoords=this.coordsMathToScreen(thePoint.location);
        theSurface.arc(theCoords[0], theCoords[1],3, 0, Math.PI*2);
        if (isInForeGround)
          theSurface.fill();
        else
          theSurface.stroke();
      },
      getExtremePoint: function(indexToCompareBy, getLarger, pt1, pt2, pt3, pt4)
      { var result=pt1[indexToCompareBy];
        if (getLarger==1)
        { if (result<pt2[indexToCompareBy])
            result=pt2[indexToCompareBy];
          if (result<pt3[indexToCompareBy])
            result=pt3[indexToCompareBy];
          if (result<pt4[indexToCompareBy])
            result=pt4[indexToCompareBy];
        } else
        { if (result>pt2[indexToCompareBy])
            result=pt2[indexToCompareBy];
          if (result>pt3[indexToCompareBy])
            result=pt3[indexToCompareBy];
          if (result>pt4[indexToCompareBy])
            result=pt4[indexToCompareBy];
        }
        return result;
      },
      coordsMathScreenToBufferIndicesROWSFloat: function (input)
      { return (this.zBufferRowCount)*(input-this.boundingBoxMathScreen[0][1])/(this.boundingBoxMathScreen[1][1]-this.boundingBoxMathScreen[0][1]);
      },
      coordsMathScreenToBufferIndicesROWS: function (input)
      { var result= Math.floor(this.coordsMathScreenToBufferIndicesROWSFloat(input));
        if (result>=this.zBufferRowCount)
          result--;
        return result;
      },
      coordsMathScreenToBufferIndicesCOLSFloat: function (input)
      { return (this.zBufferColCount)*(input-this.boundingBoxMathScreen[0][0])/(this.boundingBoxMathScreen[1][0]-this.boundingBoxMathScreen[0][0]);
      },
      coordsMathScreenToBufferIndicesCOLS: function (input)
      { var result=Math.floor(this.coordsMathScreenToBufferIndicesCOLSFloat(input));
        if (result>=this.zBufferColCount)
          result--;
        return result;
      },
      coordsMathScreenToBufferIndices: function (input)
      { var row=this.coordsMathScreenToBufferIndicesROWS(input[1]);
        var col=this.coordsMathScreenToBufferIndicesCOLS(input[0]);
        if (row<0 || row>=this.zBufferRowCount || col<0 || col>=this.zBufferColCount)
          console.log("point with math-screen coords: " + input + " is out of the bounding box");
        return [row,col];
      },
      accountOnePointMathCoordsInBufferStrip: function(row, thePoint, patchIndex)
      { if (row<0 || row>=this.zBufferIndexStrip.length)
          return;
        var bufferCoords= this.coordsMathScreenToBufferIndices(this.coordsMathToMathScreen(thePoint));
        //If there were no rounding errors, row would be equal
        //to bufferCoords[0]. However since there will be rounding errors,
        //the row is passed instead as an argument.
        if (this.zBufferIndexStrip[row][0]==-1)
        { this.zBufferIndexStrip[row][0]=bufferCoords[1];
          this.zBufferIndexStrip[row][1]=bufferCoords[1];
          return;
        }
        if (bufferCoords[1]< this.zBufferIndexStrip[row][0])
          this.zBufferIndexStrip[row][0]=bufferCoords[1];
        if (bufferCoords[1]>this.zBufferIndexStrip[row][1])
          this.zBufferIndexStrip[row][1]=bufferCoords[1];
      },
      accountEdgeInBufferStrip: function(base, edgeVector, patchIndex)
      { var end=vectorPlusVector(base, edgeVector);
        var baseCopy=base.slice();
        var edgeCopy=edgeVector.slice();
        var lowFloat=this.coordsMathScreenToBufferIndicesROWSFloat(this.coordsMathToMathScreen(baseCopy)[1]);
        var highFloat=this.coordsMathScreenToBufferIndicesROWSFloat(this.coordsMathToMathScreen(end)[1]);
        if (lowFloat>highFloat)
        { var temp=lowFloat;
          lowFloat=highFloat;
          highFloat=temp;
          var temp2=end;
          end=baseCopy;
          baseCopy=temp2;
          vectorTimesScalar(edgeCopy,-1);
        }
        this.accountOnePointMathCoordsInBufferStrip(Math.floor(lowFloat), baseCopy, patchIndex);
        this.accountOnePointMathCoordsInBufferStrip(Math.floor(highFloat), end, patchIndex);
        if (lowFloat==highFloat)
          return;
        var currentPoint;
        for (var i=Math.ceil(lowFloat); i<highFloat; i++)
        { currentPoint=baseCopy.slice();
          vectorAddVectorTimesScalar(currentPoint, edgeCopy, (i-lowFloat)/(highFloat-lowFloat));
          this.accountOnePointMathCoordsInBufferStrip(i, currentPoint, patchIndex);
          this.accountOnePointMathCoordsInBufferStrip(i-1, currentPoint, patchIndex);
        }
      },
      accountOnePatch: function(patchIndex)
      { var thePatch=this.theIIIdObjects.thePatches[patchIndex];
        var pt1=this.coordsMathToMathScreen(thePatch.base);
        var pt2=this.coordsMathToMathScreen(thePatch.v1);
        var pt3=this.coordsMathToMathScreen(thePatch.v2);
        var pt4=this.coordsMathToMathScreen(thePatch.vEnd);
        var lowFloat=this.getExtremePoint(1, 0, pt1, pt2, pt3, pt4);
        var highFloat=this.getExtremePoint(1, 1, pt1, pt2, pt3, pt4);
        var low=this.coordsMathScreenToBufferIndicesROWS(lowFloat);
        var high=this.coordsMathScreenToBufferIndicesROWS(highFloat);
        for (var i=low; i<=high; i++)
        { this.zBufferIndexStrip[i][0]=-1;
          this.zBufferIndexStrip[i][1]=-1;
        }
        this.accountEdgeInBufferStrip(thePatch.base, thePatch.edge1, patchIndex);
        this.accountEdgeInBufferStrip(thePatch.base, thePatch.edge2, patchIndex);
        this.accountEdgeInBufferStrip(thePatch.v1, thePatch.edge2,   patchIndex);
        this.accountEdgeInBufferStrip(thePatch.v2, thePatch.edge1,   patchIndex);
        for (var i=low; i<=high; i++)
          for (var j=this.zBufferIndexStrip[i][0]; j<=this.zBufferIndexStrip[i][1]; j++)
          { if (i==-1 || j==-1)
              continue;
            this.zBuffer[i][j].push(patchIndex);
          }
      },
      computeBoundingBoxAccountPoint: function(input)
      { var theV=this.coordsMathToMathScreen(input);
        if (theV[0]<this.boundingBoxMathScreen[0][0])
          this.boundingBoxMathScreen[0][0]=theV[0];
        if (theV[1]<this.boundingBoxMathScreen[0][1])
          this.boundingBoxMathScreen[0][1]=theV[1];
        if (theV[0]>this.boundingBoxMathScreen[1][0])
          this.boundingBoxMathScreen[1][0]=theV[0];
        if (theV[1]>this.boundingBoxMathScreen[1][1])
          this.boundingBoxMathScreen[1][1]=theV[1];
      },
      computeBoundingBox: function()
      { var thePatches=this.theIIIdObjects.thePatches;
        var theContours=this.theIIIdObjects.theContours;
        var thePoints=this.theIIIdObjects.thePoints;
        for (var i=0; i<thePatches.length; i++)
        { this.computeBoundingBoxAccountPoint(thePatches[i].base);
          this.computeBoundingBoxAccountPoint(thePatches[i].v1);
          this.computeBoundingBoxAccountPoint(thePatches[i].v2);
          this.computeBoundingBoxAccountPoint(thePatches[i].vEnd);
        }
        for (var i=0; i<theContours.length; i++)
          for (var j=0; j<theContours[i].thePoints.length; j++)
            this.computeBoundingBoxAccountPoint(theContours[i].thePoints[j]);
        for (var i=0; i<thePoints.length; i++)
          this.computeBoundingBoxAccountPoint(thePoints[i]);
      },
      computeBuffers: function()
      { var thePatches=this.theIIIdObjects.thePatches;
        for (var i=0; i<this.zBuffer.length; i++)
          for (var j=0; j<this.zBuffer[i].length; j++)
            this.zBuffer[i][j]=[];
        this.computeBoundingBox();
        this.bufferDeltaX=(this.boundingBoxMathScreen[1][0]-this.boundingBoxMathScreen[0][0])/this.zBufferColCount;
        this.bufferDeltaY=(this.boundingBoxMathScreen[1][1]-this.boundingBoxMathScreen[0][1])/this.zBufferRowCount;
        for (var i=0; i<thePatches.length; i++)
          this.accountOnePatch(i);
      },
      redraw: function()
      { var theContours=this.theIIIdObjects.theContours;
        var thePatches=this.theIIIdObjects.thePatches;
        var thePoints=this.theIIIdObjects.thePoints;
        var theSurface=this.surface;
        theSurface.clearRect(0, 0, this.width, this.height);
        this.computeBuffers();
        this.paintZbuffer();
        for (var i=0; i<thePatches.length; i++)
          this.computePatch(thePatches[i]);
        for (var i=0; i<thePatches.length; i++)
          this.paintOnePatch(thePatches[i]);
        for (var i=0; i<theContours.length; i++)
          this.paintOneContour(theContours[i]);
        for (var i=0; i<thePoints.length; i++)
          this.paintOnePoint(thePoints[i]);
        this.paintMouseInfo();
      },
      allocateZbuffer: function()
      { if (this.zBufferRowCount<1)
          this.zBufferRowCount=1;
        if (this.zBufferColCount<1)
          this.zBufferColCount=1;
        if (this.zBuffer.length<this.zBufferRowCount)
        { this.zBuffer=new Array(this.zBufferRowCount);
          this.zBufferIndexStrip=new Array(this.zBufferRowCount);
          for (var i=0; i< this.zBufferRowCount; i++)
          { this.zBuffer[i]= new Array(this.zBufferColCount);
            for (var j=0; j< this.zBufferColCount; j++)
              this.zBuffer[i][j]=[];
            this.zBufferIndexStrip[i]=[-1,-1];
          }
        }
      },
      getBufferBox: function(row, col)
      { return [[this.boundingBoxMathScreen[0][0]+this.bufferDeltaX*col,
                 this.boundingBoxMathScreen[0][1]+this.bufferDeltaY*row],
                [this.boundingBoxMathScreen[0][0]+this.bufferDeltaX*(col+1),
                 this.boundingBoxMathScreen[0][1]+this.bufferDeltaY*(row+1)]];
      },
      paintZbuffer: function()
      { var theSurface=this.surface;
        theSurface.strokeStyle="gray";
        theSurface.setLineDash([]);
        for (var i=0; i<this.zBuffer.length; i++)
          for (var j=0; j< this.zBuffer[i].length; j++)
          { var bufferBox=this.getBufferBox(i,j);
            var bufferBoxLowLeft=this.coordsMathScreenToScreen(bufferBox[0]);
            var bufferBoxTopRight=this.coordsMathScreenToScreen(bufferBox[1]);
            theSurface.beginPath();
            theSurface.moveTo(bufferBoxLowLeft[0],  bufferBoxLowLeft [1]);
            theSurface.lineTo(bufferBoxTopRight[0], bufferBoxLowLeft [1]);
            theSurface.lineTo(bufferBoxTopRight[0], bufferBoxTopRight[1]);
            theSurface.lineTo(bufferBoxLowLeft[0],  bufferBoxTopRight[1]);
            theSurface.lineTo(bufferBoxLowLeft[0],  bufferBoxLowLeft [1]);
            theSurface.stroke();
            if (this.zBuffer[i][j].length!=0)
              theSurface.fill();
          }
      },
      computeBasisFromNormal: function(inputNormal)
      { if (inputNormal[0]!=0)
        { this.screenBasisUser[0]=[-inputNormal[1], inputNormal[0], 0];
          this.screenBasisUser[1]=[-inputNormal[2], 0, inputNormal[0]];
        } else if (inputNormal[1]!=0)
        { this.screenBasisUser[0]=[1, 0, 0];
          this.screenBasisUser[1]=[0, inputNormal[2], -inputNormal[1]];
        } else
        { this.screenBasisUser[0]=[1, 0, 0];
          this.screenBasisUser[1]=[0, 1, 0];
        }
      },
      computeBasis: function ()
      { //if (this.screenBasisOrthonormal.length<2)
        //  this.screenBasisOrthonormal.length=2;
        this.screenBasisOrthonormal[0]=this.screenBasisUser[0].slice();
        this.screenBasisOrthonormal[1]=this.screenBasisUser[1].slice();
        var e1=this.screenBasisOrthonormal[0];
        var e2=this.screenBasisOrthonormal[1];
        vectorNormalize(e1);
        vectorAddVectorTimesScalar(e2, e1, - vectorScalarVector(e1,e2));
        vectorNormalize(e2);
        this.screenNormal=vectorCrossVector(e1, e2);
        this.textProjectionInfo=
        "e1: " + this.screenBasisOrthonormal[0]+
        "<br>e2: " + this.screenBasisOrthonormal[1]+
        "<br>selected e1: " + this.selectedScreenBasis[0]+
        "<br>selected e2: "+ this.selectedScreenBasis[1]
        ;
      },
      init: function()
      { document.getElementById(this.canvasId).addEventListener("DOMMouseScroll", calculatorCanvasMouseWheel, true);
        document.getElementById(this.canvasId).addEventListener("mousewheel", calculatorCanvasMouseWheel, true);
        this.spanMessages=document.getElementById(this.canvasId+"Messages");
        this.spanCriticalErrors=document.getElementById(this.canvasId+"CriticalErrors");
        this.computeBasis();
        if (this.zBuffer.length==0)
          this.allocateZbuffer();
      },
      coordsMathScreenToMath: function(theCoords)
      { var output=this.screenBasisOrthonormal[0].slice();
        vectorTimesScalar(output, theCoords[0]);
        vectorAddVectorTimesScalar(output, this.screenBasisOrthonormal[1], theCoords[1]);
        return output;
      },
      coordsMathToMathScreen: function(vector)
      { return [vectorScalarVector(vector, this.screenBasisOrthonormal[0]),
                vectorScalarVector(vector, this.screenBasisOrthonormal[1])];
      },
      projectToMathScreen: function(vector)
      { var output=this.screenBasisOrthonormal[0].slice();
        vectorTimesScalar(output, vectorScalarVector(vector, this.screenBasisOrthonormal[0]));
        vectorAddVectorTimesScalar(output, this.screenBasisOrthonormal[1], vectorScalarVector(vector, this.screenBasisOrthonormal[1]));
        return output;
      },
      projectToSelectedMathScreen: function(vector)
      { var output=this.selectedScreenBasis[0].slice();
        vectorTimesScalar(output, vectorScalarVector(vector, this.selectedScreenBasis[0]));
        vectorAddVectorTimesScalar(output, this.selectedScreenBasis[1], vectorScalarVector(vector, this.selectedScreenBasis[1]));
        return output;
      },
      coordsMathToScreen: function(vector)
      { return [this.scale*vectorScalarVector(vector, this.screenBasisOrthonormal[0])+this.centerX,
           (-1)*this.scale*vectorScalarVector(vector, this.screenBasisOrthonormal[1])+this.centerY];
      },
      coordsMathToSelectedScreen: function(vector)
      { return [this.scale*vectorScalarVector(vector, this.selectedScreenBasis[0])+this.centerX,
           (-1)*this.scale*vectorScalarVector(vector, this.selectedScreenBasis[1])+this.centerY];
      },
      getPosXPosY: function (cx, cy)
      { return getPosXPosYObject(this, cx, cy);
      },
      coordsScreenToMathScreen: function(screenX, screenY)
      { var xyScreen= getPosXPosYObject(this.canvasContainer, screenX,screenY);
        return [ (xyScreen[0]-this.centerX)/this.scale, (this.centerY-xyScreen[1])/this.scale];
      },
      coordsMathScreenToScreen: function(theCoords)
      { return [this.scale*theCoords[0]+this.centerX, this.centerY-this.scale*theCoords[1]];
      },
      rotateOutOfPlane: function (input, orthoBasis1, orthoBasis2, theAngle)
      { var vComponent= input.slice();
        var scal1= vectorScalarVector(orthoBasis1, input);
        var scal2= vectorScalarVector(orthoBasis2, input);
        var projection= orthoBasis1.slice();
        vectorTimesScalar(projection, scal1);
        vectorAddVectorTimesScalar(projection, orthoBasis2, scal2);
        vectorAddVectorTimesScalar(vComponent, projection, -1);
        projection=orthoBasis1.slice();
        vectorTimesScalar(projection, Math.cos(theAngle)*scal1-Math.sin(theAngle)*scal2);
        vectorAddVectorTimesScalar(projection, orthoBasis2, Math.sin(theAngle)*scal1+Math.sin(theAngle)*scal2);
        return vectorPlusVector(vComponent, projection);
      },
      rotateAfterCursorDefaultGreatNormalCircle: function(doRotateRadially)
      //please use only with doRotateRadially=false
      //the option creates confusing user interface, needs to be fixed/reworked
      { if (this.mousePosition.length==0)
          return;
        if (doRotateRadially)
        {/* this.selectedPolarAngleChange=-getAngleChangeMathScreen(newX, newY, oldX, oldY);
          vectorTimesScalar(newE1, Math.cos(this.selectedPolarAngleChange));
          vectorAddVectorTimesScalar(newE1, oldE2, Math.sin(this.selectedPolarAngleChange));
          vectorTimesScalar(newE2, Math.cos(this.selectedPolarAngleChange));
          vectorAddVectorTimesScalar(newE2, oldE1, -Math.sin(this.selectedPolarAngleChange));
          this.screenBasisOrthonormal[0] = newE1;
          this.screenBasisOrthonormal[1] = newE2;*/
        }
        var oldX=this.clickedPosition[0];
        var oldY=this.clickedPosition[1];
        var newX=this.mousePosition[0];
        var newY=this.mousePosition[1];
        this.positionDelta=[newX-oldX, newY-oldY];
        this.unitRay= this.clickedPosition.slice();
        vectorNormalize(this.unitRay);
        this.rayComponent=this.unitRay.slice();
        vectorTimesScalar(this.rayComponent, vectorScalarVector(this.mousePosition, this.unitRay));
        var osculatingOldX=vectorLength(this.clickedPosition);
        var osculatingOldCos=osculatingOldX/vectorLength(this.selectedVector);
//        var osculatingOldSin=1-osculatingOldCos*osculatingOldCos;
        var osculatingNewX=vectorScalarVector(this.mousePosition, this.unitRay)
        //vectorLength(this.mousePosition)
        ;
        var osculatingNewCos=osculatingNewX/vectorLength(this.selectedVector);
//        var osculatingNewSin=1-osculatingNewCos*osculatingNewCos;
//        var osculatingOldY=vectorScalarVector(this.selectedVector, selectedOrthogonal);
        this.newAngleNormal=Math.acos(osculatingNewCos);
        this.oldAngleNormal=Math.acos(osculatingOldCos);
        this.angleNormal=this.oldAngleNormal-this.newAngleNormal;
        if (isNaN(this.angleNormal))
          return;
        var newE1 = this.selectedScreenBasis[0].slice();
        var newE2 = this.selectedScreenBasis[1].slice();
        newE1=this.rotateOutOfPlane(newE1, this.selectedScreenProjectionNormalized, this.selectedScreenNormal, this.angleNormal);
        newE2=this.rotateOutOfPlane(newE2, this.selectedScreenProjectionNormalized, this.selectedScreenNormal, this.angleNormal);
        this.screenBasisUser[0] = newE1.slice();
        this.screenBasisUser[1] = newE2.slice();
        this.computeBasis();
        this.redraw();
      },
      rotateAfterCursorDefaultCartesian: function()
      { if (this.mousePosition.length==0)
          return;
        this.positionDelta=[this.mousePosition[0]-this.clickedPosition[0], this.mousePosition[1]-this.clickedPosition[1]];
        var xyThreshhold=5;
        if (this.positionDelta[0]>xyThreshhold)
          this.positionDelta[0]=xyThreshhold;
        if (this.positionDelta[1]>xyThreshhold)
          this.positionDelta[1]=xyThreshhold;
        if (this.positionDelta[0]<-xyThreshhold)
          this.positionDelta[0]=-xyThreshhold;
        if (this.positionDelta[1]<-xyThreshhold)
          this.positionDelta[1]=-xyThreshhold;
        this.computeBasis();
        this.redraw();
      },
      rotateAfterCursorDefault: function()
      { if (this.mousePosition.length==0)
          return;
        this.rotateAfterCursorDefaultGreatNormalCircle(false);
      },
      mouseMove: function(screenX, screenY)
      { if (this.selectedElement=="")
          return;
        this.mousePosition=this.coordsScreenToMathScreen(screenX, screenY);
        this.logMouseStatus();
        if (this.selectedElement=="default")
        { this.rotateAfterCursorDefault();
        }
        if (this.selectedElement=="origin")
        { this.panAfterCursor();
        }
      },
      panAfterCursor: function()
      { var difference=vectorMinusVector(this.mousePosition, this.clickedPosition);
        this.centerX+=difference[0]*this.scale;
        this.centerY-=difference[1]*this.scale;
        this.redraw();
      },
      pointsWithinClickTolerance: function (leftXY, rightXY)
      { var squaredDistance= ((leftXY[0]-rightXY[0])*(leftXY[0]-rightXY[0]) +
              (leftXY[1]-rightXY[1])*(leftXY[1]-rightXY[1]))*this.scale;
        return squaredDistance<7;
      },
      canvasClick: function (screenX, screenY)
      { this.clickedPosition=this.coordsScreenToMathScreen(screenX, screenY);
        this.mousePosition=[];
        if (this.pointsWithinClickTolerance(this.clickedPosition,[0,0]))
          this.selectedElement="origin";
        else
        { this.selectedElement="default";
          this.computeSelectedVector();
        }
        this.logMouseStatus();
      },
      selectEmpty: function()
      { this.selectedElement="";
        this.selectedScreenBasis=[];
        this.selectedVector=[];
        this.selectedPolarAngleChange=0;
        this.angleNormal=0;
      },
      computeSelectedVector: function()
      { this.selectedScreenProjectionNormalized=this.coordsMathScreenToMath(this.clickedPosition);
        this.selectedVector=this.selectedScreenProjectionNormalized.slice();
        vectorNormalize(this.selectedScreenProjectionNormalized);
        this.selectedScreenNormal=this.screenNormal;
//        vectorAddVectorTimesScalar(this.selectedVector, this.screenNormal, 0.1);
        var lengthSelectedVector= vectorScalarVector(this.selectedVector, this.selectedVector);
        if (lengthSelectedVector<0.5)
          vectorTimesScalar(this.selectedVector, 1/Math.sqrt(lengthSelectedVector));
        this.selectedScreenBasis=[this.screenBasisOrthonormal[0].slice(), this.screenBasisOrthonormal[1].slice()];
      },
      showMessages: function()
      { if (this.spanMessages==null || this.spanMessages==undefined)
          return;
        var theHTML=
        "<span>" +this.textMouseInfo+ "</span><hr>"+
        "<span>" +this.textProjectionInfo+ "</span>";
        if (this.textErrors!="")
          theHTML+="<span>" +this.textProjectionInfo+ "</span>";
        this.spanMessages.innerHTML=theHTML;
        ;
      },
      logMouseStatus: function()
      { this.textMouseInfo=
        "selected element: " + this.selectedElement +
        "<br>mouse coordinates: " + this.mousePosition +
        "<br>clicked coordinates: " + this.clickedPosition +
        "<br>delta of position: " + this.positionDelta +
        "<br>ray component of mouse: " + this.rayComponent +
        "<br>selected vector: " + this.selectedVector +
        "<br>normal angle change: "+ this.angleNormal.toFixed(3)
        + " (" + (this.angleNormal*180/Math.PI).toFixed(1) + " deg)"
        + " = " +
        this.oldAngleNormal.toFixed(3)+ " (" + (this.oldAngleNormal*180/Math.PI).toFixed(1) + " deg)"
        + "-"+
        this.newAngleNormal.toFixed(3)+ " (" + (this.newAngleNormal*180/Math.PI).toFixed(1) + " deg)"

        ;
        this.showMessages();
      }
    };
    if (firstCanvas==null)
      firstCanvas=calculatorCanvases[inputCanvas.id];
  }
  return calculatorCanvases[inputCanvas.id];
}

function calculatorCanvasMouseMoveRedraw(inputCanvas, x, y)
{ var theCanvas=calculatorGetCanvas(inputCanvas);
  if (theCanvas.theIIIdObjects.theContours.length==0)
  { theCanvas.init();
    theCanvas.drawLine([-1,0,0],[1,0,0], 'black');
    theCanvas.drawLine([0,-1,0],[0,1,0], 'black');
    theCanvas.drawLine([0,0,-1],[0,0,1], 'black');
    theCanvas.drawLine([0,0,0] ,[1,0.5,0.5], 'red');
    theCanvas.drawLine([0,0,0] ,[-2,0,1], 'blue');
    theCanvas.drawPatch([1,0,0], [0,1,0], [0,0,1], 'cyan');
    theCanvas.drawPoint([1,0,0], 'red');
    theCanvas.drawPoint([0,1,0], 'green');
    theCanvas.drawPoint([0,0,1], 'blue');

    //console.log(theCanvas.theIIIdObjects.thePatches);
    theCanvas.redraw();
  }
  if (theCanvas!=null && theCanvas!=undefined)
    theCanvas.mouseMove(x,y);
}

function calculatorCanvasMouseWheel(theCanvasContainer, theEvent)
{ theEvent = theEvent ? theEvent : window.event;
  theEvent.preventDefault();
  theEvent.stopPropagation();
  var theWheelDelta = theEvent.detail ? theEvent.detail * -1 : theEvent.wheelDelta / 40;
  var theCanvas=calculatorCanvases[theCanvasContainer.id];
  if (theCanvas==undefined || theCanvas==null)
    return;
  theCanvas.scale+= theWheelDelta *3;
  if (theCanvas.scale<=0)
    theCanvas.scale=3;
  theCanvas.redraw();
}

function calculatorCanvasMouseUp(inputCanvas)
{ var theCanvas=calculatorGetCanvas(inputCanvas);
  theCanvas.selectEmpty();
}

function calculatorCanvasClick(theCanvasContainer, theEvent)
{ calculatorCanvases[theCanvasContainer.id].canvasClick(theEvent.clientX, theEvent.clientY);
}
