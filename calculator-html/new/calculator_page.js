"use strict";
var calculatorMQString;
var calculatorMQStringIsOK = true;
var ignoreNextMathQuillUpdateEvent = false;
var calculatorLeftPosition = 0;
var calculatorRightPosition = 0;
var calculatorPlotUpdaters = {};
var calculatorInputBoxToSliderUpdaters = {};
var calculatorInputBoxNames = [];
var startingCharacterSectionUnderMathQuillEdit;
var calculatorCanvases;
var keyWordsKnownToMathQuill = ['sqrt', 'frac', 'cdot', 'left', 'right', 'infty', 'otimes', 'times', 'oplus', 'pmatrix','int', 'begin', 'end'];

function createSelectionNoFocus(field, start, end) { 
  if (field.createTextRange) { 
    var selRange = field.createTextRange();
    selRange.collapse(true);
    selRange.moveStart('character', start);
    selRange.moveEnd('character', end);
    selRange.select();
    field.focus();
    calculatorMQfield.focus();
  } else if (field.setSelectionRange){ 
    field.focus();
    field.setSelectionRange(start, end);
    calculatorMQfield.focus();
  } else if (typeof field.selectionStart != 'undefined') {
    field.selectionStart = start;
    field.selectionEnd = end;
    field.focus();
    calculatorMQfield.focus();
  }
}

function updateCalculatorSliderEventHandler() {
  event.preventDefault();
  var sliderName = calculatorInputBoxToSliderUpdaters[this.name];
  var theSliders = document.getElementsByName(sliderName);
  for (var counterSlider = 0; counterSlider < theSliders.length; counterSlider ++) {
    var currentSlider = theSliders[counterSlider];
    currentSlider.value = this.value;
  }
  updateCalculatorSliderToInputBox(this.name, sliderName);
}

function calculatorAddListenersToInputBoxes() {
  //var theString=" updating: box names, slider names: ";
  for (var i = 0; i < calculatorInputBoxNames.length; i ++) {
    var theBoxes = document.getElementsByName(calculatorInputBoxNames[i]);
    for (var j = 0; j < theBoxes.length; j ++) {
      theBoxes[j].addEventListener("input", updateCalculatorSliderEventHandler.bind(theBoxes[j]));
    }
  }
}

function updateCalculatorSliderToInputBox(boxName, sliderName) { 
  var theBoxes = document.getElementsByName(boxName);
  var theSliders = document.getElementsByName(sliderName);
  var sliderValue = theSliders[0].value;
  for (var i = 0; i < theBoxes.length; i ++) {
    theBoxes[i].value = sliderValue;
  }
  if (calculatorPlotUpdaters[sliderName] !== undefined) { 
    var theCanvas = calculatorCanvases[calculatorPlotUpdaters[sliderName]];
    if (theCanvas !== undefined) { 
      if (theCanvas.canvasResetFunction !== null) {
        theCanvas.canvasResetFunction();
      }
    }
  }
}

function createSelection(field, start, end) { 
  if (field.createTextRange) { 
    var selRange = field.createTextRange();
    selRange.collapse(true);
    selRange.moveStart('character', start);
    selRange.moveEnd('character', end);
    selRange.select();
    field.focus();
  } else if (field.setSelectionRange) { 
    field.focus();
    field.setSelectionRange(start, end);
  } else if (field.selectionStart !== undefined) { 
    field.selectionStart = start;
    field.selectionEnd = end;
    field.focus();
  }
}

function exampleCalculatorClick(theLink) {
  var theAtom = decodeURIComponent(theLink.atom);
  var theIndex = theLink.index;
  var isComposite = theLink.composite;
  var theHandler = null;
//console.log(theLink);
  if (isComposite){
    theHandler = theCalculatorExamples[theAtom].composite[theIndex];
  } else {
    theHandler = theCalculatorExamples[theAtom].regular[theIndex];
  }

//console.log(theHandler);
  var mainInput = document.getElementById("mainInputID");
  mainInput.value = theHandler.example;
  submitCalculatorComputation();
}

function processOneFunctionAtom(handlers, isComposite) {
  var resultString = "";
  for (var counterHandlers = 0; counterHandlers < handlers.length; counterHandlers ++) {
    resultString += "<br>";
    var currentDescription = handlers[counterHandlers].description;
    var currentExample = handlers[counterHandlers].example;
    resultString += `<calculatorAtom>${handlers[counterHandlers].atom}</calculatorAtom>`;
    resultString += "<calculatorCompositeAtom>(composite)</calculatorCompositeAtom>";
    resultString += ` (${counterHandlers + 1} out of ${handlers.length})`;
    var currentId = "example_";
    if (isComposite) {
      currentId += "t_";
    } else {
      currentId += "f_";
    }
    var encodedAtom = encodeURIComponent(handlers[counterHandlers].atom);
    currentId += `${encodedAtom}_${counterHandlers}_${handlers.length}`;
    resultString += `<a href = '#' class = 'linkInfo' onclick = "switchMenu('${currentId}')">info</a>`;
    resultString += `<calculatorExampleInfo id = "${currentId}" class = "hiddenClass">${currentDescription}<br><b>Example:</b><br>${currentExample}</calculatorExampleInfo>`;
    resultString += `<a href = "#" class = "linkInfo" onclick = "this.composite =${isComposite}; this.index =${counterHandlers}; this.atom='${encodedAtom}'; exampleCalculatorClick(this);"> Example</a>`;
    //resultString += currentExample;
    //console.log(handlers[counterHandlers]);
  }
  return resultString;
}

var theCalculatorExamples;

function processExamples(inputJSONtext) {
  try {
    theCalculatorExamples = JSON.parse(inputJSONtext);
    var examplesString = "";
    var atomsSorted = Object.keys(theCalculatorExamples).slice().sort();
    var numHandlers = 0;
    for (var counterAtoms = 0; counterAtoms < atomsSorted.length; counterAtoms ++) {
      var atom = atomsSorted[counterAtoms];
      examplesString += processOneFunctionAtom(theCalculatorExamples[atom].regular, false);
      examplesString += processOneFunctionAtom(theCalculatorExamples[atom].composite, false);
      numHandlers += theCalculatorExamples[atom].regular.length + theCalculatorExamples[atom].composite.length;
    }
    var resultString = `${atomsSorted.length} built-in atoms, ${numHandlers} handlers. `;
    resultString += examplesString;
    document.getElementById("divCalculatorExamples").innerHTML = resultString;
  } catch (e) {
    console.log(`Bad json: ${e}\n Input JSON follows.`);
    console.log(inputJSONtext);
  }
}

function toggleCalculatorExamples(theButton) {
  var theExamples = document.getElementById('divCalculatorExamples');
  if (theExamples.innerHTML.length < 300) {
    submitGET({
      url: `${pathnames.calculatorAPI}?request=calculatorExamplesJSON`,
      callback: processExamples,
      progress: "spanProgressCalculatorExamples"
    });
    theButton.innerHTML = "&#9660;";
  } else {
    switchMenu('divCalculatorExamples');
    if (! theExamples.classList.contains("hiddenClass")) {
      theButton.innerHTML = "&#9660;";
    } else {
      theButton.innerHTML = "&#9656;";
    }
  }
}
