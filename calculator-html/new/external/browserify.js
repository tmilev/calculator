"use strict";
const browserify = require('browserify');
const colors = require('colors');
const fs = require('fs');

var currentDirectory = __dirname;

var theBrowserifier = browserify();

theBrowserifier.add(`${currentDirectory}/include.js`);
var theFileStream = fs.createWriteStream(`${currentDirectory}/build/output.js`);
theBrowserifier.bundle().pipe(theFileStream);
