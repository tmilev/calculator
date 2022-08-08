"use strict";
const ids = require("./ids_dom_elements");
const cookies = require("./cookies");
const configuration = require("./configuration");

class StorageVariable {
  constructor(
    /**@type @{{name: string, nameURL: string, nameCookie: string, nameLocalStorage: string, associatedDOMId: string, type: string, secure: string, showInURLByDefault: bool, showInURLOnPages: Object, callbackOnValueChange: function}} */
    inputs
  ) {
    this.value = "";
    this.name = inputs.name;
    this.nameURL = "";
    this.nameCookie = "";
    this.nameLocalStorage = "";
    this.associatedDOMId = "";
    this.type = "string";
    this.secure = true;
    this.showInURLByDefault = false;
    /**@type {Function|null} */
    this.callbackOnValueChange = null;
    /**@type {Function|null} */
    this.callbackSetValueFromStorage = null;
    let labelsToRead = [
      "nameURL",
      "nameCookie",
      "nameLocalStorage",
      "associatedDOMId",
      "type",
      "secure",
      "callbackOnValueChange",
      "showInURLByDefault",
      "showInURLOnPages",
    ];
    for (let counterLabel = 0; counterLabel < labelsToRead.length; counterLabel++) {
      let currentLabel = labelsToRead[counterLabel];
      let incoming = inputs[currentLabel];
      if (incoming !== "" && incoming !== null && incoming !== undefined) {
        this[currentLabel] = incoming;
      }
    }
  }

  /**@returns {boolean}  */
  isTrue() {
    if (this.value === "true" || this.value === true) {
      return true;
    }
    return false;
  }

  getValue() {
    return this.value;
  }

  /**@returns{string} */
  loadMe(hashParsed) {
    let candidate = "";
    if (Storage !== undefined || localStorage !== undefined && this.nameLocalStorage !== "") {
      let incoming = localStorage.getItem(this.nameLocalStorage);
      if (incoming !== "" && incoming !== null && incoming !== undefined) {
        candidate = incoming;
      }
    }
    if (this.nameCookie !== "") {
      let incoming = cookies.getCookie(this.nameCookie);
      if (incoming !== "" && incoming !== null && incoming !== undefined) {
        candidate = incoming;
      }
    }
    if (hashParsed !== null && hashParsed !== undefined) {
      if (this.nameURL !== "") {
        if (this.nameURL in hashParsed) {
          let incoming = hashParsed[this.nameURL];
          if (incoming !== null && incoming !== undefined) {
            candidate = incoming;
          }
        }
      }
    }
    this.setAndStore(candidate, false, true);
    return candidate;
  }

  storeMePersistent(
    /**@type {boolean} */
    updateURL,
  ) {
    if (Storage !== undefined || localStorage !== undefined) {
      if (this.nameLocalStorage !== "" && this.nameLocalStorage !== null && this.nameLocalStorage !== undefined) {
        localStorage[this.nameLocalStorage] = this.value;
      }
    }
    if (this.nameCookie !== "") {
      cookies.setCookie(this.nameCookie, this.value, 150, this.secure);
    }
    if (updateURL !== false) {
      storage.setURL();
    }
  }

  storeMe(
    /**@type {boolean} */
    updateURL,
    /**@type {boolean} */
    updateAssociatedInput,
  ) {
    this.storeMePersistent(updateURL);
    if (updateAssociatedInput !== true) {
      return;
    }
    if (this.associatedDOMId !== null && this.associatedDOMId !== undefined && this.associatedDOMId !== "") {
      let element = document.getElementById(this.associatedDOMId);
      if (element !== null) {
        element.value = this.value;
      }
    }
    if (this.callbackSetValueFromStorage !== null) {
      this.callbackSetValueFromStorage(this.value);
    }
  }

