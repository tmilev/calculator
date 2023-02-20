const path = require('path');

module.exports = {
  entry: './app.js',
  output: {
    path: path.resolve(__dirname, 'out'),
    filename: 'equation_editor_minified.js',
  },
};