class TestCase {
  constructor(
    /** @type {{
    content: string,
    skipSVG: boolean|undefined,
    skipEquationEditor: boolean|undefined,
    skipMathML: boolean|undefined,
    skipTest: boolean|undefined,
    skipStaticDOM: boolean|undefined
  }} */ input
  ) {
    this.content = input.content;
    this.skipSVG = false;
    if (input.skipSVG === true) {
      this.skipSVG = true;
    }
    this.skipEquationEditor = false;
    if (input.skipEquationEditor === true) {
      this.skipEquationEditor = true;
    }
    this.skipMathML = false;
    if (input.skipMathML === true) {
      this.skipMathML = true;
    }
    this.skipStaticDOM = false;
    if (input.skipStaticDOM === true) {
      this.skipStaticDOM = true;
    }
    this.skipTest = false;
    if (input.skipTest === true) {
      this.skipTest = true;
    }
  }
}

function constructTestCase(/** @type {string|{{
    content: string,
    dontTestSVG: boolean|undefined,
  }}} */ input) {
  if (typeof input === "string") {
    return new TestCase({
      content: input
    });
  }
  return new TestCase(input);
}

/** @type {Array.<{
   content: string,
   skipSVG: boolean|undefined,
   skipEquationEditor: boolean|undefined,
   skipMathML: boolean|undefined,
   skipTest: boolean|undefined,
   skipStaticDOM: boolean|undefined
 }|string>} */
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
  // 0x0 matrix shouldn't break us.
  `\\begin{pmatrix}\\end{pmatrix}`,
  // Support the \choose syntax.
  `n \\choose k`,
  '\\mathbf{x} + \\mathbf y + \\mathcal A + \\mathbb H',
  `\\underbrace{x}_y`,
  `\\overbrace{x}^y`,
  `\\angle\\in \\triangle ABC`,
  {// Large example. Came up in an actual scientific computation. 
    // Must not break the editor.
    content:
      `\\begin{array}{ll|l}&
ExponentOfAdjointOf{}\\left({A}_{2}, \\pi \\left({g}_{1}-{g}_{-1}\\right), {h}_{1}\\right)&Minus,~MultiplyRationals,~Minus,~DistributeMultiplication\\\\
=&
ExponentOfAdjointOf{}\\left({A}_{2}, \\pi {g}_{1}+\\pi \\left(-{g}_{-1}\\right), {h}_{1}\\right)&ConstantExtraction\\\\
=&
ExponentOfAdjointOf{}\\left({A}_{2}, \\pi {g}_{1}-\\pi {g}_{-1}, {h}_{1}\\right)&ExponentOfAdjointOf\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\left(\\begin{pmatrix}e^{\\pi\\cdot 0} & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & e^{\\pi\\cdot 0} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & e^{\\pi \\left(-\\sqrt{-1}\\right)} & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & e^{\\pi \\left(-\\sqrt{-1}\\right)} & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & e^{\\pi \\sqrt{-1}} & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & e^{\\pi \\sqrt{-1}} & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & e^{\\pi \\left(-2\\sqrt{-1}\\right)} & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & 0 & e^{\\pi\\cdot 2\\sqrt{-1}} \\end{pmatrix}\\right)\\begin{pmatrix}0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & -\\frac{1}{2} & 0 & 0 & \\frac{1}{2} & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} & \\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} & \\frac{1}{2}\\\\
-\\frac{\\sqrt{-1}}{2} & \\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{\\sqrt{-1}}{2} & \\frac{1}{2}\\\\
0 & 0 & \\frac{1}{4} & \\frac{\\sqrt{-1}}{2} & -\\frac{\\sqrt{-1}}{4} & \\frac{1}{4} & 0 & 0\\\\
0 & 0 & \\frac{1}{4} & -\\frac{\\sqrt{-1}}{2} & \\frac{\\sqrt{-1}}{4} & \\frac{1}{4} & 0 & 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&ConstantExtraction,~\\text{Previously~computed~that~} e^{\\pi\\cdot 0} = 1,~ConstantExtraction,~\\text{Previously~computed~that~} e^{\\pi \\left(-\\sqrt{-1}\\right)} = e^{-\\sqrt{-1} \\pi},~ConstantExtraction,~\\text{Previously~computed~that~} e^{\\pi \\sqrt{-1}} = e^{\\sqrt{-1} \\pi},~ConstantExtraction,~ConstantExtraction\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\left(\\begin{pmatrix}e^{0} & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & e^{-\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & e^{-\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & e^{\\sqrt{-1} \\pi} & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & e^{\\sqrt{-1} \\pi} & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & e^{-2\\sqrt{-1} \\pi} & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & 0 & e^{2\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & -\\frac{1}{2} & 0 & 0 & \\frac{1}{2} & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} & \\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} & \\frac{1}{2}\\\\
-\\frac{\\sqrt{-1}}{2} & \\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{\\sqrt{-1}}{2} & \\frac{1}{2}\\\\
0 & 0 & \\frac{1}{4} & \\frac{\\sqrt{-1}}{2} & -\\frac{\\sqrt{-1}}{4} & \\frac{1}{4} & 0 & 0\\\\
0 & 0 & \\frac{1}{4} & -\\frac{\\sqrt{-1}}{2} & \\frac{\\sqrt{-1}}{4} & \\frac{1}{4} & 0 & 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&PowerAnyToZero\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\left(\\begin{pmatrix}1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & e^{-\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & e^{-\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & e^{\\sqrt{-1} \\pi} & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & e^{\\sqrt{-1} \\pi} & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & e^{-2\\sqrt{-1} \\pi} & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & 0 & e^{2\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & -\\frac{1}{2} & 0 & 0 & \\frac{1}{2} & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} & \\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} & \\frac{1}{2}\\\\
-\\frac{\\sqrt{-1}}{2} & \\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{\\sqrt{-1}}{2} & \\frac{1}{2}\\\\
0 & 0 & \\frac{1}{4} & \\frac{\\sqrt{-1}}{2} & -\\frac{\\sqrt{-1}}{4} & \\frac{1}{4} & 0 & 0\\\\
0 & 0 & \\frac{1}{4} & -\\frac{\\sqrt{-1}}{2} & \\frac{\\sqrt{-1}}{4} & \\frac{1}{4} & 0 & 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&MultiplyMatrixByMatrix\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\begin{pmatrix}0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0\\cdot 0 & 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0\\cdot 0+0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+\\left(-\\frac{1}{2}\\right)+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi} \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{-\\sqrt{-1} \\pi}\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi} \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+e^{\\sqrt{-1} \\pi} \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+e^{\\sqrt{-1} \\pi} \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0\\\\
0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+e^{2\\sqrt{-1} \\pi} \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{2\\sqrt{-1} \\pi} \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+e^{2\\sqrt{-1} \\pi} \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+e^{2\\sqrt{-1} \\pi} \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&MultiplyRationalByAlgebraicNumber,~MultiplyRationalByAlgebraicNumber,~MultiplyRationalByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~MultiplyRationalByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0+0\\cdot 0 = 0,~MultiplyRationalByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~MultiplyRationalByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~MultiplyRationalByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) = 0,~MultiplyRationalByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~MultiplyRationalByAlgebraicNumber,~MultiplyRationalByAlgebraicNumber,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~MultiplyRationalByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) = 0,~MultiplyRationalByAlgebraicNumber,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{4}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~MultiplyRationalByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} = 0,~\\text{Previously~computed~that~} e^{-\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} e^{-\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{-\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{-\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 1+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{-\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{4}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{-\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) = 0,~\\text{Previously~computed~that~} e^{\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} e^{\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{4}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{-2\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{2\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~ConstantExtraction\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\begin{pmatrix}0+0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 1+0+0+0+0+0+0+0 & 0+0+0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0\\\\
0+0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0-\\frac{1}{2}+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0+0+0 & 0+\\frac{1}{2}+0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0\\\\
0+0+\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0+0+0+0+0 & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0+0+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0+0-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0+0+0+0 & 0+0+0+0 & 0+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0\\\\
0+0+0 & 0+0+0 & 0+0+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{2\\sqrt{-1} \\pi} \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+e^{2\\sqrt{-1} \\pi} \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+e^{2\\sqrt{-1} \\pi} \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\begin{pmatrix}0+0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0 & 1+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0\\\\
0+0+0+0+0+0+0 & 0+0+0+0+0+0 & -\\frac{1}{2}+0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0+0 & \\frac{1}{2}+0+0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0\\\\
0+\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0+0+0+0+0 & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0+0+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0+0+0 & 0+0+0 & 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0\\\\
0+0 & 0+0 & 0+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{2\\sqrt{-1} \\pi} \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+e^{2\\sqrt{-1} \\pi} \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+e^{2\\sqrt{-1} \\pi} \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddTerms,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddTerms,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\begin{pmatrix}0+0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0 & 1+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0 & 0+0+0+0\\\\
0+0+0+0+0+0 & 0+0+0+0+0 & -\\frac{1}{2}+0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0+0 & \\frac{1}{2}+0+0+0+0+0 & 0+0+0+0 & 0+0+0+0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0+0+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0+0+0 & 0+0+0 & 0+0+0 & 0+0+0 & 0+0+0 & 0+0+0 & 0+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0+0 & 0+0 & 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0\\\\
0 & 0 & 0+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{2\\sqrt{-1} \\pi} \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+e^{2\\sqrt{-1} \\pi} \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+e^{2\\sqrt{-1} \\pi} \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddTerms\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\begin{pmatrix}0+0+0+0+0 & 0+0+0+0 & 0+0+0+0+0 & 0+0+0+0 & 1+0+0+0+0 & 0+0+0+0+0 & 0+0+0 & 0+0+0\\\\
0+0+0+0+0 & 0+0+0+0 & -\\frac{1}{2}+0+0+0+0 & 0+0+0+0 & 0+0+0+0+0 & \\frac{1}{2}+0+0+0+0 & 0+0+0 & 0+0+0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0+0+0+0+0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0+0+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0+0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0+0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0+0 & 0+0 & 0+0 & 0+0 & 0+0 & 0+0 & 0+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0 & 0 & 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0\\\\
0 & 0 & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{2\\sqrt{-1} \\pi} \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+e^{2\\sqrt{-1} \\pi} \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+e^{2\\sqrt{-1} \\pi} \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddTerms,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddTerms,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} = 0,~ConstantExtraction\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\begin{pmatrix}0+0+0+0 & 0+0+0 & 0+0+0+0 & 0+0+0 & 1+0+0+0 & 0+0+0+0 & 0+0 & 0+0\\\\
0+0+0+0 & 0+0+0 & -\\frac{1}{2}+0+0+0 & 0+0+0 & 0+0+0+0 & \\frac{1}{2}+0+0+0 & 0+0 & 0+0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi} & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0+0+0 & 0+0+0 & 0+0+0 & 0+0+0 & 0+0+0 & 0+0+0 & 0-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0+0+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0+0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & 0+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0\\\\
0 & 0 & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0+0-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+e^{2\\sqrt{-1} \\pi} \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+e^{2\\sqrt{-1} \\pi} \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{1}{2}\\right) = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{-\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 1 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{-\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{4}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} e^{-\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddTerms,~AddTerms,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) = 0,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\begin{pmatrix}0+0+0 & 0+0 & 0+0+0 & 0+0 & 1+0+0 & 0+0+0 & 0 & 0\\\\
0+0+0 & 0+0 & -\\frac{1}{2}+0+0 & 0+0 & 0+0+0 & \\frac{1}{2}+0+0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi} & 0+0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0+0+0 & 0+0+0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0\\\\
0+0 & 0+0 & 0+0 & 0+0 & 0+0 & 0+0 & 0-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0+0+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{\\sqrt{-1} \\pi}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} & 0+0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+0 & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0\\\\
0 & 0 & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+e^{2\\sqrt{-1} \\pi} \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+e^{2\\sqrt{-1} \\pi} \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 1+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{4}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2}\\right) = 0,~\\text{Previously~computed~that~} e^{\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} e^{\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{\\sqrt{-1} \\pi} \\frac{1}{2} = \\frac{1}{2} e^{\\sqrt{-1} \\pi},~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~AddAlgebraicNumberToAlgebraicNumber,~AddTerms\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\begin{pmatrix}0+0 & 0 & 0+0 & 0 & 1+0 & 0+0 & 0 & 0\\\\
0+0 & 0 & -\\frac{1}{2}+0 & 0 & 0+0 & \\frac{1}{2}+0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi} & 0+0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & 0-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0+0+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0+0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} & 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+0 & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0\\\\
0 & 0 & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4}\\right)+e^{2\\sqrt{-1} \\pi} \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+e^{2\\sqrt{-1} \\pi} \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddTerms,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddTerms,~\\text{Previously~computed~that~} 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{4}\\right) = 0,~ConstantExtraction\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\begin{pmatrix}0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & -\\frac{1}{2} & 0 & 0 & \\frac{1}{2} & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi} & 0+0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0 & 0+0+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-\\sqrt{-1} \\pi} \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0\\\\
-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\left(-\\frac{\\sqrt{-1}}{4}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0\\\\
0 & 0 & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0+0+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+e^{2\\sqrt{-1} \\pi} \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{-\\sqrt{-1} \\pi} \\frac{1}{2} = \\frac{1}{2} e^{-\\sqrt{-1} \\pi},~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddTerms,~\\text{Previously~computed~that~} 0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{4} = 0,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\begin{pmatrix}0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & -\\frac{1}{2} & 0 & 0 & \\frac{1}{2} & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi} & 0+0+0+0+0 & 0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0 & 0+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0+0+0+0\\\\
-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+0+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} & 0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+0 & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0\\\\
0 & 0 & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+e^{2\\sqrt{-1} \\pi} \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddTerms,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddTerms\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\begin{pmatrix}0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & -\\frac{1}{2} & 0 & 0 & \\frac{1}{2} & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi} & 0+0+0+0 & 0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+0 & 0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}\\\\
-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0+0+0 & 0+0+0 & 0+0+0 & 0+0+0 & 0+0+0 & 0+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} & 0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+0 & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0\\\\
0 & 0 & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4}+e^{2\\sqrt{-1} \\pi} \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddTerms,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~\\text{Previously~computed~that~} e^{2\\sqrt{-1} \\pi} \\frac{1}{4} = \\frac{1}{4} e^{2\\sqrt{-1} \\pi}\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\begin{pmatrix}0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & -\\frac{1}{2} & 0 & 0 & \\frac{1}{2} & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi} & 0+0+0 & 0+0 & 0+0+0 & 0+0+0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}\\\\
-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0+0 & 0+0 & 0+0 & 0+0 & 0+0 & 0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+e^{-2\\sqrt{-1} \\pi} \\frac{1}{4}+0\\cdot \\frac{1}{4} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0\\\\
0 & 0 & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0+0+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{-2\\sqrt{-1} \\pi} \\frac{1}{4} = \\frac{1}{4} e^{-2\\sqrt{-1} \\pi},~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} = 0,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\begin{pmatrix}0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & -\\frac{1}{2} & 0 & 0 & \\frac{1}{2} & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi} & 0+0 & 0 & 0+0 & 0+0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}\\\\
-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi} & 0+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0\\\\
0 & 0 & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddTerms\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\begin{pmatrix}0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & -\\frac{1}{2} & 0 & 0 & \\frac{1}{2} & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}\\\\
-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi} & 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0\\\\
0 & 0 & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+e^{2\\sqrt{-1} \\pi}\\cdot 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddTerms,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{2\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} e^{2\\sqrt{-1} \\pi}\\cdot 0 = 0\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\begin{pmatrix}0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & -\\frac{1}{2} & 0 & 0 & \\frac{1}{2} & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}\\\\
-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot \\frac{1}{2}+e^{-2\\sqrt{-1} \\pi}\\cdot 0+0\\cdot 0\\\\
0 & 0 & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0+0+0 & 0+0+0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2}\\right)+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} = 0,~\\text{Previously~computed~that~} e^{-2\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} e^{-2\\sqrt{-1} \\pi}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\begin{pmatrix}0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & -\\frac{1}{2} & 0 & 0 & \\frac{1}{2} & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}\\\\
-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} & 0+0+0+0 & 0+0+0+0\\\\
0 & 0 & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0+0 & 0+0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\begin{pmatrix}0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & -\\frac{1}{2} & 0 & 0 & \\frac{1}{2} & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}\\\\
-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} & 0+0+0 & 0+0+0\\\\
0 & 0 & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0 & 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\begin{pmatrix}0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & -\\frac{1}{2} & 0 & 0 & \\frac{1}{2} & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}\\\\
-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} & 0+0 & 0+0\\\\
0 & 0 & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0 & 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0 & 0 & -\\sqrt{-1} & 0 & \\sqrt{-1} & 0 & 0 & 0\\\\
0 & 0 & 1 & 0 & 1 & 0 & 0 & 0\\\\
0 & -1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
\\frac{1}{2} & 0 & 0 & 0 & 0 & 0 & -\\sqrt{-1} & \\sqrt{-1}\\\\
1 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\\\
0 & 0 & 0 & \\sqrt{-1} & 0 & -\\sqrt{-1} & 0 & 0\\\\
0 & 0 & 0 & 1 & 0 & 1 & 0 & 0 \\end{pmatrix}\\right)\\begin{pmatrix}0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & -\\frac{1}{2} & 0 & 0 & \\frac{1}{2} & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}\\\\
-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0 & 0 \\end{pmatrix} \\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&MultiplyMatrixByMatrix\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+\\sqrt{-1} \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot \\frac{1}{2}-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+\\sqrt{-1}\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\sqrt{-1}\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0+0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0+0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0\\cdot 0+0+0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0+0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
0\\cdot 0-0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0+0 & 0\\cdot 0-0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0+0 & 0\\cdot 0-\\left(-\\frac{1}{2}\\right)+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
\\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\right)+0\\cdot 0-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & \\frac{1}{2}\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\sqrt{-1} \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 & 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
0\\cdot 0+0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0+0 & 0\\cdot 0+0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0+0 & 0\\cdot 0+\\left(-\\frac{1}{2}\\right)+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\sqrt{-1}\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\right)-\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\sqrt{-1}\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0-\\sqrt{-1} \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\sqrt{-1} \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0-\\sqrt{-1} \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\right)+0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0+0+0\\cdot 0+0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+0+0\\cdot 0+0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0\\cdot 0+0\\cdot 0+0+0\\cdot 0+0+0 \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0+0\\cdot 0+0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&MultiplyAlgebraicNumberByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0 = 0,~MultiplyByOne,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~MultiplyByOne,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~MultiplyAlgebraicNumberByAlgebraicNumber,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0-0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~MultiplyAlgebraicNumberByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~MultiplyByOne,~MultiplyByOne,~MultiplyAlgebraicNumberByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} -\\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} \\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} \\frac{1}{2}\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} -\\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} \\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} \\frac{1}{2}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{1}{2}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~ConstantExtraction,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{1}{2}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) = 0,~MultiplyAlgebraicNumberByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~MultiplyAlgebraicNumberByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} = \\frac{1}{4} e^{-2\\sqrt{-1} \\pi},~\\text{Previously~computed~that~} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} = \\frac{1}{4} e^{2\\sqrt{-1} \\pi},~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} \\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\right) = 0,~\\text{Previously~computed~that~} -\\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} \\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} -\\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} \\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} -\\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} \\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} -\\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 1+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} \\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} -\\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} \\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} -\\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\right) = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 1+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~MultiplyByOne,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~MultiplyByOne,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0+0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0+0 & 0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+\\sqrt{-1} \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot \\frac{1}{2}-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+\\sqrt{-1}\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\sqrt{-1}\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
0+\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0+0 & 0\\cdot 0+0\\cdot 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0+0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0+0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0\\cdot 0+0+0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0+0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
0+0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+\\frac{1}{2}+0+0+0+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0+0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & \\frac{1}{2}\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\sqrt{-1} \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0+0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 1+0+0+0+0+0+0+0 & 0+0+0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0\\\\
0+0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0-\\frac{1}{2}+0+0+0+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\sqrt{-1} \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0-\\sqrt{-1} \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddTerms,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0+0 & 0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+\\sqrt{-1} \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot \\frac{1}{2}-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+\\sqrt{-1}\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\sqrt{-1}\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0+0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0+0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0\\cdot 0+0+0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0+0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
0+0+0+0+0+0+0 & 0+0+0+0+0+0 & \\frac{1}{2}+0+0+0+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0+0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & \\frac{1}{2}\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\sqrt{-1} \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0+0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0 & 1+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0\\\\
0+0+0+0+0+0+0 & 0+0+0+0+0+0 & -\\frac{1}{2}+0+0+0+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\sqrt{-1} \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0-\\sqrt{-1} \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddTerms,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0 = 0,~MultiplyByOne,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~MultiplyByOne,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+\\sqrt{-1} \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot \\frac{1}{2}-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+\\sqrt{-1}\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\sqrt{-1}\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0+0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0+0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0+0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0\\cdot 0+0+0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0+0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
0+0+0+0+0+0 & 0+0+0+0+0 & \\frac{1}{2}+0+0+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0+0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & \\frac{1}{2}\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\sqrt{-1} \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0+0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0 & 1+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0 & 0+0+0+0\\\\
0+0+0+0+0+0 & 0+0+0+0+0 & -\\frac{1}{2}+0+0+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\sqrt{-1} \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0-\\sqrt{-1} \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~AddTerms,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0+0 & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot \\frac{1}{2}-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+\\sqrt{-1}\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\sqrt{-1}\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)+0+0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0+0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0\\cdot 0+0+0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot \\frac{1}{2}+0+0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0+0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
0+0+0+0+0 & 0+0+0+0 & \\frac{1}{2}+0+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0+0+0+0+0 & 0+0+0+0 & 0+0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & \\frac{1}{2}\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\sqrt{-1} \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0+0+0+0+0 & 0+0+0+0 & 0+0+0+0+0 & 0+0+0+0 & 1+0+0+0+0 & 0+0+0+0+0 & 0+0+0 & 0+0+0\\\\
0+0+0+0+0 & 0+0+0+0 & -\\frac{1}{2}+0+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\sqrt{-1} \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0-\\sqrt{-1} \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
0+0+0 & 0+0+0 & 0+0+0 & 0+0+0 & 0+0+0 & 0+0+0 & 0-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddTerms,~\\text{Previously~computed~that~} 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right) = 0,~MultiplyAlgebraicNumberByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 1+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right) = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0\\cdot 0+0 \\left(-\\frac{1}{2}\\right)-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+0\\cdot \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot \\frac{1}{2}-\\sqrt{-1}\\cdot 0+0\\cdot 0+\\sqrt{-1}\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+\\sqrt{-1}\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 & 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\sqrt{-1}\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0\\\\
0+0+0+0 & 0+0+0 & \\frac{1}{2}+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0+0+0+0 & 0+0+0 & 0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & \\frac{1}{2}\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\sqrt{-1} \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0+0+0+0 & 0+0+0 & 0+0+0+0 & 0+0+0 & 1+0+0+0 & 0+0+0+0 & 0+0 & 0+0\\\\
0+0+0+0 & 0+0+0 & -\\frac{1}{2}+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0+0+0 & 0+0+0 & 0+0+0 & 0+0+0 & 0+0+0 & 0+0+0 & 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\sqrt{-1} \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0-\\sqrt{-1} \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
0+0 & 0+0 & 0+0 & 0+0 & 0+0 & 0+0 & 0-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&\\text{Previously~computed~that~} 0\\cdot 0 = 0,~MultiplyAlgebraicNumberByAlgebraicNumber,~MultiplyAlgebraicNumberByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~MultiplyAlgebraicNumberByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} -\\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} \\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~ConstantExtraction,~MultiplyAlgebraicNumberByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} -\\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} \\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~MultiplyAlgebraicNumberByAlgebraicNumber,~\\text{Previously~computed~that~} -\\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} \\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{4} e^{2\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} \\sqrt{-1}\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0-\\sqrt{-1}\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} \\sqrt{-1}\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0+0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0+0+0 & 0+0+0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0\\\\
0+0+0 & 0+0 & \\frac{1}{2}+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0+0+0 & 0+0 & 0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & \\frac{1}{2}\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\sqrt{-1} \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0+0+0 & 0+0 & 0+0+0 & 0+0 & 1+0+0 & 0+0+0 & 0 & 0\\\\
0+0+0 & 0+0 & -\\frac{1}{2}+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0+0 & 0+0 & 0+0 & 0+0 & 0+0 & 0+0 & 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\sqrt{-1} \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0-\\sqrt{-1} \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & 0-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddTerms\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0+0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0 & 0+0+0+0\\\\
0+0 & 0 & \\frac{1}{2}+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0+0 & 0 & 0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & \\frac{1}{2}\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\sqrt{-1} \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0+0 & 0 & 0+0 & 0 & 1+0 & 0+0 & 0 & 0\\\\
0+0 & 0 & -\\frac{1}{2}+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\sqrt{-1} \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0-\\sqrt{-1} \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0 \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddTerms,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} \\frac{1}{2} e^{-\\sqrt{-1} \\pi} = \\frac{1}{2} e^{-\\sqrt{-1} \\pi},~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} \\frac{1}{2} e^{\\sqrt{-1} \\pi} = \\frac{1}{2} e^{\\sqrt{-1} \\pi},~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0+0+0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0+0+0 & 0+0+0+0 & 0+0+0+0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+0+0 & 0+0+0\\\\
0 & 0 & \\frac{1}{2}+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & \\frac{1}{2}\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\sqrt{-1} \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & -\\frac{1}{2}+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0\\cdot 0+0\\cdot 0+0\\cdot 0+\\sqrt{-1} \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0-\\sqrt{-1} \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0+\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0+\\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0 \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddTerms,~AddTerms,~AddTerms,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~AddTerms\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0+0+0+0+0 & 0+0+0+0 & 0+0+0+0+0 & 0+0+0+0+0 & 0+0+0 & 0+0+0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0+0+0 & 0+0+0 & 0+0+0 & 0+0+0 & 0+0 & 0+0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & 0\\cdot 0-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & \\frac{1}{2}\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\sqrt{-1} \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & 0\\cdot 0+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}+\\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) & 0\\cdot 1+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0+\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+0-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0-0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~MultiplyByOne,~MultiplyByOne,~\\text{Previously~computed~that~} \\frac{1}{2}\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0+0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} = \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi},~\\text{Previously~computed~that~} \\left(-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right) = -\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi},~AddTerms\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0+0+0+0 & 0+0+0 & 0+0+0+0 & 0+0+0+0 & 0+0 & 0+0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0+0 & 0+0 & 0+0 & 0+0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & 0+0+0+0+0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0\\cdot 1-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0+0+0+0+0+\\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\sqrt{-1} \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & 0+0+0+0+0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0\\cdot 1+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0+0+0 & 0+0 & 0+0+0 & 0+0+0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & 0+0+0+0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0\\cdot 1-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0+0+0+0+\\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\sqrt{-1} \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & 0+0+0+0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0\\cdot 1+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0+0 & 0 & 0+0 & 0+0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & 0+0+0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0\\cdot 1-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0+0+0+\\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\sqrt{-1} \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & 0+0+0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0\\cdot 1+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & 0+0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0\\cdot 1-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0+0+\\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\sqrt{-1} \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & 0+0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0\\cdot 1+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0\\cdot 1-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0+\\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\sqrt{-1} \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & 0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0\\cdot 1+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddTerms,~AddTerms,~AddTerms\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0\\cdot 1-0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\sqrt{-1} \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0\\cdot 1+0+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right)+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&\\text{Previously~computed~that~} 0\\cdot 1 = 0,~\\text{Previously~computed~that~} -0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~MultiplyByOne,~MultiplyByOne,~MultiplyAlgebraicNumberByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 1 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}\\right) = -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi},~\\text{Previously~computed~that~} \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} = \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi}\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0+0+0+0+0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}+0+0+0+0+0-\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0+0+0+0+0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0+0+0+0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}+0+0+0+0-\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0+0+0+0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0+0+0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}+0+0+0-\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0+0+0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0+0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}+0+0-\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0+0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}+0-\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}-\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & 0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddTerms,~AddTerms,~AddTerms\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{1}{2}\\cdot 0+0\\cdot \\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0-\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+\\frac{1}{2}+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&\\text{Previously~computed~that~} 0\\cdot 0 = 0,~MultiplyAlgebraicNumberByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} = \\frac{1}{4} e^{-2\\sqrt{-1} \\pi},~\\text{Previously~computed~that~} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} = \\frac{1}{4} e^{2\\sqrt{-1} \\pi},~\\text{Previously~computed~that~} \\frac{1}{2}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} -\\sqrt{-1} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} = -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi},~\\text{Previously~computed~that~} \\sqrt{-1} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} = \\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi},~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~MultiplyAlgebraicNumberByAlgebraicNumber,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} \\frac{1}{4} e^{-2\\sqrt{-1} \\pi} = \\frac{1}{4} e^{-2\\sqrt{-1} \\pi},~\\text{Previously~computed~that~} \\frac{1}{4} e^{2\\sqrt{-1} \\pi} = \\frac{1}{4} e^{2\\sqrt{-1} \\pi}\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0-\\frac{1}{2}+0+0+0+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & 0+0+0+0+0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0+\\frac{1}{2}+0+0+0+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & -\\frac{1}{2}+0+0+0+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & 0+0+0+0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}+0+0+0+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & -\\frac{1}{2}+0+0+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & 0+0+0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}+0+0+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & -\\frac{1}{2}+0+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & 0+0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}+0+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & -\\frac{1}{2}+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & 0+0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}+0+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & -\\frac{1}{2}+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & 0-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}+\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddTerms,~AddTerms,~AddTerms\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & 0\\cdot 0-0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0-0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0 & \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}-\\sqrt{-1}\\cdot 0+\\sqrt{-1}\\cdot 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & 0\\cdot 0+0+0\\cdot 0+0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right)+0\\cdot 0+0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}+0+0 & 0\\cdot 0+0+0\\cdot 0+0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+0\\cdot 0+0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi}+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&\\text{Previously~computed~that~} 0\\cdot 0-0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0-0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} -\\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} \\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} \\frac{1}{2}\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} -\\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} \\sqrt{-1}\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0 \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}\\right) = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{-\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot \\frac{1}{2} e^{\\sqrt{-1} \\pi} = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0 = 0\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & 0+0+0+0+0+0 & 0+0+0+0+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0+0+0+0+0+0 & 0+0+0+0+0+0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & 0+0+0+0+0+0 & 0+0+0+0+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & 0+0+0+0+0 & 0+0+0+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0+0+0+0+0 & 0+0+0+0+0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & 0+0+0+0+0 & 0+0+0+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & 0+0+0+0 & 0+0+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0+0+0+0 & 0+0+0+0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & 0+0+0+0 & 0+0+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToRational\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & 0+0+0 & 0+0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0+0+0 & 0+0+0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & 0+0+0 & 0+0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & 0+0 & 0+0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0+0 & 0+0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & 0+0 & 0+0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\left(\\begin{pmatrix}\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & 0 & 0 & 0 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & 0 & 0\\\\
0 & 0 & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & 0 & 0\\\\
0 & 0 & 0 & 0 & 1 & 0 & 0 & 0\\\\
0 & 0 & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2} & \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} & -\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi} & \\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2} & 0 & 0\\\\
0 & 0 & 0 & 0 & 0 & 0 & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} & \\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\\\
0 & 0 & 0 & 0 & 0 & 0 & -\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi} & \\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi} \\end{pmatrix}\\right)\\begin{pmatrix}0\\\\
0\\\\
0\\\\
1\\\\
0\\\\
0\\\\
0\\\\
0 \\end{pmatrix}&MultiplyMatrixByMatrix\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\begin{pmatrix}\\left(\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi}\\right)0+\\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\right)0+0\\cdot 0+0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0\\\\
\\left(\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\right)0+\\left(\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi}\\right)0+0\\cdot 0+0\\cdot 1+0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 0\\\\
0\\cdot 0+0\\cdot 0+\\left(\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2}\\right)0+\\left(\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right)1+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi}\\right)0+\\left(\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2}\\right)0+0\\cdot 0+0\\cdot 0\\\\
0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi}\\right)0+\\left(\\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi}\\right)1+\\left(-\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}-\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2}\\right)0+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi}\\right)0+0\\cdot 0+0\\cdot 0\\\\
0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 1+0+0\\cdot 0+0\\cdot 0+0\\cdot 0\\\\
0\\cdot 0+0\\cdot 0+\\left(\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2}\\right)0+\\left(\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right)1+\\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi}\\right)0+\\left(\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2}\\right)0+0\\cdot 0+0\\cdot 0\\\\
0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 1+0\\cdot 0+0\\cdot 0+\\left(\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi}\\right)0+\\left(\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\right)0\\\\
0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 1+0\\cdot 0+0\\cdot 0+\\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\right)0+\\left(\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi}\\right)0 \\end{pmatrix}&ConstantExtraction,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 1 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~ConstantExtraction,~\\text{Previously~computed~that~} \\left(\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi}\\right)0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 1 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0 = 0,~ConstantExtraction,~ConstantExtraction,~ConstantExtraction,~ConstantExtraction,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi}\\right)0 = 0,~ConstantExtraction,~ConstantExtraction,~\\text{Previously~computed~that~} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi}\\right)0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 1 = 0,~\\text{Previously~computed~that~} 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} \\left(\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}-\\frac{1}{2}\\right)0 = 0,~\\text{Previously~computed~that~} \\left(\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right)1 = \\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi},~\\text{Previously~computed~that~} \\left(-\\frac{\\sqrt{-1}}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{4} e^{2\\sqrt{-1} \\pi}\\right)0 = 0,~\\text{Previously~computed~that~} \\left(\\frac{1}{4} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{4} e^{2\\sqrt{-1} \\pi}+\\frac{1}{2}\\right)0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 1 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0 = 0,~\\text{Previously~computed~that~} \\left(\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi}\\right)0 = 0,~\\text{Previously~computed~that~} \\left(\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\right)0 = 0,~\\text{Previously~computed~that~} 0\\cdot 0+0\\cdot 0+0\\cdot 0+0\\cdot 1+0\\cdot 0+0\\cdot 0 = 0,~\\text{Previously~computed~that~} \\left(-\\frac{\\sqrt{-1}}{2} e^{-\\sqrt{-1} \\pi}+\\frac{\\sqrt{-1}}{2} e^{\\sqrt{-1} \\pi}\\right)0 = 0,~\\text{Previously~computed~that~} \\left(\\frac{1}{2} e^{-\\sqrt{-1} \\pi}+\\frac{1}{2} e^{\\sqrt{-1} \\pi}\\right)0 = 0\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\begin{pmatrix}0+0+0+0+0+0+0+0\\\\
0+0+0+0+0+0+0+0\\\\
0+0+\\left(\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right)+0+0+0+0\\\\
0+0+\\left(\\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi}\\right)+0+0+0+0\\\\
0+0+0+0+0+0\\\\
0+0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}+0+0+0+0\\\\
0+0+0+0+0\\\\
0+0+0 \\end{pmatrix}&AddRationals,~AddRationals,~AddAlgebraicNumberToRational,~MultiplyByOne,~AddAlgebraicNumberToRational,~MultiplyByOne,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\begin{pmatrix}0+0+0+0+0+0+0\\\\
0+0+0+0+0+0+0\\\\
0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}+0+0+0+0\\\\
0+\\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi}+0+0+0+0\\\\
0+0+0+0+0\\\\
0+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}+0+0+0+0\\\\
0+0+0+0\\\\
0+0 \\end{pmatrix}&AddRationalToAlgebraicNumber,~AddRationalToAlgebraicNumber,~AddTerms,~AddTerms,~AddAlgebraicNumberToAlgebraicNumber,~AddTerms,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\begin{pmatrix}0+0+0+0+0+0\\\\
0+0+0+0+0+0\\\\
\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\\\
\\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi}\\\\
0+0+0+0\\\\
\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\\\
0+0+0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\begin{pmatrix}0+0+0+0+0\\\\
0+0+0+0+0\\\\
\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\\\
\\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi}\\\\
0+0+0\\\\
\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\\\
0+0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToRational\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\begin{pmatrix}0+0+0+0\\\\
0+0+0+0\\\\
\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\\\
\\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi}\\\\
0+0\\\\
\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\begin{pmatrix}0+0+0\\\\
0+0+0\\\\
\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\\\
\\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi}\\\\
0\\\\
\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\begin{pmatrix}0+0\\\\
0+0\\\\
\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\\\
\\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi}\\\\
0\\\\
\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\\\
0\\\\
0 \\end{pmatrix}&AddAlgebraicNumberToAlgebraicNumber,~AddAlgebraicNumberToAlgebraicNumber\\\\
=&
\\left(g_{-3}, g_{-2}, g_{-1}, h_{1}, h_{2}, g_{1}, g_{2}, g_{3}\\right) \\begin{pmatrix}0\\\\
0\\\\
\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\\\
\\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi}\\\\
0\\\\
\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\\\
0\\\\
0 \\end{pmatrix}&MultiplySequenceByMatrix\\\\
=&
{Sequence{}\\left(\\left(g_{-1}\\right)\\left(\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right)+\\left(h_{1}\\right)\\left(\\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi}\\right)+\\left(g_{1}\\right)\\left(\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right)\\right)}&CommuteConstants,~CommuteConstants,~CommuteConstants\\\\
=&
{Sequence{}\\left(\\left(\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right)g_{-1}+\\left(\\frac{1}{2} e^{-2\\sqrt{-1} \\pi}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi}\\right)h_{1}+\\left(\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi}\\right)g_{1}\\right)}&DistributeMultiplication,~DistributeMultiplication,~DistributeMultiplication\\\\
=&
{Sequence{}\\left(\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} g_{-1}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} g_{-1}+\\frac{1}{2} e^{-2\\sqrt{-1} \\pi} h_{1}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} h_{1}+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} g_{1}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} g_{1}\\right)}&AssociativeRule,~AssociativeRule,~AssociativeRule,~AssociativeRule,~AssociativeRule,~AssociativeRule,~AddTerms\\\\
=&
{Sequence{}\\left(\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} g_{-1}+\\frac{\\sqrt{-1}}{2} e^{-2\\sqrt{-1} \\pi} g_{1}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} g_{-1}-\\frac{\\sqrt{-1}}{2} e^{2\\sqrt{-1} \\pi} g_{1}+\\frac{1}{2} e^{-2\\sqrt{-1} \\pi} h_{1}+\\frac{1}{2} e^{2\\sqrt{-1} \\pi} h_{1}\\right)}\\end{array}`,
    // Not testing svg
    skipSVG: true,
    skipMathML: false,
    skipEquationEditor: true,
    // Too slow to test on Firefox.
    // Seems to run at a good speed in Chrome.
    // Perhaps the editor needs Firefox-specific improvements?
    skipStaticDOM: true,
  },
];

