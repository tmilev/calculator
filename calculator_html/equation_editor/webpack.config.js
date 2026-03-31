const path = require('path');

module.exports = {
  mode: 'production',
  entry: {
    demo_app: './src/app_equation_editor_example.js',
    equation_editor_minified: './src/equation_editor_global.js',
  },
  output: {
    path: path.resolve(__dirname, 'out'),
    filename: '[name].js',
  },
};
