const equationEditor = require("./equation_editor");
const AnswerPanel = require("./answer_panel").AnswerPanel;
const initializeButtons = require("./initialize_buttons");
const submit = require("./submit_requests");
const miscellaneous = require("./miscellaneous_frontend");
const answerProcessing = require("./answer_processing");
const pathnames = require("./pathnames");

class HardCodedProblemCollection {
  constructor() {
    /**@type{HardCodedProblem} */
    this.problems = [];
  }
  create(
    /**@type{HTMLElement[]|string[]} */
    elements,
    /**@type{string} */
    server,
  ) {
    if (!Array.isArray(elements)) {
      elements = [elements];
    }
    for (let i = 0; i < elements.length; i++) {
      let incoming = new HardCodedProblem(elements[i], server);
      this.problems.push(incoming);
      incoming.create();
    }
    initializeButtons.initializeAccordionButtons();
  }
}

class HardCodedProblem {
  constructor(
    /**@type{HTMLElement|string} */
    element,
    /**@type{string} */
    server,
  ) {
    if (typeof element === "string") {
      element = document.getElementById(element);
    }
    /**@type{HTMLElement} */
    this.element = element;
    this.calculatorServer = server;
    /**@type{AnswerPanel[]} */
    this.answers = [];
  }

  create() {
    this.createAnswersHardcoded();
    this.createAnswersCalculator();
  }

  createAnswersHardcoded() {
    let answersElements = this.element.querySelectorAll("answer");
    this.createAnswers(answersElements, true);
  }

  createAnswersCalculator() {
    let answersElements = this.element.querySelectorAll("calculatorAnswer");
    this.createAnswers(answersElements, false);
  }

  createAnswers(
    /**@type{HTMLElement[]} */
    answersElements,
    /**@type{boolean} */
    hardCoded,
  ) {
    for (let i = 0; i < answersElements.length; i++) {
      let element = answersElements[i];
      let answerString = "";
      let answerCheck = "";
      if (hardCoded) {
        answerString = element.textContent;
      } else {
        answerString = element.getAttribute("answer");
        if (answerString === null || answerString === undefined) {
          answerString = "";
        }
        answerCheck = element.textContent;
      }
      let answer = new HardCodedAnswer(
        element,
        answerString,
        answerCheck,
        this.calculatorServer,
        hardCoded,
      );
      this.answers.push(answer);
      answer.writeToElement();
    }
    equationEditor.typeset(this.element, "", false, false, false);
  }
}

class HardCodedAnswer {
  constructor(
    /**@type{HTMLElement} */
    element,
    /**@type{string} */
    desiredAnswer,
    /**@type{string} */
    answerCheck,
    /**@type{string} */
    calculatorServer,
    /**@type{boolean} */
    hardCoded,
  ) {
    /**@type{HTMLElement} */
    this.element = element;
    /**@type{string|null} */
    this.answerId = this.element.id;
    /**@type{string} */
    this.calculatorServer = calculatorServer;
    /**@type{boolean} Whether the problem should have an answer/solution button. */
    this.forReal = false;
    let forRealString = this.element.getAttribute("forReal");
    if (forRealString === "true") {
      this.forReal = true;
    }
    /**@type{boolean} When true, printouts will contain additional information. */
    this.debug = false;
    if (this.element.getAttribute("debug") === "true") {
      this.debug = true;
    }
    /**@type{string} */
    this.buttonOptions = this.element.getAttribute("buttons");
    /**@type{string} */
    this.desiredAnswer = desiredAnswer;
    /**@type{string} */
    this.answerCheck = answerCheck;
    /**@type{AnswerPanel|null} */
    this.answerPanel = null;
    /**@type{HTMLElement|null} */
    this.pureLatexElement = null;
    /**@type{HTMLElement|null} */
    this.buttonSubmitHardCoded = null;
    /**@type{HTMLElement|null} */
    this.equationEditor = null;
    /**@type{HTMLElement|null} */
    this.panelWithButtons = null;
    /**@type{HTMLElement|null} */
    this.verificationSpan = null;
    /**@type{HTMLElement|null} */
    this.solutionSpan = null;
    /**@type{HTMLElement|null} */
    this.hardCoded = hardCoded;
  }

  writeToElement() {
    this.element.textContent = "";
    this.element.style.display = "";
    // non-null non-undefined means create answer button
    let idButtonAnswer = "";
    if (this.forReal || this.desiredAnswer == "") {
      idButtonAnswer = null;
    }
    this.answerPanel = new AnswerPanel({
      forReal: this.forReal,
      generateInterpretButton: false,
      mathQuillPanelOptions: this.buttonOptions,
      dontBootstrapButtons: true,
      idButtonAnswer: idButtonAnswer,
      valueChangeHandler: () => {
        this.handleLatexChange();
      }
    });
    this.answerPanel.writeToElement(this.element);
    this.answerPanel.buttonSubmit.addEventListener("click", () => {
      this.submitAnswer();
    });
    if (!this.forReal) {
      this.answerPanel.buttonAnswer.addEventListener("click", () => {
        this.giveUp();
      });
    }
  }

  submitAnswer() {
    if (this.hardCoded) {
      this.submitAnswerHardCoded();
    } else {
      this.submitAnswerCalculator();
    }
  }

  submitAnswerCalculator() {
    if (this.id === "" || this.id === null) {
      this.verificationSpan.innerHTML = "<b style='color:red'>Non-hard-coded calculator answers need id property.</b>";
      return;
    }
    let givenData = this.answerPanel.panel.equationEditor.rootNode.toLatex();
    let urlRelative = pathnames.addresses.submitAnswerHardcoded(
      this.answerId, givenData, this.answerCheck, false,
    );
    let url = this.calculatorServer + urlRelative;
    submit.submitGET({
      url: url,
      callback: (input) => {
        this.writeResult(input, givenData);
      },
    });
  }

  submitAnswerHardCoded() {
    let givenData = this.answerPanel.panel.equationEditor.rootNode.toLatex();
    let urlRelative = pathnames.addresses.compareExpressions(
      givenData, this.desiredAnswer, false,
    );
    let url = this.calculatorServer + urlRelative;
    submit.submitGET({
      url: url,
      callback: (input) => {
        this.writeResult(input, givenData);
      },
    });
  }

  writeResult(
    input,
    /**@type{string} */
    givenData,
  ) {
    try {
      let result = miscellaneous.jsonUnescapeParse(input);
      let resultHTML = answerProcessing.answerProcessing.htmlUserFriendlyResult(result);
      if (this.hardCoded) {
        resultHTML += `<br>Your answer: \\(${givenData}\\)`;
      }
      this.answerPanel.verificationSpan.innerHTML = resultHTML;
      equationEditor.typeset(this.answerPanel.verificationSpan);
    } catch (e) {
      this.answerPanel.verificationSpan.innerHTML = `<b style='color:red'>${e}</b><br>${input}`;
    }
  }

  giveUp() {
    this.answerPanel.verificationSpan.innerHTML = `\\(${this.desiredAnswer}\\)`;
    equationEditor.typeset(this.answerPanel.verificationSpan, "", false, false, false);
  }

  handleLatexChange() {
    this.answerPanel.pureLatexElement.value = initializeButtons.processMathQuillLatex(
      this.answerPanel.panel.equationEditor.rootNode.toLatex(),
    );
  }
}

function create(
  /**@type{HTMLElement[]|string[]} */
  elements,
  /**@type{string} */
  server,
) {
  hardCodedProblems.create(elements, server);
}

let hardCodedProblems = new HardCodedProblemCollection();

module.exports = {
  create,
  hardCodedProblems,
};