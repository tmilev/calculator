// The present file is the entry point of the entire
// javascript library.
"use strict";
/**
 * An object with keys the names of all scripts and values 
 * that are wrappers of the scripts.
 * 
 * Normally, require is a variable that contains a reference to  
 * the Browserifier.require function below.
 * However, this variable has no use in the present script
 * which defines the require function for the commonJS modules.
 * Therefore, the variable is free for our calculator to use for other 
 * purposes. Indeed, our calculator hijacks the require function 
 * it to inject the content of all of our commonJS modules.
 * It does so by declaring require to be an 
 * object with keys equal to the file names and values equal to
 * the contents of each file, wrapped in a 
 * function(require, module) { 
 * // content of the script goes here.
 * }
 * For example, require could contain:
 * var require = {
 *   "./app": function(require, module) {
 *   // content of the app.js file here. 
 *   },
 *   // ... and so on for the remaining files.
 * }
 * 
 * The values of the object are the 
 * function(require, module, export) { *** } 
 * enclosures of each script, with the script in place of the ***.
 * 
 * @type{Object<string,Function>} 
 */
var require;

/**
 * The entry script: the first require()-d script.
 * 
 * Normally, module is a variable that contains 
 * a reference to Browserifier.allModules object defined below.
 * However, this variable has no use in the present script which
 * defines the allModules object, and so the calculator
 * hijacks it to inject the name of the entry point script.
 *  
 * @type{string}
 */
var module;

class Browserifier {
  constructor() {
    this.jsContent = require;
    this.entryPoint = module;
    this.allModules = {};
    this.sanitizedFileNameContents = {};
    this.calculatorHtmlBaseFolder = "/calculator_html/";
  }

  require(
    /** @type{string} */
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
    for (let fileName in this.jsContent) {
      let newFileName = "";
      if (!fileName.startsWith(expectedStart)) {
        continue;
      }
      let fileEnd = fileName.slice(expectedStart.length);
      newFileName = `./${fileEnd}`;
      this.sanitizedFileNameContents[newFileName] = jsContent[fileName];
    }
    if (this.entryPoint === null) {
      this.entryPoint = "./app";
    }
    this.require(this.entryPoint);
  }
}

// This is the first and only function call that will be executed by loading the javascript library.
new Browserifier().browserifyAndRun();