// The present file is the entry point of the entire
// javascript library.
"use strict";
class Browserifier {
  constructor() {
    // theJSContent is closure variable that is constructed by the calculator builder
    // and contains an object whose keys are the file names of the scripts.
    // The values of the object are functions that enclose the contents of each script.
    /**@type{Object<string,Function>} */
    this.theJSContent = theJSContent;
    document.onreadystatechange = () => {
      if (document.readyState !== 'complete') {
        return;
      }
      this.browserifyAndRun();
    }
    window.calculator = {
      browserifier: this,
    };
    this.allModules = {};
    this.sanitizedFileNameContents = {};
    this.calculatorHtmlBaseFolder = "/calculator_html/";
  }

  require(
    /**@type{string} */
    inputFileName,
  ) {
    let currentSource = this.sanitizedFileNameContents[inputFileName];
    if (!(inputFileName in this.allModules)) {
      this.allModules[inputFileName] = {};
    }
    let currentModule = this.allModules[inputFileName];
    if (!currentModule.flagLoaded) {
      currentModule.flagLoaded = true;
      currentSource(this.require.bind(this), currentModule);
    }
    return currentModule.exports;
  }

  browserifyAndRun() {
    let expectedStart = "/calculator_html/";
    for (let fileName in this.theJSContent) {
      let newFileName = "";
      if (!fileName.startsWith(expectedStart)) {
        continue;
      }
      var fileEnd = fileName.slice(expectedStart.length);
      newFileName = `./${fileEnd}`;
      this.sanitizedFileNameContents[newFileName] = theJSContent[fileName];
    }
    this.require('./app');
  }
}

// This is the first and only function call that will be executed by loading the javascript library.
new Browserifier();