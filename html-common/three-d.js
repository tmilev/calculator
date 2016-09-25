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
  for (i=0; i<s.length; i++)
    result+=s[i]*t[i];
  return result;
}

function vectorTimesScalar(vector, s)
{ for (i=0;i<vector.length; i++)
    vector[i]*=s;
}

function vectorAddVectorTimesScalar(output, otherVector, scalar)
{ for (i=0;i<output.length; i++)
    output[i]+=otherVector[i]*scalar;
}


function vectorLength(vector)
{ return Math.sqrt(vectorScalarVector(vector, vector));
}

function deepCopy(from, to)
{ to = new Array(from.length)
  for (i=0; i<from.length; i++)
    if (from[i] instanceof Array)
      deepCopy(from[i], to[i]);
    else
      to[i]=from[i];
}

function vectorNormalize(vector)
{ vectorTimesScalar(vector, 1/vectorLength(vector));
}

function calculatorGetCanvas(inputCanvas)
{ if (calculatorCanvases[inputCanvas.id]==undefined)
  { calculatorCanvases[inputCanvas.id]=
    { surface: inputCanvas.getContext("2d"),
      theIIIdObjects:
      { theLines: []
      },
      screenBasisUser: [[2,1,0],[0,1,1]],
      drawLine: function (leftPt, rightPt, inputColor)
      { theLines=this.theIIIdObjects.theLines;
        theLines[theLines.length]={left:leftPt, right: rightPt, color: inputColor};
      },
      redraw: function()
      { var theLs=this.theIIIdObjects.theLines;
        var leftPt;
        var rightPt;
        var theSurface=this.surface;
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
      computeBasis: function ()
      { this.screenBasisOrthographic[0]=theCanvas.screenBasisUser[0].slice();
        this.screenBasisOrthographic[1]=theCanvas.screenBasisUser[1].slice();
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
      centerX: inputCanvas.width/2,
      centerY: inputCanvas.height/2,
      scale: 50,
      screenBasisOrthographic: []
    };
  }
  return calculatorCanvases[inputCanvas.id];
}

function calculatorCanvasMouseMoveRedraw(inputCanvas, x, y)
{ theCanvas=calculatorGetCanvas(inputCanvas);
  theCanvas.computeBasis();
  if (theCanvas.theIIIdObjects.theLines.length==0)
  { theCanvas.drawLine([-1,0,0],[1,0,0], 'black');
    theCanvas.drawLine([0,-1,0],[0,1,0], 'black');
    theCanvas.drawLine([0,0,-1],[0,0,1], 'black');
    theCanvas.drawLine([0,0,0],[1,2,0], 'red');
    theCanvas.drawLine([0,0,0],[-2,0,1], 'blue');
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
