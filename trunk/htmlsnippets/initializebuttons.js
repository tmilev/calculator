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
"</tr><tr>"+
"<td>" + getDivideButton(i)+ "</td><td>"+getPowerButton(i) + "</td>"+ 
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
  return "<button onclick='sqrt_N_Click(answerMathQuillObjects[" + indexMathField + "]);'>nthroot</button>";
}

function sqrt_N_Click(currentMathField){ 
  currentMathField.write("\\sqrt[]{}");
}

function getSqrtButton(indexMathField){
  return "<button onclick='sqrtClick(answerMathQuillObjects[" + indexMathField + "]);'>sqrt</button>";
}

function sqrtClick(currentMathField){ 
  currentMathField.write("\\sqrt{}");
}

function getDivideButton(indexMathField){
  return "<button onclick='divideButtonClick(answerMathQuillObjects[" + indexMathField + "]);'>divide</button>";
}

function divideButtonClick(currentMathField){ 
  currentMathField.write("\\frac{}{}");
}

function getPowerButton(indexMathField){
  return "<button onclick='powerClick(answerMathQuillObjects[" + indexMathField + "]);'>power</button>";
}

function powerClick(currentMathField){ 
  currentMathField.write("{}^{}");
}



