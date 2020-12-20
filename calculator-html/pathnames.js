"use strict";

const urls = {
  calculatorAPI: "/cgi-bin/calculator",
  appNoCache: "/appNoCache",
  appWithCache: "/app",
};

const urlFields = {
  activationToken: "activationToken",
  request: "request",
  requests: {
    forgotLogin: "forgotLogin",
    toggleMonitoring: "toggleMonitoring",
    userInfoJSON: "userInfoJSON",
    loginDisabledEveryoneIsAdmin: "loginDisabledEveryoneIsAdmin",
    useFallbackDatabase: "useFallbackDatabase",
    httpsSupport: "httpsSupport",
    indicator: "indicator",
    pause: "pause",
    unpause: "unpause",
    slidesFromSource: "slidesFromSource",
    homeworkFromSource: "homeworkFromSource",
    sourceSlides: "slidesSource",
    sourceHomework: "homeworkSource",
    clonePage: "clonePageJSON",
    modifyPage: "modifyPage",
    setProblemWeight: "setProblemWeight",
    setProblemDeadline: "setProblemDeadline",
    setTeacher: "setTeacher",
    database: "database",
    serverStatusJSON: "serverStatusJSON",
    workerId: "workerId",
    monitoring: "monitoring",
    calculatorInput: "calculatorInput",
    fileContent: "fileContent",
    calculatorExamplesJSON: "calculatorExamplesJSON",
    solveJSON: "solveJSON",
  },
  result: {
    resultHtml: "resultHtml",
    crashReport: "crashReport",
    error: "error",
    // Comments on performance.
    performance: "performance",
    // Computation time. Excludes https traffic.
    computationTime: "computationTime",
    // Comments for the specific task.
    comments: "comments",
    // Global comments, usually used for debugging.
    commentsGlobal: "commentsGlobal",
  },
  problem: {
    failedToLoadProblem: "Failed to load problem.",
    problemLabel: "problemLabel",
    content: "content",
    debugInformation: "debugInformation",
    fileName: "fileName",
    courseHome: "courseHome",
    fileNameTarget: "fileNameTarget",
    topicList: "topicList",
  },
  teachersAndSections: "teachersAndSections",
  mainInput: "mainInput",
  requestEditPage: "editPageJSON",
  calculatorCompute: "compute",
  database: {
    operation: "databaseOperation",
    fetch: "databaseFetch",
    labels: "databaseLabels",
  },
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
  userRole: "userRole",
}

class Addresses {
  constructor() { }
  /** @returns{string} */
  solveJSON(
    /**@type{string} */
    problem,
    /**@type{boolean} */
    debug,
  ) {
    let result = `${urls.calculatorAPI}?`;
    result += `${urlFields.request}=${urlFields.requests.solveJSON}&`;
    result += `${urlFields.requests.calculatorInput}=${encodeURIComponent(problem)}&`;
    if (debug) {
      result += `${urlFields.debugFlag}=true&`;
    }
    return result;
  }
}

let addresses = new Addresses();

module.exports = {
  urls,
  urlFields,
  addresses,
};