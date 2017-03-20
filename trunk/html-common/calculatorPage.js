"use strict";
var calculatorInput;
var calculatorMQfield;
var calculatorMQobject;
var mqProblemSpan;
var calculatorLeftString=undefined;
var calculatorRightString=undefined;
var calculatorMQString;
var calculatorMQStringIsOK=true;
var ignoreNextMathQuillUpdateEvent=false;
var calculatorLeftPosition=0;
var calculatorRightPosition=0;
var calculatorPlotUpdaters= new Object;
var calculatorInputBoxToSliderUpdaters=new Object;
var calculatorInputBoxNames=[];
var numInsertedJavascriptChildren=0;
var calculatorCanvases;
var keyWordsKnownToMathQuill=['sqrt', 'frac', 'cdot'];
function initializeCalculatorVariables()
{ answerIdsPureLatex=['mainInputID'];
  answerMQspanIds=['mainInputMQfield'];
  mqProblemSpan=document.getElementById('mqProblemSpan');
  calculatorInput=document.getElementById('mainInputID');
  calculatorMQfield=document.getElementById('mainInputMQfield');
}

var calculatorMQobjectsInitialized=false;

function createSelectionNoFocus(field, start, end)
{ if( field.createTextRange )
  { var selRange = field.createTextRange();
    selRange.collapse(true);
    selRange.moveStart('character', start);
    selRange.moveEnd('character', end);
    selRange.select();
    field.focus();
    calculatorMQfield.focus();
  } else if( field.setSelectionRange )
  { field.focus();
    field.setSelectionRange(start, end);
    calculatorMQfield.focus();
  } else if( typeof field.selectionStart != 'undefined' )
  { field.selectionStart = start;
    field.selectionEnd = end;
    field.focus();
    calculatorMQfield.focus();
  }
}

function initializeCalculatorPage()
{ if (calculatorMQobjectsInitialized===true)
    return;
  calculatorMQobjectsInitialized=true;
  initializeCalculatorVariables();
  ////////////////////////
  globalMQ.config({autoFunctionize: 'sin cos tan sec csc cot log ln'});
  calculatorMQobject =
  globalMQ.MathField( calculatorMQfield,
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
          if (calculatorLeftString===undefined ||
              calculatorRightString===undefined)
            mQHelpCalculator();
          var theInserted=processMathQuillLatex(theBoxContent);
          calculatorInput.value =calculatorLeftString+
          theInserted+
          calculatorRightString;
//          calculatorInput.innerHTML =calculatorLeftString+
//          +"<span style='color:red'>" +theInserted+ "</span>"+
//          calculatorRightString;

//          createSelectionNoFocus(calculatorInput, calculatorLeftString.length, calculatorLeftString.length+theInserted.length);
        }
      }
    }
   );
  answerMathQuillObjects=[calculatorMQobject];
  preferredButtonContainers=['mainInputMQfieldButtons'];
  initializeButtonsCalculator();
  mQHelpCalculator();
  //I am getting mq errors if calculatorMQobject.write()
  //is called before mQHelpCalculator() is called.
  //I don't quite know what causes this behavior, so
  //for the time being please leave the code above as is.
  //I will resolve the issue later.
}

function onLoadDefaultFunction(idElement)
{ var spanVerification=document.getElementById(idElement);
  var scripts = spanVerification.getElementsByTagName('script');
  for (var i=0; i<numInsertedJavascriptChildren; i++)
  { document.getElementsByTagName('head')[0].removeChild(document.getElementsByTagName('head')[0].lastChild);
//    document.getElementsByTagName('head')[0].appendChild(scriptChild);
  }
  numInsertedJavascriptChildren=0;
  calculatorInputBoxNames=[];
  calculatorInputBoxToSliderUpdaters= new Object;
  calculatorCanvases=new Object;
  for (i=0; i<scripts.length; i++)
  { var scriptChild= document.createElement('script');
    scriptChild.innerHTML=scripts[i].innerHTML;
    scriptChild.type='text/javascript';
    document.getElementsByTagName('head')[0].appendChild(scriptChild);
    numInsertedJavascriptChildren++;
  }
  MathJax.Hub.Queue(['Typeset', MathJax.Hub, document.getElementById(idElement)]);
  MathJax.Hub.Queue([calculatorAddListenersToInputBoxes]);

//  alert(theString);
}

