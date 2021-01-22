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
    let answersElements = this.element.querySelectorAll("answer");
    for (let i = 0; i < answersElements.length; i++) {
      let answer = new HardCodedAnswer(
        answersElements[i],
        answersElements[i].textContent,
        this.calculatorServer,
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
    calculatorServer,
  ) {
    /**@type{HTMLElement} */
    this.element = element;
    /**@type{string} */
    this.calculatorServer = calculatorServer;
    /**@type{boolean} Whether the problem should have an answer/solution button. */
    this.forReal = false;
    let forRealString = this.element.getAttribute("forReal");
    if (forRealString === "true") {
      this.forReal = true;
    }
    /**@type{string} */
    this.buttonOptions = this.element.getAttribute("buttons");
    /**@type{string} */
    this.desiredAnswer = desiredAnswer;
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
  }

  writeToElement() {
    this.element.textContent = "";
    this.element.style.display = "";
    // non-null non-undefined means create answer button
    let idButtonAnswer = "";
    if (this.forReal) {
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
      resultHTML += `<br>Your answer: \\(${givenData}\\)`;
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