/** @type {TestCase[]} */
const allTestCases = [];
for (const input of allLatexSnippets) {
  allTestCases.push(constructTestCase(input));
}

/** @type {Object.<string, string>} */
const parsedToLatex = {
  '2 or 3': '2or3',
  '2~or\\ 3': '2~or~3',
}

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


/** @return {EquationEditor} */
function renderLatexReturnEditor(
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
  return equationEditor;
}

describe("Equation editor", () => {
  it("Equation editor editable renders", async () => {
    for (const testCase of allTestCases) {
      if (testCase.skipEquationEditor || testCase.skipTest) {
        continue;
      }
      const container = renderLatex(testCase.content, true, false, false);
      const thirdChild = container.childNodes[2];
      expect(thirdChild.getAttribute("mathtagname")).toBe("root");
      expect(container.getAttribute("latexsource").length > 0).toBe(true)
    }
  });

  it("Equation editor renders static DOM", async () => {
    for (const testCase of allTestCases) {
      if (testCase.skipStaticDOM || testCase.skipTest) {
        continue;
      }
      const container = renderLatex(testCase.content, false, false, false);
      const secondChild = container.childNodes[1];
      expect(secondChild.getAttribute("mathtagname")).toBe("root");
      expect(container.getAttribute("latexsource").length > 0).toBe(true);
      container.textContent = "";
    }
  });

  it("Equation editor renders with SVG", async () => {
    for (const testCase of allTestCases) {
      if (testCase.skipSVG || testCase.skipTest) {
        continue;
      }
      const container = renderLatex(testCase.content, false, true, false);
      const secondChild = container.childNodes[1];
      expect(secondChild.getAttribute("mathtagname")).toBe("root");
      expect(container.getAttribute("latexsource").length > 0).toBe(true)
    }
  });

  it("Equation editor renders with mathML", async () => {
    for (const testCase of allTestCases) {
      if (testCase.skipMathML || testCase.skipTest) {
        continue;
      }
      const editor = renderLatexReturnEditor(testCase.content, false, false, true);
      const container = editor.container;
      const annotation = container.childNodes[1].childNodes[0].childNodes[0].childNodes[1];
      const secondChild = container.childNodes[1];
      expect(secondChild.getAttribute("mathtagname")).toBe("root");
      expect(container.getAttribute("latexsource").length > 0).toBe(true)
      // The mathml annotation leaves the original latex source unchanged.
      expect(annotation.textContent).toBe(testCase.content);
    }
  });
});