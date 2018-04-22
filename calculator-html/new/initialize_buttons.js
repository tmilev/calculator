"use strict";
var studentScoresInHomePage = [];
//var lastFocus;
var charsToSplit = ['x','y'];
var panelsCollapseStatus = {};
var calculatorSeparatorLeftDelimiters = ['(', '{'];
var calculatorSeparatorRightDelimiters = [')', '}'];
var panelDataRegistry = {};
var calculatorPanel = null;

function processMathQuillLatex(theText){ 
  for (var i = 0; i < theText.length; i ++){
    if (i + 1 < theText.length){ 
      if ((theText[i] === '_' || theText[i] === '^') && theText[i + 1] !== '{'){
        theText = theText.slice(0, i + 1) + '{' + theText[i + 1] + '}' + theText.slice(i + 2);
      }
      if (theText[i] === '\\' && theText[i + 1] === '\\'){
        theText = theText.slice(0, i + 2) + ' ' + theText.slice(i + 2);
      }
    }
  }
  if (charsToSplit !== undefined){
    for (var i = 0; i < theText.length - 1; i ++){ 
      for (var j = 0; j < charsToSplit.length; j ++){
        if (theText[i] === charsToSplit[j] && theText[i + 1] !== ' ' && 
            theText[i + 1] !== '\\' && theText[i + 1] !== '+' && 
            theText[i + 1] !== '*' && theText[i + 1] !== '/' && 
            theText[i + 1] !== '-' && theText[i + 1] !== '='){ 
          if (theText[i] === 'x'){
            if (theText.slice(i - 5, i + 1) === 'matrix'){
              continue;
            }
          }
          theText = theText.slice(0, i + 1) + " " + theText.slice(i + 1);
        }
      }
    }
  }
  return theText;
}

function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

var MathQuillCommandButtonCollection = {};

function mathQuillCommandButton(inputCommand, inputLabel, additionalStyle, doWriteInsteadOfCmdInput, inputExtraDirection){ 
  var commandObject = {};
  commandObject.theCommand = inputCommand;
  commandObject.theLabel = inputLabel;
  commandObject.isComposite = false;
  commandObject.extraDirection = inputExtraDirection;
  if (typeof(inputCommand) !== "string"){
    commandObject.isComposite = true;
  }
  if (!commandObject.isComposite){
    commandObject.id = commandObject.theCommand;
  } else { 
    commandObject.id = "";
    for (var i = 0; i < inputCommand.length; i ++){
      commandObject.id += inputCommand[i];
    }
  }
//  console.log(commandObject.id);
  if (doWriteInsteadOfCmdInput !== undefined){
    commandObject.doWriteInsteadOfCmd = doWriteInsteadOfCmdInput;
  } else {
    commandObject.doWriteInsteadOfCmd = false;
  }
  commandObject.getButton = function (indexMathField){ 
    var resultString = "";
    resultString += "<button style='";
    resultString += "width:25; height:21.2; padding:0; ";
    if (additionalStyle !== "" && additionalStyle !== undefined){
      resultString += additionalStyle;
    }
    resultString += "'";
    var commandObjectIDescaped = "";
    for (var i = 0; i < commandObject.id.length; i ++){ 
      commandObjectIDescaped += commandObject.id[i];
      if (commandObject.id[i] === '\\'){
        commandObjectIDescaped += "\\";
      }
    }
    resultString += " onmousedown=\"MathQuillCommandButtonCollection['" +
    commandObjectIDescaped + "']." +
    "clickFunction(answerMathQuillObjects[" + indexMathField + "]);\">" +
    commandObject.theLabel + "</button>";
    return resultString;
  }
  commandObject.clickFunction = function(currentMathField){ 
    if (!commandObject.isComposite){ 
      if (!this.doWriteInsteadOfCmd){
        currentMathField.cmd(commandObject.theCommand);
      } else {
        currentMathField.write(commandObject.theCommand);
      }
    } else { 
      for (var i = 0; i < commandObject.theCommand.length; i ++){ 
        var doCMD = !this.doWriteInsteadOfCmd;
        if (!doCMD){
          if (commandObject.theCommand[i] === '(' ||
              commandObject.theCommand[i] === ')' ||
              commandObject.theCommand[i] === ' '){
            doCMD = true;
          }
        }
        if (doCMD){
          currentMathField.cmd(commandObject.theCommand[i]);
        } else {
          currentMathField.write(commandObject.theCommand[i]);
        }
      }
    }
    if (commandObject.extraDirection !== undefined){
      currentMathField.moveToDirEnd(MQ.L);
    }
    currentMathField.focus();
    event.preventDefault();
  }
  MathQuillCommandButtonCollection[commandObject.id] = commandObject;
}

