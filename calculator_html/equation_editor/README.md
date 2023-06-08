# A math equation editor written from scratch

Demo: https://calculator-algebra.org:8166/calculator_html/equation_editor/example_equation_editor_with_panel.html

Demo, with a custom button: https://calculator-algebra.org:8166/calculator_html/equation_editor/example_equation_editor.html

Detailed demo+debug page: https://calculator-algebra.org:8166/calculator_html/equation_editor/test_equation_editor.html

- Uses Apache 2.0 license (no more license headaches).
- Written from scratch: not a single line from another project.
- No css. 
	- No fonts. When rendering from `\(\)`-text, inherits the font from the parent html element.
	- When rendering from a div with class="mathcalculator", keeps your font/styling.
- No external dependencies whatsoever, not even jquery.
- No more build system headaches.
	- Can be require("./equation_editor")-ed like a regular node module.
	- Can be copy and pasted: one [single javascript file](https://github.com/tmilev/calculator/blob/HEAD/calculator_html/equation_editor/src/equation_editor.js) and you're done.
- Matrix support from the start.
- Will construct math tags from content enclosed in \\(...\\) just like MathJax; can be used as a drop-in replacement. 
- Will also construct math from ```<div class="mathcalculator">your formula here</div>```.
- Copy+paste your LaTeX at the cursor.
- Prints to SVG and canvas.

```

<html>
<script type="text/javascript" src="equation_editor.js"></script>
<script>
  function createEditor() {
    const options = new EquationEditorOptions({
      // Can you edit the math or is it static math?
      editable: true,
      // Bind a raw LaTeX input box to the editor. Editing the box automatically updates the editor.  
      latexInput: document.getElementById('latex'),
      // Set to true to have your LaTeX sanitized to a preferred mathematical equivalent.
      sanitizeLatexSource: false,
      // Set to true to strip \displaystyle from your latex source, independent of the sanitization flag.
      removeDisplayStyle: false,
      // Show debug-related information.
      debugLogContainer: null,
      // Should I generate a copy button at the corner of the editor?
      copyButton: false,
    });
    let editor = new EquationEditor(document.getElementById('editor'), options);
    // The \cursor command indicates the desired location of the cursor. 
    new EquationEditorButtonFactory("\\frac{123}{\\cursor}\\sqrt{7}", "", "").attachToClickById("custom-button", editor);
    buttonFactories.sqrt.attachToClickById("sqrt-button", editor);
    editor.updateDOM();
    editor.rootNode.focus(- 1);
  }
</script>

<body onload="createEditor()">

  Equation editor here: <div id='editor' style="font-size: 72px; font-family:'Times New Roman'"> </div>
  <br>
  Latex of what you typed, can edit directly: <input id="latex">
  <hr>
  Sample buttons:
  <button id="sqrt-button">&Sqrt;</button>
  <button id="custom-button">custom</button>
  <hr>
  Typeset all math in the 'toBeTypeSet' tag.
  <button id="typeSetButton" onclick="typeset(document.getElementById('toBeTypeSet'));">typeset</button>

  <div id='toBeTypeSet'>
    \(\frac{-b\pm \sqrt{b^2-4ac}}{2a}\)
  </div>
</body>
</html>
```

## Common commands
Build the example app with webpack.

```
npm run build
```

or, alternatively:
```
webpack src/app.js
```

Push a new version to npm. You also need to increment the version number in package.json.

```
npm publish
```

Run the unit tests.

```
npm test
```

Run the unit tests manually.
```
npx jasmine-browser-runner runSpecs
```


