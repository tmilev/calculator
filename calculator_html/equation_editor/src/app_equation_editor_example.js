const equation_editor = require("./equation_editor");

function createEditor() {
  const options = new equation_editor.EquationEditorOptions({
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
  let editor = new equation_editor.EquationEditor(document.getElementById('editor'), options);
  // The \cursor command indicates the desired location of the cursor. 
  new equation_editor.EquationEditorButtonFactory("\\frac{123}{\\cursor}\\sqrt{7}", "", "").attachToClickById("custom-button", editor);
  equation_editor.buttonFactories.sqrt.attachToClickById("sqrt-button", editor);
  editor.updateDOM();
  editor.rootNode.focus(- 1);
}

if (document.readyState !== 'loading') {
  createEditor();
} else {
  window.addEventListener("load", () => {
    createEditor();
  });
}