function calculatorAddListenersToInputBoxes()
{ //var theString=" updating: box names, slider names: ";
  for (var i=0; i<calculatorInputBoxNames.length; i++)
  { var theBoxes=document.getElementsByName(calculatorInputBoxNames[i]);
    for (var j=0; j<theBoxes.length; j++)
      theBoxes[j].addEventListener("input",
      function()
      { var sliderName=calculatorInputBoxToSliderUpdaters[this.name];
        document.getElementById(sliderName).value=this.value;
        event.preventDefault();
        updateCalculatorSliderToInputBox(this.name, sliderName);
      }
      );
  }
}

function updateCalculatorSliderToInputBox(boxName, sliderName)
{ var theBoxes= document.getElementsByName(boxName);
  var theSlider=document.getElementById(sliderName);
  for (var i=0; i<theBoxes.length; i++)
    theBoxes[i].value=theSlider.value;
  if (calculatorPlotUpdaters[sliderName]!==undefined)
  { var theCanvas=calculatorCanvases[calculatorPlotUpdaters[sliderName]];
    if (theCanvas!==undefined)
    { if (theCanvas.canvasResetFunction!==null)
        theCanvas.canvasResetFunction();
    }
  }
}

function mQHelpCalculator()
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
{ if (mqProblemSpan===undefined)
    initializeCalculatorPage();
  if (calculatorRightPosition-calculatorLeftPosition>1000)
  { calculatorMQStringIsOK=false;
    mqProblemSpan.innerHTML="<span style='color:red'><b>Formula too big </b></span>";
    return;
  }
  calculatorMQStringIsOK=true;
  mqProblemSpan.innerHTML="Equation assistant";
  calculatorMQfield.style.visibility="visible";
  calculatorMQString= calculatorInput.value.
  substring(calculatorLeftPosition, calculatorRightPosition+1);
  calculatorLeftString=calculatorInput.value.
  substring(0, calculatorLeftPosition);
  calculatorRightString=calculatorInput.value.
  substring(calculatorRightPosition+1);
}

function isSeparatorCharacter(theChar)
{ if(theChar[0]>='a' && theChar[0]<='z')
    return false;
  if (theChar[0]>='A' && theChar[0]<='Z')
    return false;
  return true;
}

function isKeyWordStartKnownToMathQuill(input)
{ for (var i=0; i<keyWordsKnownToMathQuill.length; i++)
    if (keyWordsKnownToMathQuill[i].startsWith(input))
      return true;
  return false;
}

function isKeyWordEndKnownToMathQuill(input)
{ for (var i=0; i<keyWordsKnownToMathQuill.length; i++)
    if (keyWordsKnownToMathQuill[i].endsWith(input))
      return true;
  return false;
}

function getSemiColumnEnclosure()
{ if (calculatorInput===undefined)
    initializeCalculatorPage();
  if (calculatorInput.selectionEnd===undefined)
    calculatorInput.selectionEnd=0;
  var startPos =calculatorInput.selectionEnd;
  for (; startPos>0; startPos--)
    if (isSeparatorCharacter(calculatorInput.value[startPos]))
      break;
  var rightPos=startPos;
  var lastSeparator=startPos;
  var lastWord='';
  var currentChar='a';
  for (; rightPos<calculatorInput.value.length-1; rightPos++)
  { currentChar=calculatorInput.value[rightPos];
    if (currentChar===';')
    { if (rightPos>0)
        rightPos--;
      break;
    }
    if (isSeparatorCharacter(currentChar))
    { lastWord='';
      lastSeparator=rightPos;
    } else
      lastWord+=currentChar;
    if (lastWord.length>3)
      if (!isKeyWordStartKnownToMathQuill(lastWord))
      { rightPos=lastSeparator;
        break;
      }
  }
  var calculatorSeparatorCounts=
  new Array(calculatorSeparatorLeftDelimiters.length).fill(0);
  var leftPos=rightPos;
  lastWord='';
  lastSeparator=rightPos;
  for (; leftPos>0; leftPos--)
  { currentChar=calculatorInput.value[leftPos];
    if (currentChar===';')
    { leftPos++;
      break;
    }
    if (accountCalculatorDelimiterReturnMustEndSelection
        (calculatorInput.value, calculatorSeparatorCounts, leftPos)
       )
    { leftPos++;
      break;
    }
    if (isSeparatorCharacter(currentChar))
    { lastWord='';
      lastSeparator=leftPos;
    } else
    { lastWord=currentChar+lastWord;
    }
    if (lastWord.length>3)
      if (!isKeyWordEndKnownToMathQuill(lastWord))
      { leftPos=lastSeparator;
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
