"use strict";

let stringResources = {
  prefixDeadlineContainer: "deadlinesContainer"
};

let domElements = {
  calculatorDefaultsEnabled: "calculatorDefaultsEnabled",
  pages: {
    solve: {
      div: "divSolvePage",
      buttonSelectSolvePage: "buttonSelectSolvePage",
      buttonSolve: "buttonSolve",
      editor: "editorSolveProblem",
      editorButtonPanel: "editorSolveButtonPanel",
      editorSolveProblemDebug: "editorSolveProblemDebug",
      solutionBox: "solutionBox",
    },
    solveSocratic: {
      editor: "editorSolveProblemSocratc",
      editorButtonPanel: "editorSolveSocraticButtonPanel",
      input: "inputBoxSolveSocratic",
      div: "divSolveSocraticPage",
      anchorThisPage: "socraticLinkToThisPage",
      link: "socraticLink",
      editorSolveProblemDebug: "editorSolveProblemSocraticDebug",
      buttonMain: "buttonSolveSocratic",
      solution: "divSocraticSolution"
    },
    privacyPolicy: {
      div: "divPrivacyPolicy",
      button: "buttonPrivacyPolicy",
    },
    themes: {
      div: "divThemes",
      button: "buttonThemes",
    },
    editPage: {
      div: "divEditPage",
      button: "buttonEditPage",
    },
    calculator: {
      editorComments: "calculatorEditorComments",
      editorCommentsDebug: "calculatorEditorDebug",
      equationEditorFontSizeSlider: "equationEditorFontSizeSlider",
      inputMain: "mainInputID",
      monitoring: {
        progressTimer: "idProgressReportTimer",
        spanStatus: "spanMonitoringStatus",
        buttonPauseToggle: "buttonPauseToggle",
      },
      progress: "calculatorInputProgress",
    },
    compareExpressions: {
      div: "divCompareExpressionsPage",
      buttonCompare: "buttonCompareExpressions",
      givenEquationEditor: "divGivenEquationEditor",
      desiredEquationEditor: "divDesiredEquationEditor",
      givenPanel: "divGivenPanel",
      desiredPanel: "divDesiredPanel",
      givenRawInput: "inputGivenRawInput",
      desiredRawInput: "inputDesiredRawInput",
      resultUserFriendly: "compareExpressionsResultUserFriendly",
      resultRaw: "divCompareExpressionsResultRaw",
      resultFormatted: "divCompareExpressionsResultFormatted",
      calculatorLink: "divCompareExpressionsCalculatorLink",
      progress: "progressReportCompareExpressions",
    },
    compareExpressionsSocratic: {
      div: "divCompareExpressionsSocraticPage",

    },
    login: {
      divLoginCalculatorPanel: "divLoginCalculatorPanel",
      userNameReport: "divLoginPanelUsernameReport"
    },
  },
  applicationIdentifiers: {
    default: "divDefaultApplication",
    compareExpressions: "divCompareExpressions",
  },
  compareExpressions: {
    mainPage: "divCompareExpressionsMainPage",
  },
  themeRadioButtons: {
    radioThemeExperimental: "radioThemeExperimental",
    radioThemeDefault: "radioThemeDefault",
  },
  spanOldEmail: "spanOldEmail",
  buttonChangeEmail: "buttonChangeEmail",
  inputUsername: "inputUsername",
  inputEmail: "inputEmail",
  inputPassword: "inputPassword",
  labelNewEmail: "labelNewEmail",
  switch: {
    monitoring: "sliderMonitoring",
  },
  calculatorExamples: "divCalculatorExamples",
  spanCalculatorMainOutput: "spanCalculatorMainOutput",
  spanProgressReportGeneral: "spanProgressReportGeneral",
  spanUserIdInActivateAccountPage: "spanUserIdInActivateAccountPage",
  spanUserIdInAccountsPage: "spanUserIdInAccountsPage",
  spanCurrentActivationEmail: "spanCurrentActivationEmail",
  spanCurrentActivationToken: "spanCurrentActivationToken",
  spanVerificationActivation: "spanVerificationActivation",
  spanProblemLinkWithRandomSeed: "spanProblemLinkWithRandomSeed",
  inputNewPasswordInActivationAccount: "inputNewPasswordInActivationAccount",
  inputReenteredPasswordInActivationAccount: "inputReenteredPasswordInActivationAccount",
  sliderMathJaxFlag: "mathJaxFlag",
  sliderDebugFlag: "sliderDebugFlag",
  sliderStudentView: "sliderStudentView",
  spanErrorsEditPage: "spanErrorEditPage",
  spanDatabaseComments: "spanDatabaseComments",
  spanMathJaxFlag: "spanMathJaxFlag",
  spanDebugFlagToggleReport: "spanDebugFlagToggleReport",
  spanStudentViewFlagToggleReport: "spanStudentViewToggleReport",
  spanUserExtraInfo: "spanUserExtraInfo",
  spanStudentViewPanel: "spanStudentViewPanel",
  spanStudentViewSectionSelectPanel: "spanStudentViewSectionSelectPanel",
  divCalculatorMainInputOutput: "divCalculatorMainInputOutput",
  divPerformance: "divPerformance",
  divCurrentCourse: "divCurrentCourse",
  divCurrentCourseBody: "divCurrentCourseBody",
  divMathjaxProblematicRender: "divMathjaxProblematicRender",
  divLoginPanelExtraInfo: "divLoginPanelExtraInfo",
  divLoginPanelDatabaseInfo: "divLoginPanelDatabaseInfo",
  courseEditPanel: "divCurrentCourseEditPanel",
  classSpanDeadlineContainer: "spanDeadlineContainer",
  calculatorBuildVersion: "anchorCalculatorBuildVersion",
  divDatabaseOutput: "divDatabaseOutput",
  problemPageContentContainer: "divProblemPageContentContainer",
  divProblemInfoBar: "divProblemInfoBar",
  textAreaTopicListEntry: "textareaTopicListEntry",
  divEditorAce: "divEditorAce",
  buttonSaveEdit: "buttonSaveEdit",
  spanButtonPreviousEdit: "spanButtonPreviousEdit",
  spanButtonNextEdit: "spanButtonNextEdit",
  spanSubmitEditPageReport: "spanSubmitEditPageReport",
  serverStatus: "idServerStatus",
};

module.exports = {
  domElements,
  stringResources
};