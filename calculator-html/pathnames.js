"use strict";

var urls = {
  calculatorAPI: "/cgi-bin/calculator",
  appNoCache: "/appNoCache",
  appWithCache: "/app",
};

var urlFields = {
  activationToken: "activationToken",
  request: "request",
  requests: {
    forgotLogin: "forgotLogin",
    monitor: "monitor",
    toggleMonitoring: "toggleMonitoring",
    userInfoJSON: "userInfoJSON",
    indicator: "indicator",
    mainInput: "mainInput",
    pause: "pause",
    slidesFromSource: "slidesFromSource",
    homeworkFromSource: "homeworkFromSource",
    sourceSlides: "slidesSource",
    sourceHomework: "homeworkSource",
    clonePage: "clonePageJSON",
    fileName: "fileName",
    modifyPage: "modifyPage"
  },
  mainInput: "mainInput",
  requestDatabase: "databaseJSON",
  requestEditPage: "editPageJSON", 
  calculatorCompute: "compute",
  databaseTable: "currentDatabaseTable",
  setProblemData: "setProblemData",
  debugFlag: "debugFlag",
  selectCourse: "selectCourseJSON",
  scoredQuizJSON: "scoredQuizJSON",
  changePassword: "changePassword",
  password: "password",
  newPassword: "newPassword",
  reenteredPassword: "reenteredPassword",
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
  username: "username",
  email: "email",
  recaptchaToken: "recaptchaToken",
}

module.exports = {
  urls,
  urlFields,
}