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
      if ((theText[i]==='_' || theText[i]==='^') && theText[i+1]!=='{')
        theText=theText.slice(0,i+1)+'{'+theText[i+1]+'}'+theText.slice(i+2);
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

function initializeButtonsCommon()
{ ///initializing accordions
  var acc = document.getElementsByClassName("accordion");
  for (var i = 0; i < acc.length; i++)
  { acc[i].onclick = function()
    { if (this.firstLoad===undefined)
      { this.firstLoad=true;
        var theDeadlines=this.nextElementSibling.getElementsByClassName("modifyDeadlineInput");
        for (var j=0; j<theDeadlines.length; j++)
          $('#'+ theDeadlines[j].id).datepicker();
//        console.log("first run: "+theDeadlines);
      }
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

function initializeButtons()
{ for (var i=0; i<answerIdsPureLatex.length; i++)
  { var currentButtonPanel=document.getElementById(preferredButtonContainers[i]);
    currentButtonPanel.innerHTML=
"<table><tr>"+
"<td>" + getSqrtButton(i)+ "</td><td>"+getSqrt_N_Button(i) + "</td>"+ 
"</tr>"+
"<tr>"+
"<td>" + getDivideButton(i)+ "</td><td>"+getPowerButton(i) + "</td>"+ 
"</tr>"+
"<tr>"+
"<td>" + getPlusButton(i)+ "</td><td>"+getTimesButton(i) + "</td>"+ 
"</tr>"+
"<tr>"+
"<td>" + getInftyButton(i)+ "</td><td>" + getPiButton(i) + "</td>"+ 
"</tr>"+
"<tr>"+
"<td>" + getUnderscoreButton(i)+ "</td><td>" + getLogBaseButton(i) + "</td>"+ 
"</tr>"+
"</table>"
;
  }
  initializeButtonsCommon();
}

function initializeButtonsMatrixSupport()
{ for (i=0; i<answerIdsPureLatex.length; i++)
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

function updateStudentScores(idElement)
{ onLoadDefaultFunction(idElement);
  for (var i=0; i<studentScoresInHomePage.length; i++)
  { var currentElt=document.getElementById(studentScoresInHomePage[i].theId);
    if (currentElt===null || currentElt===undefined)
      continue;
    currentElt.innerHTML=
    "<span style=\"color:green\">" +
    studentScoresInHomePage[i].numSolvedAll+
    " solved all; </span> <span style=\"color:orange\">" +
    studentScoresInHomePage[i].numSolvedPart+
    " solved some; </span>"+
    "<span style=\"color:Red\"> " +studentScoresInHomePage[i].numSolvedNone+
    " solved none.</span>";
    if (!studentScoresInHomePage[i].weightsOK)
      currentElt.innerHTML+= " Not all problems have weights. ";
  }
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
  currentMathField.typedText("^");
  currentMathField.focus();
}

function getPlusButton(indexMathField){
  return "<button style='width:25' onmousedown='plusClick(answerMathQuillObjects[" + indexMathField + "]);'>+</button>";
}

function plusClick(currentMathField){ 
  currentMathField.cmd("+");
  event.preventDefault();
}

function getTimesButton(indexMathField){
  return "<button style='width:25' onmousedown='timesClick(answerMathQuillObjects[" + indexMathField + "]);'>*</button>";
}

function timesClick(currentMathField){ 
  currentMathField.cmd("\\cdot");
  event.preventDefault();
}

function getInftyButton(indexMathField){
  return "<button style='width:25' onmousedown='inftyClick(answerMathQuillObjects[" + indexMathField + "]);'>&#8734</button>";
}

function inftyClick(currentMathField){ 
  currentMathField.cmd("\\infty");
  event.preventDefault();
}

function getUnderscoreButton(indexMathField){
  return "<button style='width:25' onmousedown='underscoreClick(answerMathQuillObjects[" + indexMathField + "]);'>_</button>";
}

function underscoreClick(currentMathField){ 
  currentMathField.cmd("_");
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

function getPiButton(indexMathField){
  return "<button style='width:25' onmousedown='piClick(answerMathQuillObjects[" + indexMathField + "]);'>&#960</button>";
}

function piClick(currentMathField){ 
  currentMathField.cmd("\\pi");
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