mathQuillCommandButton("+","+");
mathQuillCommandButton("-","-");
mathQuillCommandButton("*","*");
mathQuillCommandButton("/","/");
mathQuillCommandButton("sqrt","&#8730;");
mathQuillCommandButton("nthroot","&#8731;");
mathQuillCommandButton("^","^");
mathQuillCommandButton("(","(");
mathQuillCommandButton(")",")");
mathQuillCommandButton(",",",");
mathQuillCommandButton("[","[");
mathQuillCommandButton("]","]");
mathQuillCommandButton("i","i");
mathQuillCommandButton("x","x");
mathQuillCommandButton("y","y");
mathQuillCommandButton("=","=");
mathQuillCommandButton(["log","_"],"log_", "font-size:10px; ");
mathQuillCommandButton("_","_");
mathQuillCommandButton("ln","ln");
mathQuillCommandButton("e","e");
mathQuillCommandButton("arcsin","asin", "font-size:7px");
mathQuillCommandButton("arccos","acos", "font-size:7px");
mathQuillCommandButton("arctan","atan", "font-size:7px");


mathQuillCommandButton("sin","sin", "font-size:10px; ");
mathQuillCommandButton("cos","cos", "font-size:10px; ");
mathQuillCommandButton("tan","tan", "font-size:10px; ");
mathQuillCommandButton("cot","cot", "font-size:10px; ");
mathQuillCommandButton("sec","sec", "font-size:10px; ");
mathQuillCommandButton("csc","csc", "font-size:10px; ");

mathQuillCommandButton("emptyset", "&#8709;");
mathQuillCommandButton("alpha", "&alpha;");
mathQuillCommandButton("beta", "&beta;");
mathQuillCommandButton("gamma", "&gamma;");
mathQuillCommandButton("theta", "&theta;");
mathQuillCommandButton("pi", "&pi;");
mathQuillCommandButton("sum", "&#8721;");
mathQuillCommandButton("infty", "&#8734;");
mathQuillCommandButton(" DNE ", "DNE", "font-size: 7px");
mathQuillCommandButton("!", "!");
mathQuillCommandButton("binom", "binom", "font-size : 7px;");
mathQuillCommandButton("cup", "&#8746;");
mathQuillCommandButton(" or ", "or");
mathQuillCommandButton("in", "&#8712;");
mathQuillCommandButton(["^","circ"], "&#176;");
mathQuillCommandButton("circ", "&#9675;");
mathQuillCommandButton(["NewtonsMethod ","(", ",", ",", ")"], "Newton", "font-size: 6px", false);
mathQuillCommandButton("\\begin{pmatrix} \\\\ \\end{pmatrix}", "2x1","font-size : 7px;", true);
mathQuillCommandButton("\\begin{pmatrix} \\\\ \\\\ \\end{pmatrix}", "3x1", "font-size : 7px;", true);
mathQuillCommandButton("\\begin{pmatrix} & \\\\ & \\end{pmatrix}", "2x2", "font-size : 7px;", true);
mathQuillCommandButton("\\begin{pmatrix} & & \\\\ & & \\\\ & & \\end{pmatrix}", "3x3", "font-size : 7px;", true);

mathQuillCommandButton("\\mathbf{i}", "i", "font-weight: bold");
mathQuillCommandButton("\\mathbf{j}", "j", "font-weight: bold");
mathQuillCommandButton("\\mathbf{k}", "k", "font-weight: bold");

function accountCalculatorDelimiterReturnMustEndSelection(text, calculatorSeparatorCounts, thePos){ 
  var result = false;
  for (var j = 0; j < calculatorSeparatorLeftDelimiters.length; j ++){ 
    if (calculatorSeparatorLeftDelimiters[j] === text[thePos]){
      calculatorSeparatorCounts[j] ++;
    }
    if (calculatorSeparatorRightDelimiters[j] === text[thePos]){
      calculatorSeparatorCounts[j] --;
    }
    if (calculatorSeparatorLeftDelimiters[j] > 0){
      result = true;
    }
  }
  return result;
}

