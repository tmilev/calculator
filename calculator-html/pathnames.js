"use strict";

var urls = {
  calculatorAPI: "/cgi-bin/calculator",
  appNoCache: "/appNoCache",
  appWithCache: "/app",
};

var urlFields = {
  request: "request",
  mainInput: "mainInput",
  fileName: "fileName",
  requestDatabase: "databaseJSON",
  requestEditPage: "editPageJSON", 
  requestModifyPage: "modifyPage",
  requestClonePage: "clonePageJSON",
  databaseTable: "currentDatabaseTable",
  setProblemData: "setProblemData",
  debugFlag: "debugFlag",
  selectCourse: "selectCourseJSON",
  scoredQuizJSON: "scoredQuizJSON",
  exerciseJSON: "exerciseJSON",
  problemGiveUp: "problemGiveUp",
  problemGiveUpNoLogin: "problemGiveUpNoLogin",
  randomSeed: "randomSeed",
  problemSolution: "problemSolution",
  problemSolutionNoLogin: "problemSolutionNoLogin",
  submitAnswers: "submitAnswers",
  submitExercise: "submitExercise",
  submitExerciseNoLogin: "submitExerciseNoLogin",
  submitAnswersPreview: "submitAnswersPreview",
  submitExercisePreviewNoLogin: "submitExercisePreviewNoLogin",
  submitExercisePreview: "submitExercisePreview",  
  accountsJSON: "accountsJSON",
  signUp: "signUp",
}

module.exports = {
  urls,
  urlFields,
}