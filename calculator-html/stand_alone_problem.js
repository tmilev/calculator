"use strict";

function StandAloneProblem() {

}

StandAloneProblem.prototype.initAndRun = function (input) {
  console.log(`DEBUG: The input I got was: ${input}`);
}

var standAloneProblem = new StandAloneProblem();

module.exports = {
  standAloneProblem,
};