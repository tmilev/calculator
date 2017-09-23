"use strict";
var answerIdsPureLatex=[];
var answerMathQuillObjects=[];
var answerMQspanIds=[];
var preferredButtonContainers=[];
var studentScoresInHomePage=[];
//var lastFocus;
var charsToSplit=['x','y'];

function processMathQuillLatex(theText)
{ for (var i=0; i<theText.length; i++)
    if (i+1<theText.length)
    { if ((theText[i]==='_' || theText[i]==='^') && theText[i+1]!=='{')
        theText=theText.slice(0,i+1)+'{'+theText[i+1]+'}'+theText.slice(i+2);
      if (theText[i]==='\\'  && theText[i+1]==='\\')
        theText=theText.slice(0,i+2)+' '+theText.slice(i+2);
    }
  if (charsToSplit!=undefined)
    for (i=0; i<theText.length-1; i++)
    { for (var j=0; j<charsToSplit.length; j++)
        if (theText[i]===charsToSplit[j] &&
            theText[i+1]!==' ' && theText[i+1]!=='\\'
            && theText[i+1]!=='+' && theText[i+1]!=='*'
            && theText[i+1]!=='/' && theText[i+1]!=='-'
            && theText[i+1]!=='='
            )
        { if (theText[i]==='x')
            if (theText.slice(i-5,i+1)==='matrix')
              continue;
          theText=theText.slice(0,i+1)+" "+theText.slice(i+1);
        }
    }
  return theText;
}

function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

function initializeButtonsCommon()
{ ///initializing accordions
  var acc = document.getElementsByClassName("accordion");
  for (var i = 0; i < acc.length; i++)
  { acc[i].onclick = //async
    function()
    { if (this.firstLoad===undefined)
      { this.firstLoad=true;
        var theDeadlines=this.nextElementSibling.getElementsByClassName("modifyDeadlineInput");
        for (var j=0; j<theDeadlines.length; j++)
          $('#'+ theDeadlines[j].id).datepicker();
//        console.log("first run: "+theDeadlines);
        this.nextElementSibling.style.display="inline-block";
        //await sleep(400);
      }
      //this.nextElementSibling.style.transition="0.6s linear";
      this.classList.toggle("active");
      this.nextElementSibling.classList.toggle("show");
    }
  }
//  for (i=0; i<answerIdsPureLatex.length; i++){ 
//    currentButtonPanel=document.getElementById(answerMQspanIds[i]);	
//    document.addEventListener("blur", function(){
//      lastfocus=this;
//    });
//  }
}

function initializeOneButtonPanel(currentButtonPanel, panelIndex)
{ var buttonArray= currentButtonPanel.attributes.buttons.value.toLowerCase().split(/(?:,| )+/);
  //console.log(buttonArray);
  var buttonBindings = [];
  function addBinding(theFun)
  { if (buttonBindings.indexOf(theFun)>-1)
      return;
    buttonBindings.push(theFun);
  }
  var noOptions=false;
  var includeAll=false;
  if (buttonArray.indexOf("all")>-1)
    includeAll=true;
  if (buttonArray.length===0)
    noOptions=true;
  if (buttonArray.length===1)
    if (buttonArray[0]==="")
      noOptions=true;
  if (buttonArray.indexOf("algebra")>-1 || noOptions || includeAll)
  { addBinding(getPlusButton);
    addBinding(getMinusButton);
    addBinding(getTimesButton);
    addBinding(getDivideButton);

    addBinding(getSqrtButton);
    addBinding(getSqrt_N_Button);
    addBinding(getPowerButton);
    addBinding(getLParenButton);
    addBinding(getRParenButton);
  }
  if (buttonArray.indexOf("brackets")>-1 || includeAll)
  { addBinding(getLBracketButton);
    addBinding(getRBracketButton);
    addBinding(getCommaButton);
  }
  if (buttonArray.indexOf("complex")>-1 || buttonArray.indexOf("imaginary")>-1 || includeAll)
  { addBinding(getiButton);
  }
  if (buttonArray.indexOf("variables")>-1 || includeAll)
  { addBinding(getxButton);
    addBinding(getyButton);
    addBinding(getEqualsButton);
  }
  if (buttonArray.indexOf("logarithms")>-1 || noOptions || includeAll)
  { addBinding(getLogBaseButton);
    addBinding(getUnderscoreButton);
    addBinding(getLnButton);
    addBinding(geteButton);
  }
  if (noOptions || includeAll)
  { addBinding(getBinomButton);
    addBinding(getFactorialButton);
    addBinding(getSumButton);
    addBinding(getInftyButton);
    addBinding(getCircButton);
  }
  if (buttonArray.indexOf("interval")>-1 || buttonArray.indexOf("intervals")>-1 || noOptions || includeAll)
  { addBinding(getOrButton);
    addBinding(getUnionButton);
    addBinding(getBelongsToButton);
    addBinding(getEmptySetButton);
  }
  if (buttonArray.indexOf("angle")>-1 || buttonArray.indexOf("angles")>-1 || noOptions || includeAll)
  { addBinding(getPiButton);
    addBinding(getDegreesButton);
    addBinding(getAlphaButton);
    addBinding(getBetaButton);
    addBinding(getGammaButton);
    addBinding(getThetaButton);
  }

  var theContent="<table>";
  var numButtonsPerLine=4;
  for (var j=0; j<buttonBindings.length; j++)
  { if (j%numButtonsPerLine===0)
    { if (j!==0)
        theContent+="</tr>";
      theContent+="<tr>";
    }
    theContent+="<td>"+ buttonBindings[j](panelIndex)+"</td>";
  }
  if (buttonBindings.length>0)
    theContent+="</tr>";
  currentButtonPanel.innerHTML=theContent;
}