  setAndStore(
    newValue,
    /**@type {boolean} */
    updateURL,
    /**@type {boolean} */
    updateAssociatedInput,
  ) {
    if (updateURL === undefined) {
      updateURL = true;
    }
    let changed = (this.value !== newValue);
    this.value = newValue;
    if (!configuration.configuration.calculatorDefaultsEnabled) {
      return;
    }
    this.storeMe(updateURL, updateAssociatedInput);
    if (!changed) {
      return;
    }
    if (this.callbackOnValueChange === null || this.callbackOnValueChange === undefined) {
      return;
    }
    // Calling function with timeout ensures the current function sequence is finished first.
    setTimeout(() => {
      this.callbackOnValueChange(this.value);
    }, 0);
  }
}

class StorageCalculator {
  constructor() {
    let pageNamesOnWhichToShowProblemURLs = {
      selectCourse: true,
      currentCourse: true,
      problemPage: true,
      editPage: true,
    };
    this.variables = {
      currentPage: new StorageVariable({
        name: "currentPage",
        nameLocalStorage: "currentPage", //<- when given and non-empty, local storage will be used to store variable
        nameCookie: "", //<- when given and non-empty, cookies will be used to store variable
        nameURL: "currentPage", //<- when given and non-empty, url will be used to store variable
        showInURLByDefault: true, // <- when given and true, url will be added to the window hash
      }),
      database: {
        labels: new StorageVariable({
          name: "databaseLabels",
          nameLocalStorage: "databaseLabels"
        }),
      },
      editor: {
        currentlyEditedPage: new StorageVariable({
          name: "currentlyEditedPage",
          nameLocalStorage: "currentlyEditedPage"
        }),
      },
      solve: {
        problemToAutoSolve: new StorageVariable({
          name: "problemToAutoSolve",
          nameURL: "problemToAutoSolve",
          nameLocalStorage: "problemToAutoSolve",
        }),
        panel: {
          forceShowAll: new StorageVariable({
            name: "solvePanelForceShowAll",
            nameLocalStorage: "solvePanelForceShowAll",
          }),
          forceShowNone: new StorageVariable({
            name: "solvePanelForceShowNone",
            nameLocalStorage: "solvePanelForceShowNone",
          }),
        },
      },
      solveSocratic: {
        problemToAutoSolve: new StorageVariable({
          name: "problemToAutoSolveSocratic",
          nameURL: "problemToAutoSolveSocratic",
          nameLocalStorage: "problemToAutoSolveSocratic",
          showInURLOnPages: {
            "solveSocratic": true,
          },
        }),
        panel: {
          forceShowAll: new StorageVariable({
            name: "solvePanelForceShowAll",
            nameLocalStorage: "solvePanelForceShowAll",
          }),
          forceShowNone: new StorageVariable({
            name: "solvePanelForceShowNone",
            nameLocalStorage: "solvePanelForceShowNone",
          }),
        },
        backend: new StorageVariable({
          name: "socracticBackend",
          nameLocalStorage: "socracticBackend",
        }),
      },
      compare: {
        given: new StorageVariable({
          name: "given",
          nameURL: "compareGivenExpression",
          nameLocalStorage: "compareGivenExpression",
          showInURLOnPages: { "compareExpressions": true },
        }),
        desired: new StorageVariable({
          name: "desired",
          nameURL: "compareDesiredExpression",
          nameLocalStorage: "compareDesiredExpression",
          showInURLOnPages: { "compareExpressions": true },
        }),
      },
      compareSocratic: {
        given: new StorageVariable({
          name: "givenSocratic",
          nameURL: "compareGivenExpressionSocratic",
          nameLocalStorage: "compareGivenExpressionSocratic",
          showInURLOnPages: { "compareExpressionsSocratic": true },
        }),
        desired: new StorageVariable({
          name: "desiredSocratic",
          nameURL: "compareGivenExpressionSocratic",
          nameLocalStorage: "compareDesiredExpressionSocratic",
          showInURLOnPages: { "compareExpressionsSocratic": true },
        }),
      },
      currentSectionComputed: new StorageVariable({
        name: "currentSectionComputed",
        nameLocalStorage: "currentSectionComputed"
      }),
      currentCourse: {
        courseHome: new StorageVariable({
          name: "courseHome",
          nameCookie: "courseHome",
          nameURL: "courseHome",
          showInURLOnPages: pageNamesOnWhichToShowProblemURLs,
        }),
        topicList: new StorageVariable({
          name: "topicList",
          nameCookie: "topicList",
          nameURL: "topicList",
          showInURLOnPages: pageNamesOnWhichToShowProblemURLs,
        }),
        fileName: new StorageVariable({
          name: "fileName",
          nameCookie: "fileName",
          nameURL: "fileName",
          problemPage: pageNamesOnWhichToShowProblemURLs,
        }),
        problemFileName: new StorageVariable({
          name: "problemFileName",
          nameCookie: "problemFileName",
          nameURL: "problemFileName",
          showInURLOnPages: pageNamesOnWhichToShowProblemURLs,
        }),
        exerciseType: new StorageVariable({
          name: "exerciseType",
          nameLocalStorage: "exerciseType",
          nameURL: "exerciseType",
          showInURLOnPages: {
            problemPage: true,
          },
        }),
        randomSeed: new StorageVariable({
          name: "randomSeed",
          nameURL: "randomSeed",
          // Since this variable has no local storage outside of the URL,
          // to avoid wiping the variable while normalizing the URL,
          // we need to keep it displayed there by default.
          showInURLOnPages: pageNamesOnWhichToShowProblemURLs,
        }),
      },
      flagDebug: new StorageVariable({
        name: "debugFlag",
        nameURL: "debugFlag",
        nameCookie: "debugFlag",
        secure: true,
      }),
      flagStudentView: new StorageVariable({
        name: "studentView",
        nameURL: "studentView",
        nameCookie: "studentView",
        secure: true,
      }),
      calculator: {
        input: new StorageVariable({
          name: "calculatorInput",
          nameURL: "calculatorInput",
          associatedDOMId: ids.domElements.pages.calculator.inputMain,
          showInURLOnPages: {
            calculator: true,
          },
        }),
        request: new StorageVariable({
          name: "calculatorRequest",
          nameURL: "calculatorRequest",
          nameLocalStorage: "calculatorRequest",
        }),
        monitoring: new StorageVariable({
          name: "monitoring",
          nameLocalStorage: "monitoring",
          nameURL: "monitoring",
          nameCookie: "monitoring",
        }),
        useWebAssembly: new StorageVariable({
          name: "useWebAssembly",
          nameURL: "useWebAssembly",
          nameLocalStorage: "useWebAssembly",
        }),
      },
      user: {
        activationToken: new StorageVariable({
          name: "activationToken",
          nameURL: "activationToken",
        }),
        googleToken: new StorageVariable({
          name: "googleToken",
        }),
        name: new StorageVariable({
          name: "username",
          nameCookie: "username",
          nameURL: "username",
        }),
        authenticationToken: new StorageVariable({
          name: "authenticationToken",
          nameCookie: "authenticationToken",
          nameURL: "authenticationToken",
        }),
        role: new StorageVariable({
          name: "userRole",
          nameCookie: "userRole",
          nameURL: "userRole",
        }),
        email: new StorageVariable({
          name: "email",
          nameURL: "email",
        }),
      },
      theme: new StorageVariable({
        name: "theme",
        nameLocalStorage: "theme",
      }),
    };
    this.currentHashRaw = "";
    this.currenTHashDecoded = "";
    this.urlObject = {};
  }

