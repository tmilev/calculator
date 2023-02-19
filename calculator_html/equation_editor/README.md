# A math equation editor written from scratch

Demo: https://calculator-algebra.org:8166/calculator_html/equation_editor/test_equation_editor.html

- Uses Apache 2.0 license (no more license headaches).
- Written from scratch: not a single line from another project.
- No css. 
	- No fonts. When rendering from `\(\)`-text, inherits the class from the parent html element.
	- When rendering from a div with class="mathcalculator", keeps your styling.
- No external dependencies whatsoever, not even jquery.
- No more build system headaches.
	- Can be require("./equation_editor")-ed like a regular node module.
	- Can be copy and pasted: one [single javascript file](calculator_html/equation_editor.js) and you're done.
- Matrix support from the start.
- Will construct math tags from content enclosed in \\(...\\) just like MathJax; can be used as a drop-in replacement. 
- Will also construct math from <div class="mathcalculator">your formula here</div>
```
