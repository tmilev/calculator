var answerIdsPureLatex;
var answerMathQuillObjects=[];
var answerMQspanIds;
var preferredButtonContainers;
//var lastFocus;
var charsToSplit=['x','y'];

function processMathQuillLatex(theText)
{ for (var i=0; i<theText.length; i++)
    if (i+1<theText.length)
      if (theText[i]=='_' || theText[i]=='^' &&
          theText[i+1]!='{')
        theText=theText.slice(0,i+1)+'{'+theText[i+1]+'}'+theText.slice(i+2);
  if (charsToSplit!=undefined)
    for (var i=0; i<theText.length-1; i++)
    { for (var j=0; j<charsToSplit.length; j++)
        if (theText[i]==charsToSplit[j] &&
            theText[i+1]!=' ' && theText[i+1]!='\\'
            && theText[i+1]!='+' && theText[i+1]!='*'
            && theText[i+1]!='/' && theText[i+1]!='-'
            && theText[i+1]!='='
            )
          theText=theText.slice(0,i+1)+" "+theText.slice(i+1);
    }
  return theText;
}

function initializeButtonsCommon(){
  ///initializing accordions
  acc = document.getElementsByClassName("accordion");
  for (i = 0; i < acc.length; i++) {
    acc[i].onclick = function(){
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
function initializeButtons(){
  for (i=0; i<answerIdsPureLatex.length; i++){ 
    currentButtonPanel=document.getElementById(preferredButtonContainers[i]);	
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

function initializeButtonsMatrixSupport(){
  for (i=0; i<answerIdsPureLatex.length; i++){ 
    currentButtonPanel=document.getElementById(preferredButtonContainers[i]);	
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

function getSqrt_N_Button(indexMathField){
  return "<button style='width:25' onclick='sqrt_N_Click(answerMathQuillObjects[" + indexMathField + "]);'>&#8731;</button>";
}

function sqrt_N_Click(currentMathField){ 
  currentMathField.cmd("\\nthroot");
  currentMathField.focus();
}

function getSqrtButton(indexMathField){
  return "<button style='width:25' onclick='sqrtClick(answerMathQuillObjects[" + indexMathField + "]);'>&#8730;</button>";
}

function sqrtClick(currentMathField){
  currentMathField.cmd('\\sqrt');
  currentMathField.focus();
}

function getDivideButton(indexMathField){
  return "<button style='width:25' onclick='divideButtonClick(answerMathQuillObjects[" + indexMathField + "]);'>/</button>";
}

function divideButtonClick(currentMathField){ 
  currentMathField.cmd("/");
  currentMathField.focus();
}

function getPowerButton(indexMathField){
  return "<button style='width:25' onclick='powerClick(answerMathQuillObjects[" + indexMathField + "]);'>^</button>";
}

function powerClick(currentMathField){ 
  currentMathField.typedText("^");
  currentMathField.focus();
}

function getPlusButton(indexMathField){
  return "<button style='width:25' onclick='plusClick(answerMathQuillObjects[" + indexMathField + "]);'>+</button>";
}

function plusClick(currentMathField){ 
  currentMathField.cmd("+");
  currentMathField.focus();
}

function getTimesButton(indexMathField){
  return "<button style='width:25' onclick='timesClick(answerMathQuillObjects[" + indexMathField + "]);'>*</button>";
}

function timesClick(currentMathField){ 
  currentMathField.cmd("\\cdot");
  currentMathField.focus();
}

function getInftyButton(indexMathField){
  return "<button style='width:25' onclick='inftyClick(answerMathQuillObjects[" + indexMathField + "]);'>&#8734</button>";
}

function inftyClick(currentMathField){ 
  currentMathField.cmd("\\infty");
  currentMathField.focus();
}

function getUnderscoreButton(indexMathField){
  return "<button style='width:25' onclick='underscoreClick(answerMathQuillObjects[" + indexMathField + "]);'>_</button>";
}

function underscoreClick(currentMathField){ 
  currentMathField.cmd("_");
  currentMathField.focus();
}

function getLogBaseButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onclick='logBaseButtonClick(answerMathQuillObjects[" + indexMathField + "]);'>log</button>";
}

function logBaseButtonClick(currentMathField){ 
  currentMathField.cmd("log");
  currentMathField.cmd("_");
  currentMathField.focus();
}


function getPiButton(indexMathField){
  return "<button style='width:25' onclick='piClick(answerMathQuillObjects[" + indexMathField + "]);'>&#960</button>";
}

function piClick(currentMathField){ 
  currentMathField.write("\\pi");
  currentMathField.focus();
}

function getTwoByOneMatButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onclick='twoByOneMatClick(answerMathQuillObjects[" + indexMathField + "]);'>2x1</button>";
}

function twoByOneMatClick(currentMathField){ 
  currentMathField.write("\\begin{pmatrix}\\\\ \\end{pmatrix}");
  currentMathField.focus();
}

function getTwoByTwoMatButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onclick='twoByTwoMatClick(answerMathQuillObjects[" + indexMathField + "]);'>2x2</button>";
}

function twoByTwoMatClick(currentMathField){ 
  currentMathField.write("\\begin{pmatrix} & \\\\ & \\end{pmatrix}");
  currentMathField.focus();
}

function getThreeByThreeMatButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onclick='threeByThreeMatClick(answerMathQuillObjects[" + indexMathField + "]);'>3x3</button>";
}

function threeByThreeMatClick(currentMathField){ 
  currentMathField.write("\\begin{pmatrix} & & \\\\ & & \\\\ & & \\end{pmatrix}");
  currentMathField.focus();
}

function getThreeByOneMatButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onclick='threeByOneMatClick(answerMathQuillObjects[" + indexMathField + "]);'>3x1</button>";
}

function threeByOneMatClick(currentMathField){ 
  currentMathField.write("\\begin{pmatrix}\\\\ \\\\ \\end{pmatrix}");
  currentMathField.focus();
}

function getMathBFibutton(indexMathField){
  return "<button style='width:25; font-weight: bold' onclick='mathBFiClick(answerMathQuillObjects[" + indexMathField + "]);'>i</button>";
}

function mathBFiClick(currentMathField){ 
  currentMathField.write('\\mathbf{i}');
  currentMathField.focus();
}

function getMathBFjbutton(indexMathField){
  return "<button style='width:25; font-weight: bold' onclick='mathBFjClick(answerMathQuillObjects[" + indexMathField + "]);'>j</button>";
}

function mathBFjClick(currentMathField){ 
  currentMathField.write('\\mathbf{j}');
  currentMathField.focus();
}

function getMathBFkbutton(indexMathField){
  return "<button style='width:25; font-weight: bold' onclick='mathBFkClick(answerMathQuillObjects[" + indexMathField + "]);'>k</button>";
}

function mathBFkClick(currentMathField){ 
  currentMathField.write('\\mathbf{k}');
  currentMathField.focus();
}

