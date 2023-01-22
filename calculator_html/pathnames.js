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
    debugLogin: "debugLogin",
    useFallbackDatabase: "useFallbackDatabase",
    httpsSupport: "httpsSupport",
    indicator: "indicator",
    pause: "pause",
    unpause: "unpause",
    answerId: "answerId",
    answerValue: "answerValue",
    answerCheck: "answerCheck",
    slidesFromSource: "slidesFromSource",
    homeworkFromSource: "homeworkFromSource",
    sourceSlides: "slidesSource",
    sourceHomework: "homeworkSource",
    clonePage: "clonePageJSON",
    modifyPage: "modifyPage",
    setProblemWeight: "setProblemWeight",
    setProblemDeadline: "setProblemDeadline",
    setTeacher: "setTeacher",
    teachers: "teachers",
    sections: "sections",
    database: "database",
    serverStatusJSON: "serverStatusJSON",
    workerId: "workerId",
    monitoring: "monitoring",
    calculatorInput: "calculatorInput",
    fileContent: "fileContent",
    calculatorExamplesJSON: "calculatorExamplesJSON",
    solveJSON: "solveJSON",
    compareExpressions: "compareExpressions",
    submitAnswerHardcoded: "submitAnswerHardcoded",
    expressionGiven: "compareExpressionsGiven",
    expressionDesired: "compareExpressionsDesired",
    calculatorCompute: "compute",
    mainInput: "mainInput",
  },
  result: {
    result: "result",
    resultHtml: "resultHtml",
    outputString: "outputString",
    outputSliders: "sliders",
    crashReport: "crashReport",
    error: "error",
    syntaxErrors: "syntaxErrors",
    syntaxErrorsExtra: "syntaxErrorsExtra",
    // Comments on performance.
    performance: "performance",
    // Computation time. Excludes https traffic.
    computationTime: "computationTime",
    // Comments for the specific task.
    comments: "comments",
    // Global comments, usually used for debugging.
    commentsGlobal: "commentsGlobal",
    canvasName: "canvasName",
    controlsName: "controlsName",
    messagesName: "messagesName",
    solution: {
      steps: "solutionSteps",
      transformation: "transformation",
      stepType: "stepType",
      annotations: "annotations",
    },
    comparison: {
      errorEvaluation: "errorEvaluation",
      errorInAnswer: "errorInAnswer",
      areEqual: "areEqual",
      areEqualAsAnswers: "areEqualAsAnswers",
    },
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
    answerWasHardCoded: "answerWasHardCoded",
    forReal: "forReal",
  },
  teachersAndSections: "teachersAndSections",
  requestEditPage: "editPageJSON",
  calculatorCompute: "compute",
  mainInput: "mainInput",
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

class StandardResponses {
  constructor() { }

  /**
   * @returns{boolean} Whether the response indicates that the user is not logged in.
   * Please note that if this returns false, the user may still be not logged in, if for example,
   * the request was one that did not require login.
   * @param responseObjectParsed the js object returned from the calculator, parsed.
   */
  isNotLoggedInResponse(responseObjectParsed) {
    return responseObjectParsed["status"] === "not logged in";
  }
  
  /**
   * @returns{boolean} Whether the response indicates that the user logged in.
   * Please note that if this returns false, the user may still be logged in - perhaps the backend 
   * did not attach the information that the user is logged in to the response.
   * @param responseObjectParsed the js object returned from the calculator, parsed.
   */
  isLoggedInResponse(responseObjectParsed) {
    return responseObjectParsed["status"] === "logged in";
  }
}

let standardResponses = new StandardResponses();

class Addresses {
  constructor() { }

  /** @returns{string} */
  calculatorComputation(
    /** @type{string} */
    input,
  ) {
    let url = {
      "currentPage": "calculator",
      "calculatorInput": input,
    };
    let encoded = encodeURIComponent(JSON.stringify(url));
    return `${urls.appWithCache}#${encoded}`;
  }

  /** @returns{string} */
  solveJSON(
    /** @type{string} */
    problem,
    /** @type{boolean} */
    debug,
  ) {
    let result = `${urls.calculatorAPI}?`;
    result += `${urlFields.request}=${urlFields.requests.solveJSON}&`;
    let encodedProblem = encodeURIComponent(problem);
    result += `${urlFields.requests.calculatorInput}=${encodedProblem}&`;
    if (debug) {
      result += `${urlFields.debugFlag}=true&`;
    }
    return result;
  }

  compareExpressions(
    /** @type{string} */
    given,
    /** @type{string} */
    desired,
    /** @type{boolean} */
    debug,
  ) {
    let result = `${urls.calculatorAPI}?`;
    result += `${urlFields.request}=${urlFields.requests.compareExpressions}&`;
    let encodedGiven = encodeURIComponent(given);
    let encodedDesired = encodeURIComponent(desired);
    result += `${urlFields.requests.expressionGiven}=${encodedGiven}&`;
    result += `${urlFields.requests.expressionDesired}=${encodedDesired}&`;
    if (debug) {
      result += `${urlFields.debugFlag}=true&`;
    }
    return result;
  }

  submitAnswerHardcoded(
    /** @type{string} */
    answerId,
    /** @type{string} */
    answerGiven,
    /** @type{string} */
    answerCheck,
    /** @type{boolean} */
    debug,
  ) {
    let result = `${urls.calculatorAPI}?`;
    result += `${urlFields.request}=${urlFields.requests.submitAnswerHardcoded}&`;
    let encodedAnswerId = encodeURIComponent(answerId);
    let encodedGiven = encodeURIComponent(answerGiven);
    let encodedCheck = encodeURIComponent(answerCheck);
    result += `${urlFields.requests.answerId}=${encodedAnswerId}&`;
    result += `${urlFields.requests.answerValue}=${encodedGiven}&`;
    result += `${urlFields.requests.answerCheck}=${encodedCheck}&`;
    if (debug) {
      result += `${urlFields.debugFlag}=true&`;
    } else {
      result += `${urlFields.debugFlag}=false&`;
    }
    return result;
  }
}

let addresses = new Addresses();

module.exports = {
  urls,
  urlFields,
  addresses,
  standardResponses,
};