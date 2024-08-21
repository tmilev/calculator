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
    neighborsAreInRows,
  ) {
    /**@type{HTMLElement}*/
    this.element = document.getElementById(splitterId);
    this.parentElement = document.getElementById(parentId);
    this.neighbor1 = document.getElementById(neighbor1Id);
    this.neighbor2 = document.getElementById(neighbor2Id);
    this.storageVariable = storageVariable;
    this.resizing = false;
    this.neighborsAreInRows = neighborsAreInRows;
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
    if (this.neighborsAreInRows) {
      dimension = xy.y;
    } else {
      dimension = xy.x;
    }
    dimension -= 10;
    this.storageVariable.setAndStore(
      dimension, false, false
    );
    this.setDimension(dimension);
  }

  setDimension(dimension) {
    if (dimension === "" || dimension === undefined || dimension === null) {
      const neighbor1Box = this.neighbor1.getBoundingClientRect();
      if (this.neighborsAreInRows) {
        dimension = neighbor1Box.height;
      } else {
        dimension = neighbor1Box.width;
      }
    }
    if (typeof dimension === "string") {
      dimension = Number(dimension);
    }
    let dimensionParent = 0;
    let parentBounds = this.parentElement.getBoundingClientRect();
    if (this.neighborsAreInRows) {
      dimensionParent = parentBounds.height;
      if (dimensionParent < 500) {
        // TODO: for some reason, as of writing, 
        // the parent element is not fully sized
        // vertically when
        // this code first runs.
        // May be a css bug or a peculiarity of how
        // flexboxes work. 
        // Fall back to
        // the dimension of the window.
        dimensionParent = window.innerHeight;
      }
    } else {
      dimensionParent = parentBounds.width;
    }
    if (dimension > dimensionParent - 100) {
      dimension = dimensionParent - 100;
    }
    if (dimension < 90) {
      dimension = 90;
    }
    // TODO: overflowScrollBandAdjustment should be computed from context.
    let overflowScrollBandAdjustment = 10;
    if (!this.neighborsAreInRows) {
      overflowScrollBandAdjustment = 20;
    }
    let remainingDimension = (
      dimensionParent - dimension - overflowScrollBandAdjustment
    );
    if (remainingDimension < 20) {
      remainingDimension = 20;
    }
    this.neighbor1.style.flexGrow = "0";
    this.neighbor2.style.flexGrow = "0";
    this.neighbor1.style.flexShrink = "0";
    this.neighbor2.style.flexShrink = "0";
    this.neighbor1.style.flexBasis = `${dimension}px`;
    this.neighbor2.style.flexBasis = `${remainingDimension}px`;
  }
}

module.exports = {
  Splitter,
};
