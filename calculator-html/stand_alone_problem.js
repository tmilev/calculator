"use strict";
const submitRequests = require("./submit_requests");
const problemPage = require("./problem_page");
const pathnames = require("./pathnames");

function StandAloneProblem() {
}

StandAloneProblem.prototype.initAndRun = function (input) {
  var problem = new problemPage.Problem(); 

  var courseHome = "/coursetemplates/Curriculum-300-Calculus-I.html";
  var topicList = "/topiclists/Curriculum-300-Calculus-I.txt";
  var problemFileName = "/problems/Find-function-inverse-fractional-linear-1.html";
  var theURL = `${pathnames.urls.calculatorAPI}?${problem.getCalculatorURLRequestInput(false, problemFileName, courseHome, topicList)}`;
  submitRequests.submitGET({
    url: theURL,
    callback: problemPage.updateProblemPageCallback,
  });  
}

var standAloneProblem = new StandAloneProblem();

module.exports = {
  standAloneProblem,
};