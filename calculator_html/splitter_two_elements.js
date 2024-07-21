const storage = require('./storage');

class Splitter {
  constructor(
    /** @type{string} */
    splitterId,
    /** @type{string} */
    parentId,
    /** @type{string} */
    neighbor1Id,
    /** @type{string} */
    neighbor2Id,
    /** @type{storage.StorageVariable} */
    storageVariable,
    /** @type{boolean} */
    neighborsAreInColumns,
  ) {
    /**@type{HTMLElement}*/
    this.element = document.getElementById(splitterId);
    this.parentElement = document.getElementById(parentId);
    this.neighbor1 = document.getElementById(neighbor1Id);
    this.neighbor2 = document.getElementById(neighbor2Id);
    this.storageVariable = storageVariable;
    this.resizing = false;
    this.neighborsAreInColumns = neighborsAreInColumns;
  }
  initialize() {
    this.element.addEventListener('mousedown', (mouseEvent) => {
      this.onMouseDown(mouseEvent);
    });
    this.parentElement.addEventListener('mousemove', (mouseEvent) => {
      this.onMouseMove(mouseEvent);
    });
    this.parentElement.addEventListener('mouseup', (mouseEvent) => {
      this.onMouseUp(mouseEvent);
    });
    this.setDimension(this.storageVariable.getValue());
  }
  getXY(mouseEvent) {
    return {
      x: mouseEvent.clientX,
      y: mouseEvent.clientY,
    }
  }
  onMouseDown(mouseEvent) {
    this.resizing = true;
    mouseEvent.stopPropagation();
  }
  onMouseUp(mouseEvent) {
    this.resizing = false;
    mouseEvent.stopPropagation();
  }
  onMouseMove(mouseEvent) {
    if (!this.resizing) {
      return;
    }
    let xy = this.getXY(mouseEvent);
    let dimension = 0;
    if (this.neighborsAreInColumns) {
      dimension = xy.y;
    } else {
      dimension = xy.x;
    }
    this.storageVariable.setAndStore(
      dimension - 10, false, false
    );
    this.setDimension(dimension - 10);
  }
  setDimension(dimension) {
    if (dimension === "" || dimension === undefined || dimension === null) {
      if (this.neighborsAreInColumns) {
        this.neighbor1.style.height = "";
        this.neighbor2.style.height = "";
      } else {
        this.neighbor1.style.width = "";
        this.neighbor2.style.width = "";
      }
      return;
    }
    let rectangle = this.parentElement.getBoundingClientRect();
    let dimensionParent = 0;
    if (this.neighborsAreInColumns) {
      dimensionParent = rectangle.height;
    } else {
      dimensionParent = rectangle.width;
    }
    if (dimension > dimensionParent - 100) {
      dimension = dimensionParent - 100;
    }
    if (dimension < 90) {
      dimension = 90;
    }
    let remainingDimension = dimensionParent - dimension;
    if (remainingDimension < 100) {
      remainingDimension = 100;
    }
    if (this.neighborsAreInColumns) {
      this.neighbor1.style.height = dimension;
      this.neighbor2.style.height = remainingDimension;
    } else {
      this.neighbor1.style.width = dimension;
      this.neighbor2.style.width = remainingDimension;
    }
  }
}

module.exports = {
  Splitter,
};