function initializeButtons()
{ for (var i=0; i<answerIdsPureLatex.length; i++)
  { var currentButtonPanel=document.getElementById(preferredButtonContainers[i]);
    initializeOneButtonPanel(currentButtonPanel, i);
  }
  initializeButtonsCommon();
}

function initializeButtonsMatrixSupport()
{ for (var i=0; i<answerIdsPureLatex.length; i++)
  { var currentButtonPanel=document.getElementById(preferredButtonContainers[i]);
    currentButtonPanel.innerHTML=
"<table><tr>"+
"<td>" + getSqrtButton(i)+ "</td><td>"+getSqrt_N_Button(i) + "</td>"+ "<td>"+getPowerButton(i) + "</td>"+ 
"</tr>"+
"<tr>"+
"<td>" + getDivideButton(i)+ "</td>"+ "<td>" + getPlusButton(i)+ "</td><td>"+getTimesButton(i) + "</td>"+ 
"</tr>"+
"<tr>"+
"<td>" + getInftyButton(i)+ "</td><td>" + getPiButton(i) + "</td>"+ "<td>" + getTwoByTwoMatButton(i)+ "</td>"+
"</tr>"+
"<tr>"+
"<td>" + getTwoByOneMatButton(i)+ "</td>"+ "<td>" + getThreeByOneMatButton(i) + "</td>"+ "<td>" + getThreeByThreeMatButton(i)+ "</td>"+
"</tr>"+
"<tr>"+
"<td>" +getMathBFibutton(i)+ "</td>"+"<td>" +getMathBFjbutton(i)+ "</td>"+"<td>" +getMathBFkbutton(i)+ "</td>"+
"</tr>"+
"</table>"
;
  }
  initializeButtonsCommon();
}

function initializeButtonsCalculator()
{ for (var i=0; i<answerIdsPureLatex.length; i++)
  { var currentButtonPanel=document.getElementById(preferredButtonContainers[i]);
    currentButtonPanel.innerHTML=
"<table><tr>"+
"<td>" + getSqrtButton(i)+ "</td><td>"+getSqrt_N_Button(i) + "</td>"+ "<td>"+getPowerButton(i) + "</td>"+
"</tr>"+
"<tr>"+
"<td>" + getDivideButton(i)+ "</td>"+ "<td>" + getPlusButton(i)+ "</td><td>"+getTimesButton(i) + "</td>"+
"</tr>"+
"<tr>"+
"<td>" + getInftyButton(i)+ "</td><td>" + getPiButton(i) + "</td>"+ "<td>" + getTwoByTwoMatButton(i)+ "</td>"+
"</tr>"+
"<tr>"+
"<td>" + getTwoByOneMatButton(i)+ "</td>"+ "<td>" + getThreeByOneMatButton(i) + "</td>"+ "<td>" + getThreeByThreeMatButton(i)+ "</td>"+
"</tr>"+
"</table>"
;
  }
  initializeButtonsCommon();
}

