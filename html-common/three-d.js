"use strict";
var calculatorCanvases=new Object;
var firstCriticalRunTimeError="";
var firstCanvas=undefined;

function calculatorError(x)
{ if (firstCriticalRunTimeError!="")
    return;  
  firstCriticalRunTimeError=x;
  if (firstCriticalRunTimeError!="" && firstCanvas!=undefined)
    firstCanvas.logText(firstCriticalRunTimeError+" All further error messages are suppressed.");
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
{ var result= Math.atan2(newX,newY) - Math.atan2(oldX, oldY);
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
        theContours: []
      },
      surface: inputCanvas.getContext("2d"),
      canvasContainer: inputCanvas,
      canvasId: inputCanvas.id,
      screenBasisUser: [[2,1,0],[0,1,1]],
      screenNormal: [],
      screenBasisOrthonormal: [],
      zBufferColCount: 50,
      zBufferRowCount: 50,
      zBuffer: [],
      width: inputCanvas.width,
      height: inputCanvas.height,
      centerX: inputCanvas.width/2,
      centerY: inputCanvas.height/2,
      scale: 50,
      mousePosition: [],
      clickedPosition: [],
      spanMessages: undefined,
      spanCriticalErrors: undefined,
      selectedElement: "",
      selectedVector: [],
      drawPatch: function(base, edge1, edge2, color)
      { this.theIIIdObjects.thePatches.push(new Patch(base, edge1, edge2, color));
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
      },
      pointIsBehindPatch: function(thePoint, thePatch)
      { if (vectorScalarVector(vectorMinusVector(thePoint, thePatch.base), thePatch.normalScreen1) *
            vectorScalarVector(vectorMinusVector(thePatch.v2, thePatch.base), thePatch.normalScreen1)
            >=0)
          if (vectorScalarVector(vectorMinusVector(thePoint, thePatch.base), thePatch.normalScreen2) *
              vectorScalarVector(vectorMinusVector(thePatch.v1, thePatch.base), thePatch.normalScreen2)
              >=0)
          { if (vectorScalarVector(vectorMinusVector(thePoint, thePatch.base), thePatch.normal) *
                vectorScalarVector(this.screenNormal, thePatch.normal)>0.000001)
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
      paintOneContour: function(theContour)
      { if (theContour.thePoints.length<2)
          return;
        var theSurface=this.surface;
        var thePts=theContour.thePoints;
        theSurface.beginPath();
        theSurface.strokeStyle=theContour.color;
        theSurface.lineWidth=1;
        var currentPt=this.coordsMathToScreen(thePts[0]);
        theSurface.moveTo(currentPt[0], currentPt[1]);
        var oldIsInForeGround=this.pointIsInForeGround(theContour.thePoints[0],-1);
        var dashIsOn=false;
        for (var i=1; i<thePts.length; i++)
        { var newIsInForeground=this.pointIsInForeGround(theContour.thePoints[i],-1);
          if (!newIsInForeground && !oldIsInForeGround && !dashIsOn)
          { if (i>1)
            { theSurface.stroke();
              theSurface.beginPath();
              theSurface.moveTo(currentPt[0], currentPt[1]);
            }
            theSurface.setLineDash([4,4]);
            dashIsOn=true;
          } else if (dashIsOn && newIsInForeground)
          { theSurface.stroke();
            theSurface.beginPath();
            theSurface.moveTo(currentPt[0], currentPt[1]);
            dashIsOn=false;
          }
          currentPt=this.coordsMathToScreen(thePts[i]);
          theSurface.lineTo(currentPt[0], currentPt[1]);
          oldIsInForeGround=newIsInForeground;
        }
        theSurface.stroke();
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
      accountOnePatch: function(patchIndex)
      { var thePatch=this.theIIIdObjects.thePatches(patchIndex);
        var ptOne=thePatch.base;
        var ptTwo=thePatch.base;
      },
      computeBuffers: function()
      { var thePatches=this.theIIIdObjects.thePatches;
        for (var i=0; i<this.zBuffer.length; i++)
          for (var j=0; j<this.zBuffer.length; j++)
            this.zBuffer[i][j]=[];
        for (var i=0; i<thePatches.length; i++)
          this.accountOnePatch(i);
      },
      redraw: function()
      { var theContours=this.theIIIdObjects.theContours;
        var thePatches=this.theIIIdObjects.thePatches;
        var theSurface=this.surface;
        theSurface.clearRect(0, 0, this.width, this.height);
        //this.drawZbuffer();
        //this.computeBuffers();
        for (var i=0; i<thePatches.length; i++)
          this.computePatch(thePatches[i]);
        for (var i=0; i<thePatches.length; i++)
          this.paintOnePatch(thePatches[i]);
        for (var i=0; i<theContours.length; i++)
          this.paintOneContour(theContours[i]);
      },
      allocateZbuffer: function()
      { if (this.zBufferRowCount<1)
          this.zBufferRowCount=1;
        if (this.zBufferColCount<1)
          this.zBufferColCount=1;
        if (this.zBuffer.length<this.zBufferRowCount)
        { this.zBuffer=new Array(this.zBufferRowCount);
          for (var i=0; i< this.zBufferRowCount; i++)
          { this.zBuffer[i]= new Array(this.zBufferColCount);
            for (var j=0; j< this.zBufferColCount; j++)
              this.zBuffer[i][j]=[];
          }
        }
      },
      drawZbuffer: function()
      { this.allocateZbuffer();
        var theSurface=this.surface;
        theSurface.strokeStyle="gray";
        for (var i=0; i<this.zBuffer.length+1; i++)
        { theSurface.beginPath();
          theSurface.moveTo(0,         i*this.height/this.zBuffer.length);
          theSurface.lineTo(this.width,i*this.height/this.zBuffer.length);
          theSurface.stroke();
        }
        for (var i=0; i<this.zBuffer[0].length+1; i++)
        { theSurface.beginPath();
          theSurface.moveTo(i*this.width/this.zBuffer[0].length, 0);
          theSurface.lineTo(i*this.width/this.zBuffer[0].length, this.width);
          theSurface.stroke();
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
      coordsMathToMathScreen: function(vector)
      { return [vectorScalarVector(vector, this.screenBasisOrthonormal[0]),
                vectorScalarVector(vector, this.screenBasisOrthonormal[1])];
      },
      coordsMathToScreen: function(vector)
      { return [this.scale*vectorScalarVector(vector, this.screenBasisOrthonormal[0])+this.centerX,
           (-1)*this.scale*vectorScalarVector(vector, this.screenBasisOrthonormal[1])+this.centerY];
      },
      getPosXPosY: function (cx, cy)
      { return getPosXPosYObject(this, cx, cy);
      },
      coordsScreenToMathScreen: function(screenX, screenY)
      { var xyScreen= getPosXPosYObject(this.canvasContainer, screenX,screenY);
        return [ (xyScreen[0]-this.centerX)/this.scale, (this.centerY-xyScreen[1])/this.scale];
      },
      coordsMathToScreenXY: function(x, y)
      {
      },
      rotateOutOfPlane: function (input, orthoBasis1, orthoBasis2, oldCosSquared, newCosSquared)
      { var projection= orthoBasis1.slice();
        var vComponent= input.slice();
        var scal1= vectorScalarVector(orthoBasis1, input);
        var scal2= vectorScalarVector(orthoBasis2, input);
        vectorTimesScalar(projection, scal1);
        vectorAddVectorTimesScalar(projection, orthoBasis2, scal2);
        vectorAddVectorTimesScalar(vComponent, projection, -1);
        var oldAngle=Math.acos(Math.sqrt(oldCosSquared));
        var newAngle=Math.acos(Math.sqrt(newCosSquared));
        if (isNaN(oldAngle) || isNaN(newAngle))
          return input;
        var angleChange=-oldAngle+newAngle;
        vectorAddVectorTimesScalar(projection, Math.cos(angleChange)*scal1-Math.sin(angleChange)*scal2);
        vectorAddVectorTimesScalar(projection, orthoBasis2, Math.sin(angleChange)*scal1+Math.sin(angleChange)*scal2);
        return vectorPlusVector(vComponent, projection);
      },
      rotateAfterCursorDefault: function()
      { if (this.mousePosition.length==0)
          return;
        var oldX=this.clickedPosition[0];
        var oldY=this.clickedPosition[1];
        var newX=this.mousePosition[0];
        var newY=this.mousePosition[1];
        var screenAngleChange=-getAngleChangeMathScreen(newX, newY, oldX, oldY);
        var e1=this.screenBasisOrthonormal[0];
        var e2=this.screenBasisOrthonormal[1];
        var NewE1 = e1.slice();
        var NewE2 = e2.slice();
        vectorTimesScalar(NewE1, Math.cos(screenAngleChange));
        vectorAddVectorTimesScalar(NewE1, e2, Math.sin(screenAngleChange));
        vectorTimesScalar(NewE2, Math.cos(screenAngleChange));
        vectorAddVectorTimesScalar(NewE2, e1, -Math.sin(screenAngleChange));
        e1= NewE1;
        e2= NewE2;
        var vectorScalarE1=vectorScalarVector(this.selectedVector, e1);
        var vectorScalarE2=vectorScalarVector(this.selectedVector, e2);
        var vProjection= this.coordsMathToMathScreen(this.selectedVector);
        var vOrthogonal=this.selectedVector.slice();
        vectorAddVectorTimesScalar(vOrthogonal, vProjection, -1);
        var selectedVectorSquaredLength=vectorScalarVector(this.selectedVector, this.selectedVector);
        var oldCosineSquared = (selectedVectorSquaredLength-oldX*oldX-oldY*oldY)/ selectedVectorSquaredLength;
        var newCosineSquared = (selectedVectorSquaredLength-newX*newX-newY*newY)/selectedVectorSquaredLength;
        vectorNormalize(vProjection);
        vectorNormalize(vOrthogonal);
        e1=this.rotateOutOfPlane(e1, vProjection, vOrthogonal, oldCosineSquared, newCosineSquared);
        e2=this.rotateOutOfPlane(e2, vProjection, vOrthogonal, oldCosineSquared, newCosineSquared);
        this.screenBasisUser[0]=e1.slice();
        this.screenBasisUser[1]=e2.slice();
        this.computeBasis();
        this.redraw();
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
      computeSelectedVector: function()
      { this.selectedVector=this.twoDCoordinatesToActual(this.clickedPosition);
        vectorAddVectorTimesScalar(this.selectedVector, this.screenNormal, 0.01);
        var lengthSelectedVector= vectorScalarVector(this.selectedVector, this.selectedVector);
        if (lengthSelectedVector<0.5)
          vectorTimesScalar(this.selectedVector, 1/Math.sqrt(lengthSelectedVector));
      },
      twoDCoordinatesToActual: function(theCoords)
      { var output=this.screenBasisOrthonormal[0].slice();
        vectorTimesScalar(output, theCoords[0]);
        vectorAddVectorTimesScalar(output, this.screenBasisOrthonormal[1], theCoords[1]);
        return output;
      },
      logText: function(theText)
      { if (this.spanCriticalErrors==null || this.spanCriticalErrors==undefined)
          return;
        this.spanCriticalErrors.innerText=theText;
      },
      logMouseStatus: function()
      { if (this.spanMessages==null || this.spanMessages==undefined)
          return;
        this.spanMessages.innerText=
        "selected element: " + this.selectedElement +
        "\nmouse coordinates: " + this.mousePosition +
        "\nclicked coordinates: " + this.clickedPosition +
//        "\nextracted from event coords: " + screenX + ", " + screenY+
        "\nselected vector: " + this.selectedVector
        ;
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
    theCanvas.drawLine([0,0,0] ,[1,2,0], 'red');
    theCanvas.drawLine([0,0,0] ,[-2,0,1], 'blue');
    theCanvas.drawPatch([0,0,0], [1,1,0], [1,1,1], 'cyan');
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
  theCanvas.selectedElement="";
}

function calculatorCanvasClick(theCanvasContainer, theEvent)
{ calculatorCanvases[theCanvasContainer.id].canvasClick(theEvent.clientX, theEvent.clientY);
}
