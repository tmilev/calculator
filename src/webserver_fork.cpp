//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#include "webserver.h"
#include <unistd.h>
#include "crypto.h"
// sys/random.h is missing from older distros/kernels.
// #include <sys/random.h>
// Used in place of sys/random.h:
#include <syscall.h> // <- SYS_getrandom defined here.
#include <linux/random.h> // <- GRND_NONBLOCK defined here.
#include <unistd.h> // <- syscall defined here.

static ProjectInformationInstance projectInfoInstanceWebServer(__FILE__, "Web server fork.");
extern WebServer theWebServer;

void WebServer::initializeRandomBytes() {
  static bool alreadyRan = false;
  if (alreadyRan) {
    crash << "Random bytes initialization allowed only once. " << crash;
  }
  if (theGlobalVariables.numberOfRandomBytes > 256) {
    crash << "The number of system random bytes must not exceed 256. " << crash;
  }
  List<unsigned char>& output = theGlobalVariables.randomBytesCurrent;
  output.SetSize(theGlobalVariables.numberOfRandomBytes);
  // According to the
  // documentation of getrandom, the following call
  // must not block or return fewer than the requested bytes in Linux
  // when the number of bytes is less than 256.
  int generatedBytes = static_cast<int>(
    syscall(SYS_getrandom, output.TheObjects, static_cast<unsigned>(output.size), GRND_NONBLOCK)
    // Does not compile on older linux systems:
    // getrandom(output.TheObjects, static_cast<unsigned>(output.size), 0)
  );
  if (generatedBytes != output.size) {
    crash << "Failed to get the necessary number of random bytes. " << crash;
  }
}

int WebServer::Fork() {
  Crypto::GetRandomBytesSecureInternalMayLeaveTracesInMemory(this->idLastWorker, 32);

  // timer taken at server level:
  int64_t millisecondsAtFork = theGlobalVariables.GetElapsedMilliseconds();
  int result = fork();
  // We need to make sure that the child retains no information
  // about the server's random bytes, and similarly the server
  // has no information on the child's random bytes.
  //
  // 1. The parent acquires additional entropy from millisecondsAtFork.
  // Since this one-way overwrites its random bytes, the parent can no
  // longer derive the previous state of its random bytes.
  //
  // 2. The child loses 256 bits of entropy from the parent, and then
  // acquires additional entropy from millisecondsAtFork.
  // This one-way overwrites the random bytes, so the child has no information
  // about the previous state of its random bytes.
  //
  // 3. Since the child forgot 256 bits of entropy from the parent,
  // it has 256 bits entropy difference from its the parent.
  // Since the parent keeps on acquiring more entropy from the timer,
  // this difference grows further over time.
  if (result > 0) {
    // Parent process.
    // Forget previous random bytes, and gain a little extra entropy:
    Crypto::acquireAdditionalRandomness(millisecondsAtFork);
  } else if (result == 0) {
    // child process
    // lose 256 bits of entropy from the server:
    theGlobalVariables.randomBytesCurrent.SetSize(theGlobalVariables.maximumExtractedRandomBytes);
    // Forget previous random bytes, and gain a little extra entropy:
    Crypto::acquireAdditionalRandomness(millisecondsAtFork);
  }
  // if the result is negative, this is an error.
  return result;
}