function initializeAccordionButtons(){ 
  ///initializing accordions
  if (localStorage !== undefined) {
    if (localStorage.panels !== undefined){ 
      panelsCollapseStatus = JSON.parse(localStorage.panels)
      var theProps=Object.getOwnPropertyNames(panelsCollapseStatus);
      for (var i = 0; i < theProps.length; i ++){ 
        var current = panelsCollapseStatus[theProps[i]];
        if (current.isCollapsed) {
          toggleHeight(document.getElementById(current.button),theProps[i]);
        }
      }
    }
  }
  var acc = document.getElementsByClassName("accordion");
  for (i = 0; i < acc.length; i ++){ 
    acc[i].onclick = function(){ 
      if (this.firstLoad === undefined){ 
        this.firstLoad = true;
        var theDeadlines = this.nextElementSibling.getElementsByClassName("modifyDeadlineInput");
        for (var j = 0; j < theDeadlines.length; j ++){
          $('#' + theDeadlines[j].id).datepicker();
        }
//        console.log("first run: "+theDeadlines);
        this.nextElementSibling.style.display = "inline-block";
        //await sleep(400);
      }
      //this.nextElementSibling.style.transition="0.6s linear";
      this.classList.toggle("active");
      this.nextElementSibling.classList.toggle("show");
    }
  }
}

function initializeButtons(){ 
  initializeAccordionButtons();
}

function registerStatus(thePanel, isCollapsed){ 
  panelsCollapseStatus[thePanel.id] = {
    'button' : thePanel.buttonThatModifiesMe.id, 
    'isCollapsed' : isCollapsed
  };
  if (localStorage === undefined){
    return;
  }
  localStorage.panels = JSON.stringify(panelsCollapseStatus);
}

function transitionDone(event){ 
  //console.log("CAlled transitionDone");
  this.removeEventListener("transitionend", transitionDone);
  if (this.transitionState === "collapsing"){ 
    this.style.display = "none";
    this.transitionState = "collapsed";
    registerStatus(this, true)
  } else if (this.transitionState === "expanding"){ 
    this.style.display = "";
    this.transitionState = "expanded";
    registerStatus(this, false)
  }
}

function toggleHeightForTimeout(currentPanel){ 
  if (currentPanel.transitionState === "expanding"){ 
    currentPanel.style.maxHeight = currentPanel.startingMaxHeight;
    currentPanel.style.height = currentPanel.startingMaxHeight;
    currentPanel.style.opacity = "1";
  } else if (currentPanel.transitionState === "collapsing"){ 
    currentPanel.style.opacity = "0";
    currentPanel.style.maxHeight = "0px";
    currentPanel.style.height = "0px";
  }
}


function initializeCalculatorPage(){ 
  if (calculatorPanel !== null) {
    return;
  }
  calculatorPanel = new InputPanelData({
    idMQSpan: "mainInputMQfield",
    idMQcomments: "mqPanelComments",
    fileName: "",
    idPureLatex: "mainInputID",
    idButtonContainer: 'mainInputMQfieldButtons',
    flagCalculatorPanel: true

  });
  calculatorPanel.initialize();
}


function InputPanelData(input){
  this.idMQSpan = input.idMQSpan;
  this.idMQcomments = input.idMQcomments;
  this.fileName = input.fileName;

  this.idPureLatex = input.idPureLatex;
  this.idButtonContainer = input.idButtonContainer;
  panelDataRegistry[this.idButtonContainer] = this;
  this.mqObject = null;
  this.ignoreNextMathQuillUpdateEvent = false;
  this.flagAnswerPanel = input.flagAnswerPanel;
  this.flagInitialized = false;
  this.flagCalculatorPanel = input.flagCalculatorPanel;
  this.flagCalculatorMQStringIsOK = true;
  this.calculatorLeftString = "";
  this.calculatorRightString = "";
  this.theLaTeXString = null;
  this.selectionEnd = 0;
  this.javascriptInsertionAlreadyCalled = false;
  this.numInsertedJavascriptChildren = 0;
  this.timerForPreviewAnswers = 0;

  if (this.flagCalculatorPanel === undefined){
    this.flagCalculatorPanel = false;
  }
  if (this.idMQcomments === undefined){
    this.idMQcomments = null;
  }
}

