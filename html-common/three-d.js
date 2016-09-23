var calculatorCanvases=Array();
function calculatorGetCanvas(inputCanvas)
{ if (calculatorCanvases[inputCanvas.id]==undefined)
    calculatorCanvases[inputCanvas.id]=inputCanvas.getContext("2d");
  return calculatorCanvases[inputCanvas.id];
}
function calculatorCanvasMouseMoveRedraw(inputCanvas, x, y)
{ theCanvas=calculatorGetCanvas(inputCanvas);
  theCanvas.beginPath();
  theCanvas.strokeStyle="#000000"; 
  theCanvas.lineWidth=1; 
  theCanvas.moveTo(0,0); 
  theCanvas.lineTo(100,100); 
  theCanvas.stroke();
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
