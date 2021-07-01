# Calculator frontend

This folder and its subfolders contain the calculator frontend.

## Build.

The calculator frontend requires no special build procedure.
Once the back-end is installed, the front-end should work out of the box.

The calculator front-end is a one-page app, with a single html page called
index.html. 

### CommonJS modules
1. All js scripts are specified in file BUILD.json
 and are assembled into a single file, javascript_all_in_one.js.

- The assembly is done automatically by the calculator web
server and requires no action. 

- The javascript_all_in.one.js contains an object in which the keys are the 
javascript file names (prefixed with /calculator_html/ and with the .js suffix stripped).

- The script browserifier.js is executed. 

- The script javascript_all_in_one.js in ecapsulated in an anonymous function 
and should not pollute your namespace. However, as of writing, the script will 
use local storage, cookies, and will install itself in the window global variable 
under window.calculator.

- The browserification scheme works in the same way as the standard node
browserify tool, but is a little simplified. More precisely,
the exports variable is not used and instead only
module.exports is used.

2. We also have means to bootstrap an arbitrary npm commonJS module as a part of
javascript_all_in_one.js. All such modules are bootstrapped through
the single javascript file /calculator_html/external-build/output.js.
This file is pre-built: in the present paragraph we explain how to make that build.

- In order to build output.js, you need a node server with npm installed.
- Navigate to calculator_html/external.
- Install the dependencies

```
npm install
```

- Run the build script.

```
npm run browserify
```

### Additional javascript sources
All script tags with sources not in /calculator_html/ are interpreted as
regular html scripts. Such external sources are located in /html-common/
or any other directory accessible without admin privileges through the calculator.






