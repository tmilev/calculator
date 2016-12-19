"use strict";
var calculatorInput;
var calculatorMQfield;
var calculatorMQobject;
var mqProblemSpan;
var calculatorLeftString;
var calculatorRightString;
var calculatorMQString;
var calculatorMQStringIsOK=true;
var ignoreNextMathQuillUpdateEvent=false;
var calculatorLeftPosition=0;
var calculatorRightPosition=0;
function initializeCalculatorPage()
{ calculatorInput=document.getElementById('mainInputID');
  calculatorMQfield=document.getElementById('mainInputMQfield');
  mqProblemSpan=document.getElementById('mqProblemSpan');
  ////////////////////////
  globalMQ.config({autoFunctionize: 'sin cos tan sec csc cot log ln'});
  calculatorMQobject = globalMQ.MathField
  ( calculatorMQfield,
    { spaceBehavesLikeTab: true, // configurable
      supSubsRequireOperand: true, // configurable
      autoSubscriptNumerals: true, // configurable
      handlers:
      { edit: function()
        { // useful event handlers
          if (ignoreNextMathQuillUpdateEvent)
          { return;
          }
          if (!calculatorMQStringIsOK)
            return;
          var theBoxContent=calculatorMQobject.latex();
          calculatorInput.value =calculatorLeftString+
          processMathQuillLatex(theBoxContent)+
          calculatorRightString
          ;
        }
      }
    }
   );
}

function MQHelpCalculator()
{ //event.preventDefault();
  getSemiColumnEnclosure();
  ignoreNextMathQuillUpdateEvent=true;
  if (calculatorMQStringIsOK)
    calculatorMQobject.latex(calculatorMQString);
  ignoreNextMathQuillUpdateEvent=false;
}

function createSelection(field, start, end)
{ if( field.createTextRange )
  { var selRange = field.createTextRange();
    selRange.collapse(true);
    selRange.moveStart('character', start);
    selRange.moveEnd('character', end);
    selRange.select();
    field.focus();
  } else if( field.setSelectionRange )
  { field.focus();
    field.setSelectionRange(start, end);
  } else if( typeof field.selectionStart != 'undefined' )
  { field.selectionStart = start;
    field.selectionEnd = end;
    field.focus();
  }
}

var calculatorSeparatorLeftDelimiters= ['(', '{'];
var calculatorSeparatorRightDelimiters=[')', '}'];

function accountCalculatorDelimiterReturnMustEndSelection(text, calculatorSeparatorCounts, thePos)
{ var result=false;
  for (var j=0; j<calculatorSeparatorLeftDelimiters.length; j++)
  { if(calculatorSeparatorLeftDelimiters[j]===text[thePos])
      calculatorSeparatorCounts[j]++;
    if(calculatorSeparatorRightDelimiters[j]===text[thePos])
      calculatorSeparatorCounts[j]--;
    if (calculatorSeparatorLeftDelimiters[j]>0)
      result=true;
  }
  return result;
}

function chopCalculatorStrings()
{ if (calculatorRightPosition-calculatorLeftPosition>1000)
  { calculatorMQStringIsOK=false;
    mqProblemSpan.innerHTML="<span style='color:red'><b>Formula too big </b></span>";
    return;
  }
  calculatorMQStringIsOK=true;
  mqProblemSpan.innerHTML="Equation assistant";
  calculatorMQString= calculatorInput.value.
  substring(calculatorLeftPosition, calculatorRightPosition+1);
  calculatorLeftString=calculatorInput.value.
  substring(0, calculatorLeftPosition);
  calculatorRightString=calculatorInput.value.
  substring(calculatorRightPosition+1);
}

function getSemiColumnEnclosure()
{ if (calculatorInput.selectionEnd===undefined)
    return;
  var rightPos=calculatorInput.selectionEnd;
  for (; rightPos<calculatorInput.value.length; rightPos++)
    if (calculatorInput.value[rightPos]===';')
    { if (rightPos>0)
        rightPos--;
      break;
    }
  var calculatorSeparatorCounts=
  new Array(calculatorSeparatorLeftDelimiters.length).fill(0);
  var leftPos=rightPos;
  for (; leftPos>0; leftPos--)
  { if (calculatorInput.value[leftPos]===';')
    { leftPos++;
      break;
    }
    if (accountCalculatorDelimiterReturnMustEndSelection
        (calculatorInput.value, calculatorSeparatorCounts, leftPos)
       )
    { leftPos++;
      break;
    }
  }
  if (leftPos>rightPos)
    leftPos=rightPos;
  if (rightPos-leftPos>1000)
  { mqProblemSpan.innerHTML="<span style='color:red'><b></b></span>"
  }
  calculatorLeftPosition=leftPos;
  calculatorRightPosition=rightPos;
  chopCalculatorStrings();
}

/*function getCaretPosition (oField)
{ // Initialize
  var iCaretPos = 0;
  // IE Support
  if (document.selection)
  { // Set focus on the element
    oField.focus();
    // To get cursor position, get empty selection range
    var oSel = document.selection.createRange();
    // Move selection start to 0 position
    oSel.moveStart('character', -oField.value.length);
    // The caret position is selection length
    iCaretPos = oSel.text.length;
  }// Firefox support:
  else if (oField.selectionStart || oField.selectionStart == '0')
    iCaretPos = oField.selectionStart;
  // Return results
  return iCaretPos;
}*/