function getSqrt_N_Button(indexMathField){
  return "<button style='width:25' onmousedown='sqrt_N_Click(answerMathQuillObjects[" + indexMathField + "]);'>&#8731;</button>";
}

function sqrt_N_Click(currentMathField){ 
  currentMathField.cmd("\\nthroot");
  event.preventDefault();
}

function getSqrtButton(indexMathField){
  return "<button style='width:25' onmousedown='sqrtClick(answerMathQuillObjects[" + indexMathField + "]);'>&#8730;</button>";
}

function sqrtClick(currentMathField){
  currentMathField.cmd('\\sqrt');
  event.preventDefault();
}

function getUnionButton(indexMathField)
{ return "<button style='width:25' onmousedown='unionClick(answerMathQuillObjects[" + indexMathField + "]);'>&#8746;</button>";
}

function unionClick(currentMathField)
{ currentMathField.cmd('\\cup');
  event.preventDefault();
}

function getEmptySetButton(indexMathField)
{ return "<button style='width:25' onmousedown='emptySetClick(answerMathQuillObjects[" + indexMathField + "]);'>&#8709;</button>";
}

function emptySetClick(currentMathField)
{ currentMathField.cmd('\\emptyset');
  event.preventDefault();
}

function getBelongsToButton(indexMathField)
{ return "<button style='width:25' onmousedown='belongsToClick(answerMathQuillObjects[" + indexMathField + "]);'>&#8712;</button>";
}

function belongsToClick(currentMathField)
{ currentMathField.cmd('\\in');
  event.preventDefault();
}

function getAlphaButton(indexMathField)
{ return "<button style='width:25' onmousedown='alphaClick(answerMathQuillObjects[" + indexMathField + "]);'>&alpha;</button>";
}

function alphaClick(currentMathField)
{ currentMathField.cmd('\\alpha');
  event.preventDefault();
}

function getBetaButton(indexMathField)
{ return "<button style='width:25' onmousedown='betaClick(answerMathQuillObjects[" + indexMathField + "]);'>&beta;</button>";
}

function betaClick(currentMathField)
{ currentMathField.cmd('\\beta');
  event.preventDefault();
}

function getGammaButton(indexMathField)
{ return "<button style='width:25' onmousedown='gammaClick(answerMathQuillObjects[" + indexMathField + "]);'>&gamma;</button>";
}

function gammaClick(currentMathField)
{ currentMathField.cmd('\\gamma');
  event.preventDefault();
}

function getThetaButton(indexMathField)
{ return "<button style='width:25' onmousedown='thetaClick(answerMathQuillObjects[" + indexMathField + "]);'>&theta;</button>";
}

function thetaClick(currentMathField)
{ currentMathField.cmd('\\theta');
  event.preventDefault();
}

function getDivideButton(indexMathField){
  return "<button style='width:25' onmousedown='divideButtonClick(answerMathQuillObjects[" + indexMathField + "]);'>/</button>";
}

function divideButtonClick(currentMathField){ 
  currentMathField.cmd("/");
  event.preventDefault();
}

function getPowerButton(indexMathField){
  return "<button style='width:25' onmousedown='powerClick(answerMathQuillObjects[" + indexMathField + "]);'>^</button>";
}

function powerClick(currentMathField){ 
  currentMathField.cmd("^");
  currentMathField.focus();
  event.preventDefault();
}

function getPlusButton(indexMathField){
  return "<button style='width:25' onmousedown='plusClick(answerMathQuillObjects[" + indexMathField + "]);'>+</button>";
}

function plusClick(currentMathField){ 
  currentMathField.cmd("+");
  currentMathField.focus();
  event.preventDefault();
}

function getTimesButton(indexMathField){
  return "<button style='width:25' onmousedown='timesClick(answerMathQuillObjects[" + indexMathField + "]);'>*</button>";
}

