"use strict";

var module;
if (module === undefined) {
  module = { 
    exports: null,
  };
}

class MathNodeType {
  constructor(id, borderStyle) {
    this.id = id;
    this.borderStyle = borderStyle;
  }
}

var knownTypes = {
  root: new MathNodeType("root", "1px solid black"),
};



class EquationEditor {
  constructor(
    /** @type{HTMLElement} */
    container,
  ) {
    console.log("DEBUG: Hello world!!!");
    this.container = container;
    this.container.innerHTML = "";
    let element = document.createElement("span");
    this.container.appendChild(element);
    this.rootNode = new MathNode(null, element, knownTypes.root, 1);
  }
}

class MathNode {
  constructor(
    /** @type {MathNode} */
    root,
    /** @type{HTMLElement} */
    element, 
    /** @type {MathNodeType} */ 
    type, 
    /** @type {number} */
    scale
  ) {
    /** @type{Array<MathNode>} */
    this.children = [];
    /** @type {MathNodeType} */
    this.type = type;
    /** @type{HTMLElement} */
    this.element = element;
    /** @type {MathNode} */
    this.root = root;
    this.scale = scale;
    this.element.innerHTML = "";
    this.element.style.border = this.type.borderStyle;
    this.element.style.display = "inline-block";
    this.element.contentEditable = true;
    this.element.addEventListener("keydown", (e)=>{this.handleKey(e);});
    this.element.addEventListener("focus", (e)=>{this.handleFocus(e);});
    this.element.addEventListener("blur", (e)=>{this.handleBlur(e);});
    this.content = "";
    this.hasContent = true;
    this.hasFocus = true;
    this.compute();
  }  
  compute() {
    const fontSizeString = window.getComputedStyle(this.element, null).getPropertyValue('font-size');
    const fontSize = parseFloat(fontSizeString); 
    this.element.style.minHeight = this.scale * fontSize;
    this.element.style.minWidth = this.scale * fontSize / 1.6;
    this.element.textContent = this.content;
  }
  handleFocus(e) {
    this.element.style.background = "lightblue";
  }
  handleBlur(e) {
    this.element.style.background = "";
  }
  handleKey(
    /** @type {KeyboardEvent} */
    e,
  ) {
    let key = e.key;
    switch(key) {
      default:
        this.updateContent(key);
    }
  }
  updateContent(/** @type{string} */ key) {
  }
}

module.exports = {
  EquationEditor,
};