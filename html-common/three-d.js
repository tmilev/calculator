"use strict";
var calculatorCanvases=new Object;
var firstCriticalRunTimeError="";

function calculatorError(x)
{ if (firstCriticalRunTimeError!="")
    return;  
  firstCriticalRunTimeError=x;
  if (firstCriticalRunTimeError!="")
    alert(firstCriticalRunTimeError+" All further error messages are suppressed.");
}

function vectorScalarVector(s,t)
{ var result=0;
  if (s.length!=t.length)
    calculatorError("Scalar product of vectors of different length: " + s + " and " + t);
  for (var i=0; i<s.length; i++)
    result+=s[i]*t[i];
  return result;
}

function vectorTimesScalar(vector, s)
{ for (var i=0;i<vector.length; i++)
    vector[i]*=s;
}

function vectorPlusVector(left,right)
{ var output=new Array(left.length);
  for (var i=0;i<left.length; i++)
    output[i]=left[i]+right[i];
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
}

function calculatorGetCanvas(inputCanvas)
{ if (calculatorCanvases[inputCanvas.id]==undefined)
  { calculatorCanvases[inputCanvas.id]=
    { surface: inputCanvas.getContext("2d"),
      theIIIdObjects:
      { theLines: [],
        thePatches: [],
        theContours: []
      },
      screenBasisUser: [[2,1,0],[0,1,1]],
      drawPatch: function(base, edge1, edge2, color)
      { this.theIIIdObjects.thePatches.push(new Patch(base, edge1, edge2, color));
      },
      drawLine: function (leftPt, rightPt, inputColor)
      { var theLines=this.theIIIdObjects.theLines;
        theLines[theLines.length]={left:leftPt, right: rightPt, color: inputColor};
      },
      drawOnePatch: function(thePatch){
        var theSurface=this.surface;
        theSurface.fillStyle=thePatch.color;
        theSurface.beginPath();
        var
        theCoords=this.getScreenCoordinates(thePatch.base);
        theSurface.moveTo(theCoords[0], theCoords[1]);
        theCoords=this.getScreenCoordinates(vectorPlusVector(thePatch.base, thePatch.edge1));
        theSurface.lineTo(theCoords[0], theCoords[1]);
        theCoords=this.getScreenCoordinates(vectorPlusVector(vectorPlusVector(thePatch.base, thePatch.edge1), thePatch.edge2));
        theSurface.lineTo(theCoords[0], theCoords[1]);
        theCoords=this.getScreenCoordinates(vectorPlusVector(thePatch.base, thePatch.edge2));
        theSurface.lineTo(theCoords[0], theCoords[1]);
        theCoords=this.getScreenCoordinates(thePatch.base);
        theSurface.lineTo(theCoords[0], theCoords[1]);
        theSurface.fill();
//        theSurface.stroke();
      },
      redraw: function()
      { var theLs=this.theIIIdObjects.theLines;
        var thePatches=this.theIIIdObjects.thePatches;
        var leftPt;
        var rightPt;
        var theSurface=this.surface;
        for (var i=0; i<thePatches.length; i++)
        { this.drawOnePatch(thePatches[i]);
        }
        for (var i=0; i<theLs.length; i++)
        { theSurface.beginPath();
          theSurface.strokeStyle=theLs[i].color;
          theSurface.lineWidth=1;
          leftPt=this.getScreenCoordinates(theLs[i].left);
          rightPt=this.getScreenCoordinates(theLs[i].right);
          theSurface.moveTo(leftPt[0], leftPt[1]);
          theSurface.lineTo(rightPt[0], rightPt[1]);
          theSurface.stroke();
        }
      },
      allocateZbuffer: function()
      { if (this.zBufferRowCount<1)
          this.zBufferRowCount=1;
        if (this.zBufferColCount<1)
          this.zBufferColCount=1;
        if (this.zBuffer.length<this.zBufferRowCount)
          this.zBuffer=new Array(this.zBufferRowCount);
        if (this.zBuffer[0].length!=this.zBufferColCount)
          for (var i=0; i< this.zBufferRowCount; i++)
            this.zBuffer[i]= new Array(this.zBufferColCount)
      },
      drawZbuffer: function()
      { this.allocateZbuffer();
        var theSurface=this.surface;
        theSufrace.strokeStyle="gray";
        for (var i=0; i<this.zBuffer.length+1; i++)
        { theSurface.beginPath();
          theSurface.moveTo(0,         i*height/this.zBuffer.length);
          theSurface.lineTo(this.width,i*height/this.zBuffer.length);
        }
        for (var i=0; i<this.zBuffer[0].length+1; i++)
        { theSurface.beginPath();

          theSurface.moveTo(i*height/this.zBuffer[0].length, 0);
          theSurface.lineTo(i*height/this.zBuffer[0].length, this.width);
        }
      },
      screenBasisOrthographic: [],
      computeBasis: function ()
      { //if (this.screenBasisOrthographic.length<2)
        //  this.screenBasisOrthographic.length=2;
        this.screenBasisOrthographic[0]=this.screenBasisUser[0].slice();
        this.screenBasisOrthographic[1]=this.screenBasisUser[1].slice();
        var e1=this.screenBasisOrthographic[0];
        var e2=this.screenBasisOrthographic[1];
        vectorNormalize(e1);
        vectorAddVectorTimesScalar(e2, e1, - vectorScalarVector(e1,e2));
        vectorNormalize(e2);
      },
      getScreenCoordinates: function(vector)
      { return [this.scale*vectorScalarVector(vector, this.screenBasisOrthographic[0])+this.centerX,
           (-1)*this.scale*vectorScalarVector(vector, this.screenBasisOrthographic[1])+this.centerY];
      },
      zBufferColCount: 50,
      zBufferRowCount: 50,
      zBuffer: [],
      width: inputCanvas.width,
      height: inputCanvas.height,
      centerX: inputCanvas.width/2,
      centerY: inputCanvas.height/2,
      scale: 50
    };
  }
  return calculatorCanvases[inputCanvas.id];
}

function calculatorCanvasMouseMoveRedraw(inputCanvas, x, y)
{ var theCanvas=calculatorGetCanvas(inputCanvas);
  theCanvas.computeBasis();
  if (theCanvas.theIIIdObjects.theLines.length==0)
  { theCanvas.drawLine([-1,0,0],[1,0,0], 'black');
    theCanvas.drawLine([0,-1,0],[0,1,0], 'black');
    theCanvas.drawLine([0,0,-1],[0,0,1], 'black');
    theCanvas.drawLine([0,0,0] ,[1,2,0], 'red');
    theCanvas.drawLine([0,0,0] ,[-2,0,1], 'blue');
    theCanvas.drawPatch([0,0,0], [1,1,0], [1,1,1], 'cyan');
    //console.log(theCanvas.theIIIdObjects.thePatches);
   }
   theCanvas.redraw();
}

function calculatorCanvasMouseWheel(inputCanvas, event)
{
}

function calculatorCanvasClick(inputCanvas, x,y)
{
}

function calculatorCanvasMouseUp(inputCanvas)
{
}