function timesClick(currentMathField){ 
  currentMathField.cmd("\\cdot");
  event.preventDefault();
}

function getSumButton(indexMathField){
  return "<button style='width:25' onmousedown='sumClick(answerMathQuillObjects[" + indexMathField + "]);'>&#8721</button>";
}

function sumClick(currentMathField){
  currentMathField.cmd("\\sum");
  event.preventDefault();
}

function getInftyButton(indexMathField){
  return "<button style='width:25' onmousedown='inftyClick(answerMathQuillObjects[" + indexMathField + "]);'>&#8734</button>";
}

function inftyClick(currentMathField){ 
  currentMathField.cmd("\\infty");
  event.preventDefault();
}

function getMinusButton(indexMathField){
  return "<button style='width:25' onmousedown='minusClick(answerMathQuillObjects[" + indexMathField + "]);'>-</button>";
}

function minusClick(currentMathField){
  currentMathField.cmd("-");
  event.preventDefault();
}

function getUnderscoreButton(indexMathField){
  return "<button style='width:25' onmousedown='underscoreClick(answerMathQuillObjects[" + indexMathField + "]);'>_</button>";
}

function underscoreClick(currentMathField){ 
  currentMathField.cmd("_");
  event.preventDefault();
}

function getFactorialButton(indexMathField){
  return "<button style='padding:0; width:25;' onmousedown='factorialButtonClick(answerMathQuillObjects[" + indexMathField + "]);'>!</button>";
}

function factorialButtonClick(currentMathField){
  currentMathField.cmd("!");
  event.preventDefault();
}

function getBinomButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 7px; height:21.2;' onmousedown='binomButtonClick(answerMathQuillObjects[" + indexMathField + "]);'>binom</button>";
}

function binomButtonClick(currentMathField){
  currentMathField.cmd("\\binom");
  event.preventDefault();
}

function geteButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onmousedown='eButtonClick(answerMathQuillObjects[" + indexMathField + "]);'>e</button>";
}

function eButtonClick(currentMathField){
  currentMathField.cmd("e");
  event.preventDefault();
}

function getxButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onmousedown='xButtonClick(answerMathQuillObjects[" + indexMathField + "]);'>x</button>";
}

function xButtonClick(currentMathField){
  currentMathField.cmd("x");
  event.preventDefault();
}

function getLParenButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onmousedown='lParenButtonClick(answerMathQuillObjects[" + indexMathField + "]);'>(</button>";
}

function lParenButtonClick(currentMathField){
  currentMathField.cmd("(");
  event.preventDefault();
}

function getRParenButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onmousedown='rParenButtonClick(answerMathQuillObjects[" + indexMathField + "]);'>)</button>";
}

function rParenButtonClick(currentMathField){
  currentMathField.cmd(")");
  event.preventDefault();
}

function getLBracketButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onmousedown='lBracketButtonClick(answerMathQuillObjects[" + indexMathField + "]);'>[</button>";
}

function lBracketButtonClick(currentMathField){
  currentMathField.cmd("[");
  event.preventDefault();
}

function getRBracketButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onmousedown='rBracketButtonClick(answerMathQuillObjects[" + indexMathField + "]);'>]</button>";
}

function rBracketButtonClick(currentMathField){
  currentMathField.cmd("]");
  event.preventDefault();
}

function getCommaButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onmousedown='commaButtonClick(answerMathQuillObjects[" + indexMathField + "]);'>,</button>";
}

function commaButtonClick(currentMathField){
  currentMathField.cmd(",");
  event.preventDefault();
}

function getyButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onmousedown='yButtonClick(answerMathQuillObjects[" + indexMathField + "]);'>y</button>";
}

function yButtonClick(currentMathField){
  currentMathField.cmd("y");
  event.preventDefault();
}

function getEqualsButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onmousedown='equalsButtonClick(answerMathQuillObjects[" + indexMathField + "]);'>=</button>";
}

function equalsButtonClick(currentMathField){
  currentMathField.cmd("=");
  event.preventDefault();
}

function getLnButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onmousedown='lnButtonClick(answerMathQuillObjects[" + indexMathField + "]);'>ln</button>";
}

