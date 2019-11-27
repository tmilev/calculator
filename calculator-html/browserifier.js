"use strict";
function Browserifier() {
  this.allModules = {
  };
  this.sanitizedFileNameContents = {};
  this.calculatorHtmlBaseFolder = "/calculator-html/";
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
  createBrowserifier();
  var expectedStart = "/calculator-html/"; 
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

function createBrowserifier() {
  if (window.calculator !== undefined) {
    return;
  }
  window.calculator = {
    browserifier: new Browserifier(),
  };
}

function browserify() {
  createBrowserifier();
  window.calculator.browserifier.browserify();
}

createBrowserifier();