InputPanelData.prototype.mQHelpCalculator = function(){ 
  //event.preventDefault();
  this.getSemiColumnEnclosure();
  if (this.mqObject === null) {
    return;
  }
  this.ignoreNextMathQuillUpdateEvent = true;
  if (this.flagCalculatorMQStringIsOK) {
    this.mqObject.latex(this.theLaTeXString);
  }
  this.ignoreNextMathQuillUpdateEvent = false;
}


InputPanelData.prototype.submitOrPreviewAnswersCallback = function (input, outputComponent){
  outputComponent.innerHTML = input;
  var scripts = spanVerification.getElementsByTagName('script');
  var theHead = document.getElementsByTagName('head')[0];
  for (var i = 0; i < this.numInsertedJavascriptChildren; i++){
    theHead.removeChild(theHead.lastChild);
  }
  this.numInsertedJavascriptChildren = 0;
  for (var i = 0; i < scripts.length; i ++){
    var scriptChild = document.createElement('script');
    scriptChild.innerHTML = scripts[i].innerHTML;
    scriptChild.type = 'text/javascript';
    theHead.appendChild(scriptChild);
    this.numInsertedJavascriptChildren++;
  }
  this.javascriptInsertionAlreadyCalled = true;
  MathJax.Hub.Queue(['Typeset', MathJax.Hub, outputComponent]);
}

InputPanelData.prototype.submitOrPreviewAnswers = function(requestType){
  clearTimeout(this.timerForPreviewAnswers);
  var theURL = `${pathnames.calculator}?request=${theRequest}`;
  
  var theRequest = "submitExercisePreview"; //"submitProblemPreview"
  submitGET({
    url: theURL,
    progress: "spanProgressCalculatorExamples",
    callback: this.submitOrPreviewAnswersCallback,
  });

  inputParams += `&calculatorAnswer${idAnswer}=${encodeURIComponent(spanStudentAnswer.value)};`;
}

InputPanelData.prototype.giveUp = function(){
  clearTimeout(this.timerForPreviewAnswers);
  params= "";
}

InputPanelData.prototype.showSolution = function(){
  clearTimeout(this.timerForPreviewAnswers);
}

InputPanelData.prototype.submitPreview = function() {  

}

InputPanelData.prototype.previewAnswers = function() { // useful event handlers
  clearTimeout(this.timerForPreviewAnswers);
  this.timerForPreviewAnswers = setTimeout(function(){
    this.submitPreview();
  }, 4000);
}

InputPanelData.prototype.editLaTeX = function() { // useful event handlers
  //console.log("here i am");
  this.ignoreNextMathQuillUpdateEvent = true;
  this.mqObject.latex(document.getElementById(this.idPureLatex).value + ' ');
  this.ignoreNextMathQuillUpdateEvent = false;
  this.previewAnswers();
}

InputPanelData.prototype.editMQFunction = function() { // useful event handlers
  if (this.ignoreNextMathQuillUpdateEvent) {
    return;
  }
  if (this.flagAnswerPanel){
    document.getElementById(this.idPureLatex).value = processMathQuillLatex(this.mqObject.latex()); // simple API
    this.previewAnswers();
    return;
  }
  if (this.flagCalculatorPanel){
    if (!this.flagCalculatorMQStringIsOK) {
      return;
    }
    var theBoxContent = this.mqObject.latex();
    if (this.calculatorLeftString === null || this.calculatorRightString === null){
      this.mQHelpCalculator();
    }
    var theInserted = processMathQuillLatex(theBoxContent);
    if (theInserted.length > 0 && startingCharacterSectionUnderMathQuillEdit.length > 0){
      if (theInserted[0] !== ' '){
        theInserted = ' ' + theInserted;
      }
    }
    document.getElementById(this.idPureLatex).value = this.calculatorLeftString + theInserted + this.calculatorRightString;
  }
}

