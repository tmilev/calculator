"use strict";
const browserify = require('browserify');
const colors = require('colors');
const fs = require('fs');
const UglifyJS = require("uglify-js");
var currentDirectory = __dirname;


var inputFileName = `${currentDirectory}/include.js`;
var outputFileName = `${currentDirectory}/build/output.js`;
var outputUglified = `${currentDirectory}/build/output-min.js`;

function doBroswerify() {
  var theBrowserifier = browserify();
  theBrowserifier.add(`${currentDirectory}/include.js`);
  var theFileStream = fs.createWriteStream(outputFileName);
  console.log (`Proceeding to build file: ` + inputFileName.blue + ` into output: ` + outputFileName.green);
  theBrowserifier.bundle().pipe(theFileStream).on('close', ()=>{
    console.log("Non-minified file written. ");
    doUglify();
  });    
}

function doUglify() {
  var minifiedOutput = UglifyJS.minify(outputFileName);
  
  fs.writeFile(outputUglified, minifiedOutput.code, (err)=>{
    if (err) {
      console.log(err);
    }
    console.log('File written: ' + outputUglified.green);
  }); 
  //  fs.readFile(outputFileName,(err, data)=>{
//    if (err) {
//      throw err;
//    }
//    console.log(`Proceeding to uglify into: ` + outputUglified.green);
//    //console.log(`DEBUG: About to minify:` + data.toString());
//    //var options = { warnings: true };
//    //var code = {outputUglified: data.toString()};
//    var minifiedOutput = UglifyJS.minify(data.toString());
//    console.log("minified output: " + minifiedOutput.code);
//    console.log("Errors: " + minifiedOutput.error);
//    fs.writeFile(outputUglified, minifiedOutput.code, (err)=>{
//      console.log('File written: ' + outputUglified.green);
//    });  
//  });
}

doBroswerify();
