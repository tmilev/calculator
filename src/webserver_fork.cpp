 // The current file is licensed under the license terms found in the main header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "webserver.h"
#include <unistd.h>
#include <sys/prctl.h> //<- prctl here
#include <signal.h> // <-signals here

#ifndef MACRO_use_wasm

int WebServer::forkRaw() {
  return fork();
}

int WebServer::forkProcess() {
  if (!this->createProcessMutex()) {
    global << Logger::red << "Failed to create process mutex." << Logger::endL;
    return - 1;
  }
  this->computeActiveWorkerId();
  // timer taken at server level:
  int64_t millisecondsAtfork = global.getElapsedMilliseconds();
  int result = this->forkRaw();
  // We need to make sure that the child retains no information
  // about the server's random bytes, and similarly the server
  // has no information on the child's random bytes.
  //
  // 1. The parent acquires additional entropy from millisecondsAtfork.
  // Since this one-way overwrites its random bytes, the parent can no
  // longer derive the previous state of its random bytes.
  //
  // 2. The child loses 256 bits of entropy from the parent, and then
  // acquires additional entropy from millisecondsAtfork.
  // This one-way overwrites the random bytes, so the child has no information
  // about the previous state of its random bytes.
  //
  // 3. Since the child forgot 256 bits of entropy from the parent,
  // it has 256 bits entropy difference from its the parent.
  // Since the parent keeps on acquiring more entropy from the timer,
  // this difference grows further over time.
  if (result > 0) {
    // Parent process.
    // Forget previous random bytes, and gain a little extra entropy.
    Crypto::Random::acquireAdditionalRandomness(millisecondsAtfork);
  } else if (result == 0) {
    global.logs.logType = GlobalVariables::LogData::type::worker;
    // Child process.
    // Lose 256 bits of entropy from the server.
    global.randomBytesCurrent.setSize(
      static_cast<signed>(global.maximumExtractedRandomBytes)
    );
    // Forget previous random bytes, and gain a little extra entropy.
    Crypto::Random::acquireAdditionalRandomness(millisecondsAtfork);
    // Set death signal of the parent trigger death signal of the child.
    // If the parent process was killed before the prctl executed,
    // this will not work.
    int success = prctl(PR_SET_PDEATHSIG, SIGKILL);
    if (success == - 1) {
      global
      << Logger::red
      << "Failed to set parent death signal. "
      << Logger::endL;
      exit(1);
    }
  }
  if (result < 0) {
    global
    << Logger::red
    << "Negative result while forking: "
    << result
    << "."
    << Logger::endL;
  }
  // if the result is negative, this is an error.
  return result;
}

#else

int WebServer::forkProcess() {
  global.fatal
  << "In forkProcess(): Process forks are not allowed when running under wasm."
  << global.fatal;
  return - 1;
}

int WebServer::forkRaw() {
  global.fatal
  << "In forkRaw(): process forks are not allowed when running under wasm."
  << global.fatal;
  return - 1;
}

#endif