InputPanelData.prototype.initialize = function(){
  if (this.flagInitialized){
    return;
  }
  var currentSpanVariable = document.getElementById(this.idPureLatex);
  globalMQ.config({
    autoFunctionize: 'sin cos tan sec csc cot log ln'
  });
  var currentMQspan = document.getElementById(this.idMQSpan);
  var editMQFunctionBound = this.editMQFunction.bind(this);
  this.mqObject = globalMQ.MathField(currentMQspan, {
    spaceBehavesLikeTab: true, // configurable
    supSubsRequireOperand: true, // configurable
    autoSubscriptNumerals: true, // configurable
    handlers: {
      edit: editMQFunctionBound
    }
  });
  this.initializePartTwo(false);
}

function isSeparatorCharacter(theChar){ 
  if (theChar[0] >= 'a' && theChar[0] <= 'z'){
    return false;
  }
  if (theChar[0] >= 'A' && theChar[0] <= 'Z'){
    return false;
  }
  return true;
}

InputPanelData.prototype.chopStrings = function(){ 
  var mqCommentsSpan = document.getElementById(this.idMQcomments);
  if (calculatorRightPosition - calculatorLeftPosition > 1000){ 
    this.flagCalculatorMQStringIsOK = false;
    mqCommentsSpan.innerHTML = "<span style='color:red'><b>Formula too big </b></span>";
    return;
  }
  this.flagCalculatorMQStringIsOK = true;
  mqCommentsSpan.innerHTML = "Equation assistant";
  document.getElementById(this.idMQSpan).style.visibility = "visible";
  var calculatorInput = document.getElementById(this.idPureLatex);
  this.theLaTeXString = calculatorInput.value.
  substring(calculatorLeftPosition, calculatorRightPosition + 1);
  this.calculatorLeftString = calculatorInput.value.
  substring(0, calculatorLeftPosition);
  this.calculatorRightString = calculatorInput.value.
  substring(calculatorRightPosition + 1);
}

function isKeyWordStartKnownToMathQuill(input){ 
  for (var i = 0; i < keyWordsKnownToMathQuill.length; i ++){
    if (keyWordsKnownToMathQuill[i].startsWith(input)){
      return true;
    }
  }
  return false;
}

function isKeyWordEndKnownToMathQuill(input){ 
  for (var i = 0; i < keyWordsKnownToMathQuill.length; i ++){
    if (keyWordsKnownToMathQuill[i].endsWith(input)){
      return true;
    }
  }
  return false;
}

InputPanelData.prototype.getSemiColumnEnclosure = function(){ 
  var startPos = this.selectionEnd;
  var calculatorInput = document.getElementById(this.idPureLatex);
  for (; startPos > 0 && startPos < calculatorInput.value.length; startPos --){
    if (isSeparatorCharacter(calculatorInput.value[startPos])){
      break;
    }
  }
  if (startPos >= calculatorInput.value.length){
    startPos = calculatorInput.value.length - 1;
  }
  var rightPos = startPos;
  var lastSeparator = startPos;
  var lastWord = '';
  var currentChar = 'a';
  for (; rightPos < calculatorInput.value.length - 1; rightPos ++){ 
    currentChar = calculatorInput.value[rightPos];
    if (currentChar === ';'){ 
      if (rightPos > 0){
        rightPos --;
      }
      break;
    }
    if (isSeparatorCharacter(currentChar)){ 
      lastWord = '';
      lastSeparator = rightPos;
    } else {
      lastWord += currentChar;
    }
    if (lastWord.length > 3){
      if (!isKeyWordStartKnownToMathQuill(lastWord)){ 
        rightPos = lastSeparator;
        break;
      }
    }
  }
  var calculatorSeparatorCounts = new Array(calculatorSeparatorLeftDelimiters.length).fill(0);
  var leftPos = rightPos - 1;
  lastWord = '';
  lastSeparator = rightPos;
  for (; leftPos > 0; leftPos --){ 
    currentChar = calculatorInput.value[leftPos];
    if (currentChar === ';'){ 
      leftPos ++;
      break;
    }
    if (accountCalculatorDelimiterReturnMustEndSelection(calculatorInput.value, calculatorSeparatorCounts, leftPos)){ 
      leftPos ++;
      break;
    }
    if (isSeparatorCharacter(currentChar)){ 
      lastWord = '';
      lastSeparator = leftPos;
    } else { 
      lastWord = currentChar + lastWord;
    }
    if (lastWord.length > 3) {
      if (!isKeyWordEndKnownToMathQuill(lastWord)){ 
        leftPos = lastSeparator;
        break;
      }
    }
  }
  if (leftPos > rightPos){
    leftPos = rightPos;
  }
  if (rightPos - leftPos > 1000){
    mqProblemSpan.innerHTML = "<span style='color:red'><b></b></span>"
  }
  calculatorLeftPosition = leftPos;
  calculatorRightPosition = rightPos;
  startingCharacterSectionUnderMathQuillEdit = '';
  if (calculatorInput.value[leftPos] === '\n' || calculatorInput.value[leftPos] === ' ' ||
      calculatorInput.value[leftPos] === '\t'){
    startingCharacterSectionUnderMathQuillEdit = calculatorInput.value[leftPos];
  }
  this.chopStrings();
}