  /**@returns {string} */
  getPercentEncodedURL(input) {
    return encodeURIComponent(JSON.stringify(input));
  }

  /**@returns {string} */
  getCleanedUpURL(input) {
    let stringifiedInput = JSON.stringify(input);
    let isGood = true;
    try {
      let decodedAsURL = decodeURIComponent(stringifiedInput);
      if (stringifiedInput !== decodedAsURL) {
        isGood = false;
      }
    } catch (e) {
      isGood = false;
    }
    if (!isGood) {
      stringifiedInput = encodeURIComponent(stringifiedInput);
    }
    return stringifiedInput;
  }

  parseURL() {
    try {
      if (
        this.currentHashRaw === window.location.hash
      ) {
        return;
      }
      this.currentHashRaw = window.location.hash;
      this.currenTHashDecoded = decodeURIComponent(this.currentHashRaw);
      if (this.currenTHashDecoded.startsWith('#')) {
        this.currenTHashDecoded = this.currenTHashDecoded.slice(1);
      }
      if (this.currenTHashDecoded === "") {
        this.urlObject = {
        };
      } else {
        this.urlObject = JSON.parse(this.currenTHashDecoded);
      }
    } catch (e) {
      console.log(`Failed to parse your url hash ${this.currenTHashDecoded} obtained from ${window.location.hash}.${e}.`);
    }
  }

