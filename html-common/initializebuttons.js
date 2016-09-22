var answerIdsPureLatex;
var answerMathQuillObjects=[];
var answerMQspanIds;
var preferredButtonContainers;
function initializeButtonsCommon(){
  ///initializing accordions
  acc = document.getElementsByClassName("accordion");
  for (i = 0; i < acc.length; i++) {
    acc[i].onclick = function(){
      this.classList.toggle("active");
      this.nextElementSibling.classList.toggle("show");
    }
  }
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
  currentMathField.write("\\sqrt[]{}");
}

function getSqrtButton(indexMathField){
  return "<button style='width:25' onclick='sqrtClick(answerMathQuillObjects[" + indexMathField + "]);'>&#8730;</button>";
}

function sqrtClick(currentMathField){ 
  currentMathField.cmd("\\sqrt");
}

function getDivideButton(indexMathField){
  return "<button style='width:25' onclick='divideButtonClick(answerMathQuillObjects[" + indexMathField + "]);'>/</button>";
}

function divideButtonClick(currentMathField){ 
  currentMathField.write("\\frac{}{}");
}

function getPowerButton(indexMathField){
  return "<button style='width:25' onclick='powerClick(answerMathQuillObjects[" + indexMathField + "]);'>^</button>";
}

function powerClick(currentMathField){ 
  currentMathField.write("{}^{}");
}

function getPlusButton(indexMathField){
  return "<button style='width:25' onclick='plusClick(answerMathQuillObjects[" + indexMathField + "]);'>+</button>";
}

function plusClick(currentMathField){ 
  currentMathField.write("+");
}

function getTimesButton(indexMathField){
  return "<button style='width:25' onclick='timesClick(answerMathQuillObjects[" + indexMathField + "]);'>*</button>";
}

function timesClick(currentMathField){ 
  currentMathField.write("\\cdot");
}

function getInftyButton(indexMathField){
  return "<button style='width:25' onclick='inftyClick(answerMathQuillObjects[" + indexMathField + "]);'>&#8734</button>";
}

function inftyClick(currentMathField){ 
  currentMathField.write("\\infty");
}

function getPiButton(indexMathField){
  return "<button style='width:25' onclick='piClick(answerMathQuillObjects[" + indexMathField + "]);'>&#960</button>";
}

function piClick(currentMathField){ 
  currentMathField.write("\\pi");
}

function getTwoByOneMatButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onclick='twoByOneMatClick(answerMathQuillObjects[" + indexMathField + "]);'>2x1</button>";
}

function twoByOneMatClick(currentMathField){ 
  currentMathField.write("\\begin{pmatrix}\\\\ \\end{pmatrix}");
}

function getTwoByTwoMatButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onclick='twoByTwoMatClick(answerMathQuillObjects[" + indexMathField + "]);'>2x2</button>";
}

function twoByTwoMatClick(currentMathField){ 
  currentMathField.write("\\begin{pmatrix} & \\\\ & \\end{pmatrix}");
}

function getThreeByThreeMatButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onclick='threeByThreeMatClick(answerMathQuillObjects[" + indexMathField + "]);'>3x3</button>";
}

function threeByThreeMatClick(currentMathField){ 
  currentMathField.write("\\begin{pmatrix} & & \\\\ & & \\\\ & & \\end{pmatrix}");
}

function getThreeByOneMatButton(indexMathField){
  return "<button style='padding:0; width:25; font-size : 9px; height:21.2;' onclick='threeByOneMatClick(answerMathQuillObjects[" + indexMathField + "]);'>3x1</button>";
}

function threeByOneMatClick(currentMathField){ 
  currentMathField.write("\\begin{pmatrix}\\\\ \\\\ \\end{pmatrix}");
}

function getMathBFibutton(indexMathField){
  return "<button style='width:25; font-weight: bold' onclick='mathBFiClick(answerMathQuillObjects[" + indexMathField + "]);'>i</button>";
}

function mathBFiClick(currentMathField){ 
 currentMathField.write('\\mathbf{i}');
}

function getMathBFjbutton(indexMathField){
  return "<button style='width:25; font-weight: bold' onclick='mathBFjClick(answerMathQuillObjects[" + indexMathField + "]);'>j</button>";
}

function mathBFjClick(currentMathField){ 
 currentMathField.write('\\mathbf{j}');
}

function getMathBFkbutton(indexMathField){
  return "<button style='width:25; font-weight: bold' onclick='mathBFkClick(answerMathQuillObjects[" + indexMathField + "]);'>k</button>";
}

function mathBFkClick(currentMathField){ 
 currentMathField.write('\\mathbf{k}');
}

