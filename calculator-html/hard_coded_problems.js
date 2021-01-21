const equationEditor = require("./equation_editor");
const AnswerPanel = require("./answer_panel").AnswerPanel;

class HardCodedProblems {
  constructor() {
  }
  create(
    /**@type{HTMLElement|string} */
    element,
    /**@type{string} */
    server,
  ) {
    new Problem(element, server).create();
  }
}

class Problem {
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
    this.server = server;
    /**@type{AnswerPanel[]} */
    this.answers = [];
  }

  create() {
    let answersElements = this.element.querySelectorAll("answer");
    for (let i = 0; i < answersElements.length; i++) {
      let answer = new HardCodedAnswer(answersElements[i], answersElements[i].textContent);
      this.answers.push(answer);
      answer.initialize();
    }
    equationEditor.typeset(this.element, "", false, false, false);
  }
}

class HardCodedAnswer {
  constructor(
    /**@type{HTMLElement} */
    element,
    /**@type{string} */
    content,
  ) {
    /**@type{HTMLElement} */
    this.element = element;
    /**@type{string} */
    this.content = content;
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

  createElements() {
    if (this.pureLatexElement === null) {
      this.pureLatexElement = document.createElement("input");
    }
    if (this.buttonSubmit === null) {
      this.buttonSubmit = document.createElement("button");
    }
    if (this.equationEditor === null) {
      this.equationEditor = document.createElement("span");
    }
    if (this.panelWithButtons === null) {
      this.panelWithButtons = document.createElement("div");
    }
  }

  initialize() {
    this.answerPanel = new AnswerPanel({
      pureLatexElement: this.pureLatexElement,
    });
  }
}

function create(
  /**@type{HTMLElement|string} */
  element,
  /**@type{string} */
  server,
) {
  return hardCodedProblems.create(element, server);
}

let hardCodedProblems = new HardCodedProblems();

module.exports = {
  create,
  hardCodedProblems,
};