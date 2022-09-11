
#ifndef header_web_assembly_ALREADY_INCLUDED
#define header_web_assembly_ALREADY_INCLUDED

class WebAssembly {
public:
  static const int bufferSize = 1000000;
  static int numberOfMainLoopCalls;
  static int main(int argc, char** argv);
  static void mainLoop();
};

#endif // header_web_assembly_ALREADY_INCLUDED