  loadSettings() {
    if (!configuration.configuration.calculatorDefaultsEnabled) {
      return;
    }
    this.parseURL();
    this.loadSettingsRecursively(this.variables, this.urlObject);
  }

  loadSettingsRecursively(
    /**@type {StorageVariable} */
    currentStorage,
    inputHashParsed,
  ) {
    if (currentStorage instanceof StorageVariable) {
      currentStorage.loadMe(inputHashParsed);
      return;
    }
    if (typeof currentStorage === "object") {
      for (let subLabel in currentStorage) {
        this.loadSettingsRecursively(currentStorage[subLabel], inputHashParsed);
      }
    }
  }

  setURL() {
    this.urlObject = this.computeURLRecursively(this.variables);
    let incomingHashRaw = this.getPercentEncodedURL(this.urlObject);

    if (incomingHashRaw !== this.currentHashRaw) {
      window.location.hash = incomingHashRaw;
      this.currentHashRaw = incomingHashRaw;
    }
  }

  computeURLRecursively(currentStorage, recursionDepth) {
    if (recursionDepth === undefined) {
      recursionDepth = 0;
    }
    if (recursionDepth > 100) {
      throw ("Recursion is too deeply nested. This must be a programming error. ");
    }
    let result = {
    };
    if (currentStorage instanceof StorageVariable) {
      let urlName = currentStorage.nameURL;
      if (urlName === undefined || urlName === null || urlName === "") {
        return null;
      }
      let shouldShow = false;
      if (currentStorage.showInURLByDefault === true) {
        shouldShow = true;
      }
      if (currentStorage.showInURLOnPages !== null && currentStorage.showInURLOnPages !== undefined) {
        if (storage.variables.currentPage.value in currentStorage.showInURLOnPages) {
          shouldShow = true;
        }
      }
      if (!shouldShow) {
        return null;
      }
      if (currentStorage.value === null || currentStorage.value == undefined || currentStorage.value == "") {
        return null;
      }
      result[urlName] = currentStorage.value;
      return result;
    }
    if (typeof currentStorage !== "object") {
      throw (`Unexpected currentStorage input while computing url: ${currentStorage}`);
    }
    for (let label in currentStorage) {
      let incoming = this.computeURLRecursively(currentStorage[label], recursionDepth + 1);
      if (incoming === null) {
        continue;
      }
      Object.assign(result, incoming);
    }
    return result;
  }

  /**@returns{string} */
  toStringProblem() {
    let problemFileName = this.variables.currentCourse.problemFileName.getValue();
    let fileName = this.variables.currentCourse.p;
    let result = `problemFileName: ${problemFileName}. fileName: ${fileName}`;
    return result;
  }
}

let storage = new StorageCalculator();

module.exports = {
  storage,
};