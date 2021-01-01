"use strict";
class Browserifier {
  constructor(
    /**@type{string} */
    runMode,
    /**@type{string} */
    idToRunIn,
  ) {
    document.onreadystatechange = () => {
      if (document.readyState !== 'complete') {
        return;
      }
      this.initialize(runMode, idToRunIn);
    }
  }

  initialize(
    /**@type{string} */
    runMode,
    /**@type{string} */
    idToRunIn,
  ) {
    window.calculator = {
      browserifier: this,
    };
    this.allModules = {};
    this.sanitizedFileNameContents = {};
    this.calculatorHtmlBaseFolder = "/calculator-html/";
    if (runMode === "runInId") {
      this.browserifyRunInId(idToRunIn);
      return;
    }
    if (runMode === "editInId") {
      this.browserifyEditInId(idToRunIn);
      return;
    }
    this.browserifyAndRun();

  }

  require(inputFileName) {
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
    let expectedStart = "/calculator-html/";
    for (let fileName in theJSContent) {
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

  browserifyRunInId(input) {
    createBrowserifier();
    window.calculator.flagRunMainPage = false;
    window.calculator.browserifier.browserifyAndRun();
    window.calculator.standAloneProblem.standAloneProblem.initAndRun(input);
  }

  browserifyEditInId(input) {
    createBrowserifier();
    window.calculator.flagRunMainPage = false;
    window.calculator.browserifier.browserifyAndRun();
    window.calculator.standAloneProblem.standAloneProblem.initAndEdit(input);
  }
}

new Browserifier("", "");