var answerIdsPureLatex;
var answerIdsMathQuill;
function initializeButtons(){
for (i=0; i<answerIdsPureLatex.length; i++){ 
  newElt= document.createElement("span");
  newElt.innerHTML="<button onclick='sqrtClick(answerIdsMathQuill);'>sqrt</button>";
  currentElt=document.getElementById(answerIdsPureLatex[i]);	
  currentElt.parentNode.insertBefore(newElt, currentElt);
}
//alert(answerIdsPureLatex);
//alert(answerIdsMathQuill);

}

function sqrtClick(mathQuillObject){ 
  mathQuill
}
