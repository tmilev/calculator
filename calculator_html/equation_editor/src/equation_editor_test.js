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
  \\end{array}`,
  `{{x}_{1}}^{2}+{x}_{2}^{2}+x^2_3 + {x^2}_4+x^{2}_{5}+{{x}^{2}}_{6}+{{x}_{7}}^{2}=1`,
  `\\begin{array}{cc|ccccccccccccccc}
  \\mathrm{roots~simple~coords}&\\varepsilon-\\mathrm{root~notation}&[\\bullet, \\bullet]
  & g_{-6} & g_{-5} & g_{-4} & g_{-3} & g_{-2} & g_{-1} & h_{1} & h_{2} & g_{1} & g_{2} & g_{3} & g_{4} & g_{5}
  & g_{6}\\\\
  (-3, -2)&\\varepsilon_{1}+\\varepsilon_{2}-2\\varepsilon_{3}&g_{-6}& 0& 0& 0& 0& 0& 0& 0& g_{-6}& 0& g_{-5}&
  -g_{-4}& g_{-3}& -g_{-2}& -2h_{2}-3h_{1}\\\\
  (-3, -1)&-\\varepsilon_{1}+2\\varepsilon_{2}-\\varepsilon_{3}&g_{-5}& 0& 0& 0& 0& g_{-6}& 0& g_{-5}& -g_{-5}&
  g_{-4}& 0& 0& -g_{-1}& -h_{2}-3h_{1}& -g_{2}\\\\
  (-2, -1)&\\varepsilon_{2}-\\varepsilon_{3}&g_{-4}& 0& 0& 0& -3g_{-6}& 0& 3g_{-5}& 1/3g_{-4}& 0& 2g_{-3}& 0&
  -2g_{-1}& -3h_{2}-6h_{1}& -g_{1}& g_{3}\\\\
  (-1, -1)&\\varepsilon_{1}-\\varepsilon_{3}&g_{-3}& 0& 0& 3g_{-6}& 0& 0& 2g_{-4}& -1/3g_{-3}& g_{-3}& 3g_{-2}&
  -g_{-1}& -3h_{2}-3h_{1}& -2g_{1}& 0& -g_{4}\\\\
  (0, -1)&2\\varepsilon_{1}-\\varepsilon_{2}-\\varepsilon_{3}&g_{-2}& 0& -g_{-6}& 0& 0& 0& g_{-3}& -g_{-2}&
  2g_{-2}& 0& -h_{2}& -g_{1}& 0& 0& g_{5}\\\\
  (-1, 0)&-\\varepsilon_{1}+\\varepsilon_{2}&g_{-1}& 0& 0& -3g_{-5}& -2g_{-4}& -g_{-3}& 0& 2/3g_{-1}& -g_{-1}&
  -3h_{1}& 0& 3g_{2}& 2g_{3}& g_{4}& 0\\\\
  (0, 0)&0&h_{1}& 0& -g_{-5}& -1/3g_{-4}& 1/3g_{-3}& g_{-2}& -2/3g_{-1}& 0& 0& 2/3g_{1}& -g_{2}& -1/3g_{3}&
  1/3g_{4}& g_{5}& 0\\\\
  (0, 0)&0&h_{2}& -g_{-6}& g_{-5}& 0& -g_{-3}& -2g_{-2}& g_{-1}& 0& 0& -g_{1}& 2g_{2}& g_{3}& 0& -g_{5}& g_{6}\\\\
  (1, 0)&\\varepsilon_{1}-\\varepsilon_{2}&g_{1}& 0& -g_{-4}& -2g_{-3}& -3g_{-2}& 0& 3h_{1}& -2/3g_{1}& g_{1}& 0&
  g_{3}& 2g_{4}& 3g_{5}& 0& 0\\\\
  (0, 1)&-2\\varepsilon_{1}+\\varepsilon_{2}+\\varepsilon_{3}&g_{2}& -g_{-5}& 0& 0& g_{-1}& h_{2}& 0& g_{2}&
  -2g_{2}& -g_{3}& 0& 0& 0& g_{6}& 0\\\\
  (1, 1)&-\\varepsilon_{1}+\\varepsilon_{3}&g_{3}& g_{-4}& 0& 2g_{-1}& 3h_{2}+3h_{1}& g_{1}& -3g_{2}& 1/3g_{3}&
  -g_{3}& -2g_{4}& 0& 0& -3g_{6}& 0& 0\\\\
  (2, 1)&-\\varepsilon_{2}+\\varepsilon_{3}&g_{4}& -g_{-3}& g_{-1}& 3h_{2}+6h_{1}& 2g_{1}& 0& -2g_{3}& -1/3g_{4}&
  0& -3g_{5}& 0& 3g_{6}& 0& 0& 0\\\\
  (3, 1)&\\varepsilon_{1}-2\\varepsilon_{2}+\\varepsilon_{3}&g_{5}& g_{-2}& h_{2}+3h_{1}& g_{1}& 0& 0& -g_{4}&
  -g_{5}& g_{5}& 0& -g_{6}& 0& 0& 0& 0\\\\
  (3, 2)&-\\varepsilon_{1}-\\varepsilon_{2}+2\\varepsilon_{3}&g_{6}& 2h_{2}+3h_{1}& g_{2}& -g_{3}& g_{4}& -g_{5}&
  0& 0& -g_{6}& 0& 0& 0& 0& 0& 0\\\\
  \\end{array}`,
  `\\frac{ \\cancel{ x } } { \\cancel{ (d{ \\color{ pink } { d45 ^ { d } } }y) } \\cdot 5 }`,
  // Fixed on 2025/09/13.
  `\\left| 2+3\right.`,
  // Fixed on 2025/09/13.
  `2\ or \ 3`,
  // Make sure we don't die on large input!
  `123456789`.repeat(10_000),
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