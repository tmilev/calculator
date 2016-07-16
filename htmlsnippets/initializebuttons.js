var answerIdsPureLatex;
var answerMathQuillObjects=[];
var answerMQspanIds;
var preferredButtonContainers;
function initializeButtons(){
for (i=0; i<answerIdsPureLatex.length; i++){ 
  newElt= document.createElement("span");
  newElt.innerHTML=
"<table><tr>"+
"<td>" + getSqrtButton(i)+ "</td><td>"+getSqrt_N_Button(i) + "</td>"+ 
"</tr>"+
"<tr>"+
"<td>" + getDivideButton(i)+ "</td><td>"+getPowerButton(i) + "</td>"+ 
"</tr>"+
"<tr>"+
"<td>" + getPlusButton(i)+ "</td><td>"+getTimesButton(i) + "</td>"+ 
"</tr>"+
"</table>"
;
  currentMJElt=document.getElementById(answerIdsPureLatex[i]);	
  currentButtonPanel=document.getElementById(preferredButtonContainers[i]);	
  currentButtonPanel.appendChild(newElt);
}
//alert(answerIdsPureLatex);
//alert(answerIdsMathQuill);

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
  currentMathField.write("\\sqrt{}");
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



