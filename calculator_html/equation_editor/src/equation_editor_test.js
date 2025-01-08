const allLatexSnippets = [
  `\\frac{1}{2}`,
  `f(x)=`,
  `\\frac{2}{x+\\frac{1}{x}}`,
  `x=2y\\cdot y \\cdot y^2+zy+3`,
  `\\begin{array}{rcll|l} \\displaystyle \\lim\\limits_{x \\to -1}\\frac{x^{2}+4 x+3}{x^{2}+6 x+5}&=&
   \\displaystyle
   \\lim\\limits_{x \\to -1} \\frac{(x+1)(x+3)}{(x+1)(x+5)} && {\\begin{array}{l}\\text{Factor:~Vieta's~formulas~help:~
   if:}\\\\ ax^{2}+bx+c=a(x-x_{1})(x-x_{2})\\\\ \\text{then:}\\\\ {\\begin{array}{rcl} \\displaystyle x_{1}+x_{2}&=&
   \\displaystyle - \\frac{b}{a}= \\\\ \\displaystyle x_{1} x_{2} &=& \\displaystyle \\frac{c}{a} \\end{array}}
   \\end{array}}\\\\ &=&\\displaystyle \\lim\\limits_{x \\to -1} \\frac{(\\cancel{x+1})(x+3)}{(\\cancel{x+1})(x+5)}
   &&\\text{Allowed~to~substitute~}x=-1\\\\ &=&\\displaystyle \\frac{-1+3}{-1+5}\\\\ &=&\\displaystyle \\frac{1}{2}
   \\end{array}`
];

const bodyContainer = document.createElement("div");
document.body.appendChild(bodyContainer);


/** @return {HTMLElement} */
function renderLatex(
  /** @type {string} */ input,
  /** @type {boolean} */ editable,
  /** @type {boolean} */ useSVG,
  /** @type {boolean} */ useMathML,
) {
  const container = document.createElement("div");
  bodyContainer.textContent = "";
  bodyContainer.appendChild(container);
  let equationEditor = new EquationEditor(container, new EquationEditorOptions({
    editable: editable,
    useMathML: useMathML,
    useSVG: useSVG,
  }));
  equationEditor.writeLatex(input);
  return container;
}

describe("Equation editor", () => {
  it("Equation editor editable renders", async () => {
    for (const latex of allLatexSnippets) {
      const container = renderLatex(latex, true, false, false);
      const thirdChild = container.childNodes[2];
      expect(thirdChild.getAttribute("mathtagname")).toBe("root");
      expect(container.getAttribute("latexsource").length > 0).toBe(true)
    }
  });

  it("Equation editor renders static DOM", async () => {
    for (const latex of allLatexSnippets) {
      const container = renderLatex(latex, false, false, false);
      const secondChild = container.childNodes[1];
      expect(secondChild.getAttribute("mathtagname")).toBe("root");
      expect(container.getAttribute("latexsource").length > 0).toBe(true)
    }
  });

  it("Equation editor renders with SVG", async () => {
    for (const latex of allLatexSnippets) {
      const container = renderLatex(latex, false, true, false);
      const secondChild = container.childNodes[1];
      expect(secondChild.getAttribute("mathtagname")).toBe("root");
      expect(container.getAttribute("latexsource").length > 0).toBe(true)
    }
  });

  it("Equation editor renders with mathML", async () => {
    for (const latex of allLatexSnippets) {
      const container = renderLatex(latex, false, false, true);
      const secondChild = container.childNodes[1];
      expect(secondChild.getAttribute("mathtagname")).toBe("root");
      expect(container.getAttribute("latexsource").length > 0).toBe(true)
    }
  });
});