function lnButtonClick(currentMathField){
  currentMathField.cmd("ln");
  event.preventDefault();
}

function getLogBaseButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onmousedown='logBaseButtonClick(answerMathQuillObjects[" + indexMathField + "]);'>log</button>";
}

function logBaseButtonClick(currentMathField){ 
  currentMathField.cmd("log");
  currentMathField.cmd("_");
  event.preventDefault();
}

function getOrButton(indexMathField){
  return "<button style='width:25' onmousedown='orClick(answerMathQuillObjects[" + indexMathField + "]);'>or</button>";
}

function orClick(currentMathField){
  currentMathField.cmd(" or ");
  event.preventDefault();
}

function getiButton(indexMathField){
  return "<button style='width:25' onmousedown='iClick(answerMathQuillObjects[" + indexMathField + "]);'>i</button>";
}

function iClick(currentMathField){
  currentMathField.cmd("i");
  event.preventDefault();
}

function getPiButton(indexMathField){
  return "<button style='width:25' onmousedown='piClick(answerMathQuillObjects[" + indexMathField + "]);'>&#960;</button>";
}

function piClick(currentMathField){
  currentMathField.cmd("\\pi");
  event.preventDefault();
}

function getDegreesButton(indexMathField){
  return "<button style='width:25' onmousedown='degreesClick(answerMathQuillObjects[" + indexMathField + "]);'>&#176;</button>";
}

function degreesClick(currentMathField){
  currentMathField.cmd("^");
  currentMathField.cmd("\\circ");
  event.preventDefault();
}

function getCircButton(indexMathField){
  return "<button style='width:25' onmousedown='circClick(answerMathQuillObjects[" + indexMathField + "]);'>&#9675;</button>";
}

function circClick(currentMathField){
  currentMathField.cmd("\\circ");
  event.preventDefault();
}

function getTwoByOneMatButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onmousedown='twoByOneMatClick(answerMathQuillObjects[" + indexMathField + "]);'>2x1</button>";
}

function twoByOneMatClick(currentMathField){ 
  currentMathField.write("\\begin{pmatrix}\\\\ \\end{pmatrix}");
  event.preventDefault();
}

function getTwoByTwoMatButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onmousedown='twoByTwoMatClick(answerMathQuillObjects[" + indexMathField + "]);'>2x2</button>";
}

function twoByTwoMatClick(currentMathField){ 
  currentMathField.write("\\begin{pmatrix} & \\\\ & \\end{pmatrix}");
  event.preventDefault();
}

function getThreeByThreeMatButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onmousedown='threeByThreeMatClick(answerMathQuillObjects[" + indexMathField + "]);'>3x3</button>";
}

function threeByThreeMatClick(currentMathField){ 
  currentMathField.write("\\begin{pmatrix} & & \\\\ & & \\\\ & & \\end{pmatrix}");
  event.preventDefault();
}

function getThreeByOneMatButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onmousedown='threeByOneMatClick(answerMathQuillObjects[" + indexMathField + "]);'>3x1</button>";
}

function threeByOneMatClick(currentMathField){ 
  currentMathField.write("\\begin{pmatrix}\\\\ \\\\ \\end{pmatrix}");
  event.preventDefault();
}

function getMathBFibutton(indexMathField){
  return "<button style='width:25; font-weight: bold' onmousedown='mathBFiClick(answerMathQuillObjects[" + indexMathField + "]);'>i</button>";
}

function mathBFiClick(currentMathField){ 
  currentMathField.write('\\mathbf{i}');
  event.preventDefault();
}

function getMathBFjbutton(indexMathField){
  return "<button style='width:25; font-weight: bold' onmousedown='mathBFjClick(answerMathQuillObjects[" + indexMathField + "]);'>j</button>";
}

function mathBFjClick(currentMathField){ 
  currentMathField.write('\\mathbf{j}');
  event.preventDefault();
}

function getMathBFkbutton(indexMathField){
  return "<button style='width:25; font-weight: bold' onmousedown='mathBFkClick(answerMathQuillObjects[" + indexMathField + "]);'>k</button>";
}

function mathBFkClick(currentMathField){ 
  currentMathField.write('\\mathbf{k}');
  event.preventDefault();
}

