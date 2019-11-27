"use strict";
function Browserifier() {
  this.allModules = {
  };
  this.sanitizedFileNameContents = {};
}

Browserifier.prototype.require = function (inputFileName) {
  var currentSource = this.sanitizedFileNameContents[inputFileName];
  if (! (inputFileName in this.allModules)) {
    this.allModules[inputFileName] = {};
  }
  var currentModule = this.allModules[inputFileName];
  if (!currentModule.flagLoaded) {
    currentModule.flagLoaded = true;
    currentSource(this.require.bind(this), currentModule);
  }
  return currentModule.exports;
}

Browserifier.prototype.browserify = function() {
  var expectedStart = "/calculator" + "-html/"; 
  //<- please keep strings separated; the cpp builder auto-replaces the string /calculator-html
  //<- To do: fix this.
  for (var fileName in theJSContent) {
    var newFileName = "";
    if (!fileName.startsWith(expectedStart)) {
      continue;
    }
    var fileEnd = fileName.slice(expectedStart.length);
    newFileName = `./${fileEnd}`;
    this.sanitizedFileNameContents[newFileName] = theJSContent[fileName];
  }
  this.require('./app');
}

function browserify() {
  if (window.calculator === undefined) {
    window.calculator = {};
  }
  if (window.calculator.browserifier === undefined) {
    window.calculator.browserifier = new Browserifier();
  }
  window.calculator.browserifier.browserify();
}