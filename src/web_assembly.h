// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_web_assembly_ALREADY_INCLUDED
#define header_web_assembly_ALREADY_INCLUDED

class WebAssembly {
public:
  static const int bufferSize = 1000000;
  static int numberOfMainLoopCalls;
  static int main(int argc, char **argv);
  static void mainLoop();
};

#endif // header_web_assembly_ALREADY_INCLUDED
