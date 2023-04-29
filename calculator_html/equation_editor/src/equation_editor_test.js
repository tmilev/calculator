describe("Equation editor", function () {
  it("Equation editor with basic content", async function () {
    let container = document.createElement("div");
    document.body.appendChild(container);
    let equationEditor = new EquationEditor(container, null, null);
    equationEditor.writeLatex("\\frac{1}{2}");
    await 0;
    expect(true).toBe(true);
  });
});