InputPanelData.prototype.initializePartTwo = function(forceShowAll){ 
  var currentButtonPanel = document.getElementById(this.idButtonContainer);
  var buttonArray = currentButtonPanel.attributes.buttons.value.toLowerCase().split(/(?:,| )+/);
  //console.log(buttonArray);
  var buttonBindings = [];
  function addBinding(theFun){ 
    if (buttonBindings.indexOf(theFun) > - 1){
      return;
    }
    buttonBindings.push(theFun);
  }
  function addCommand(theCmd){ 
    buttonBindings.push(MathQuillCommandButtonCollection[theCmd].getButton);
  }
  var noOptions = false;
  var includeAll = false;
  if (forceShowAll){
    includeAll = true;
  }
  if (buttonArray.indexOf("all") > - 1){
    includeAll = true;
  }
  if (buttonArray.length === 0){
    noOptions = true;
  }
  if (buttonArray.length === 1){
    if (buttonArray[0] === ""){
      noOptions = true;
    }
  }
  if (buttonArray.indexOf("algebra") > - 1 || noOptions || includeAll){ 
    addCommand("+");
    addCommand("-");
    addCommand("*");
    addCommand("/");

    addCommand("sqrt");
    addCommand("nthroot");
    addCommand("^");
    addCommand("(");
    addCommand(")");
  }
  if (buttonArray.indexOf("trig") > - 1 || includeAll){ 
    addCommand("sin");
    addCommand("cos");
    addCommand("tan");
    addCommand("cot");
    addCommand("sec");
    addCommand("csc");
  }
  if (buttonArray.indexOf("inversetrig") > - 1 ||
      buttonArray.indexOf("inverse_trig") > - 1 ||
      buttonArray.indexOf("inverse-trig") > - 1 ||
      buttonArray.indexOf("inverseTrig") > - 1 ||
      buttonArray.indexOf("InverseTrig") > - 1 ||
      buttonArray.indexOf("inversetrigonometry") > - 1 ||
      buttonArray.indexOf("inverse_trigonometry") > - 1 ||
      buttonArray.indexOf("inverse-trigonometry") > - 1 ||
      includeAll){ 
    addCommand("arcsin");
    addCommand("arccos");
    addCommand("arctan");
  }
  if (buttonArray.indexOf("brackets") > - 1 ||
      buttonArray.indexOf("comma") > - 1 ||
      buttonArray.indexOf("commas") > - 1 ||
      buttonArray.indexOf("intervals") > - 1 ||
      includeAll){
    addCommand(",");
  }
  if (buttonArray.indexOf("brackets") > - 1 ||
      buttonArray.indexOf("intervals") > - 1 || includeAll){ 
    addCommand("[");
    addCommand("]");
  }
  if (buttonArray.indexOf("complex") > - 1 || buttonArray.indexOf("imaginary") > - 1 || includeAll){ 
    addCommand("i");
  }
  if (buttonArray.indexOf("variables") > - 1 || includeAll){ 
    addCommand("x");
    addCommand("y");
    addCommand("=");
  }
  if (buttonArray.indexOf("logarithms") > - 1 || noOptions || includeAll){ 
    addCommand("log_");
    addCommand("_");
    addCommand("ln");
    addCommand("e");
  }
  if (buttonArray.indexOf("infinity") > - 1 || buttonArray.indexOf("infty") > - 1 ||
      buttonArray.indexOf("\infty") > - 1 ||
      buttonArray.indexOf("interval") > - 1 ||
      buttonArray.indexOf("intervals") > - 1 ||
      buttonArray.indexOf("limits") > - 1 ||
      buttonArray.indexOf("limit") > - 1 ||
      includeAll || noOptions){
    addCommand("infty");
  }
  if (buttonArray.indexOf("limits") > - 1 ||
      buttonArray.indexOf("limit") > - 1 ||
      includeAll){
    addCommand(" DNE ");
  }
  if (buttonArray.indexOf("sum") > - 1 ||
      buttonArray.indexOf("series") > - 1 || noOptions || includeAll){ 
    addCommand("binom");
    addCommand("!");
    addCommand("sum");
  }
  if (noOptions || includeAll){ 
    addCommand("circ");
  }
  if (buttonArray.indexOf("interval") > - 1 || buttonArray.indexOf("intervals") > - 1 ||
      buttonArray.indexOf("or") > - 1 || noOptions || includeAll){
    addCommand(" or ");
  }
  if (buttonArray.indexOf("interval") > - 1 || buttonArray.indexOf("intervals") > - 1 || noOptions || includeAll){ 
    addCommand("cup");
    addCommand("in");
    addCommand("emptyset");
  }
  if (buttonArray.indexOf("matrix") > - 1 || buttonArray.indexOf("matrices") > - 1 || (includeAll && MathQuillHasMatrixSupport)){ 
    addCommand("\\begin{pmatrix} \\\\ \\end{pmatrix}");
    addCommand("\\begin{pmatrix} \\\\ \\\\ \\end{pmatrix}");
    addCommand("\\begin{pmatrix} & \\\\ & \\end{pmatrix}");
    addCommand("\\begin{pmatrix} & & \\\\ & & \\\\ & & \\end{pmatrix}");
  }
  if (buttonArray.indexOf("angle") > - 1 || buttonArray.indexOf("angles") > - 1 || noOptions || includeAll){ 
    addCommand("pi");
    addCommand("^circ");
    addCommand("alpha");
    addCommand("beta");
    addCommand("gamma");
    addCommand("theta");
  }
  if (buttonArray.indexOf("NewtonsMethod") > - 1 ||
      buttonArray.indexOf("newtonsmethod") > - 1 ||
      buttonArray.indexOf("NewtonMethod") > - 1 ||
      buttonArray.indexOf("newtonmethod") > - 1 ||
      buttonArray.indexOf("newton") > - 1 ||
      includeAll){ 
    addCommand("NewtonsMethod (,,)");
  }
  var theContent = "<table>";
  var numButtonsPerLine = 4;
  for (var j = 0; j < buttonBindings.length; j ++){ 
    if (j % numButtonsPerLine === 0){ 
      if (j !== 0){
        theContent += "</tr>";
      }
      theContent += "<tr>";
    }
    theContent += "<td>" + buttonBindings[j](this.idButtonContainer) + "</td>";
  }
  if (buttonBindings.length > 0){
    theContent += "</tr>";
  }
  theContent += "</table>";
  if (!forceShowAll && !includeAll){
    theContent+= `<small><a href=\"#\" onclick=\"panelDataRegistry['${this.idButtonContainer}'].initializePartTwo(true);\">Show all</a></small>`;
  } else {
    theContent+= `<small><a href=\"#\" onclick=\"panelDataRegistry['${this.idButtonContainer}'].initializePartTwo(false);\">Show relevant</a></small>`;
  }
  var oldHeight = window.getComputedStyle(currentButtonPanel).height;
  //console.log("oldHeight: " + oldHeight);
  currentButtonPanel.style.maxHeight = "";
  currentButtonPanel.style.height = "";
  currentButtonPanel.innerHTML = theContent;
  if (oldHeight !== 0 && oldHeight !== "0px"){ 
    var newHeight = window.getComputedStyle(currentButtonPanel).height;
    currentButtonPanel.style.maxHeight = oldHeight ;
    currentButtonPanel.style.height = oldHeight ;
    setTimeout(function(){ 
      modifyHeightForTimeout(currentButtonPanel, newHeight)
    }, 0);
  }
  return false;
}

