"use strict";
let pathnames = require("./pathnames");


class AnswerProcessing {
  constructor() {
  }

  getFirstValueFromKeys(
    input,
    /**@type{string[]} */
    keys,
  ) {
    for (let i = 0; i < keys.length; i++) {
      let candidate = input[keys[i]];
      if (candidate !== undefined && candidate !== null && candidate !== "") {
        return candidate;
      }
    }
    return "";
  }

  getAllValuesOfGivenKeys(
    input,
    /**@type{string[]} */
    labels,
  ) {
    let result = [];
    for (let i = 0; i < labels.length; i++) {
      let candidate = input[labels[i]];
      if (candidate !== undefined && candidate !== null && candidate !== "") {
        result.push(candidate);
      }
    }
    return result.join("<br>");
  }

  htmlUserFriendlyResult(input) {
    let resultHTML = this.htmlUserFriendlyResultComparisonErrorsOnly(input);
    if (resultHTML !== "") {
      return resultHTML;
    }
    resultHTML += this.getAllValuesOfGivenKeys(input, [pathnames.urlFields.result.resultHtml]);
    let commentsAndCrashes = [
      pathnames.urlFields.result.comments,
      pathnames.urlFields.result.commentsGlobal,
      pathnames.urlFields.result.crashReport,
    ];
    resultHTML += this.getAllValuesOfGivenKeys(input, commentsAndCrashes);
    return resultHTML;
  }

  htmlUserFriendlyResultComparisonErrorsOnly(input) {
    let syntaxErrors = this.getFirstValueFromKeys(input, [
      pathnames.urlFields.result.syntaxErrors,
      pathnames.urlFields.result.syntaxErrorsExtra
    ]);
    if (syntaxErrors !== "") {
      return `<b style='color:red'>?</b><br>${syntaxErrors}`;
    }
    let errorTags = [
      pathnames.urlFields.result.error,
      pathnames.urlFields.result.comparison.errorEvaluation,
      pathnames.urlFields.result.comparison.errorInAnswer,
    ];
    let errorsEvaluation = this.getAllValuesOfGivenKeys(input, errorTags);
    if (errorsEvaluation !== "") {
      return `<b style='color:red'>?</b><br>${errorsEvaluation}`;
    }
    return "";
  }
}

let answerProcessing = new AnswerProcessing();

module.exports = {
  answerProcessing,
}