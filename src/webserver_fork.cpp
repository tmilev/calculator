// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "webserver.h"
#include <unistd.h>
#include <sys/prctl.h> //<- prctl here
#include <signal.h> // <-signals here

void WebServer::initializeRandomBytes() {
  Crypto::Random::initializeRandomBytes();
}

bool WebServer::createProcessMutex() {
  WebWorker& worker = this->getActiveWorker();
  if (!worker.pauseWorker.createMe(this->toStringWorkerToWorker() + "pause mutex", true)) {
    global << "Failed to create process mutex: "
    << worker.pauseWorker.name << "\n";
    return false;
  }
  if (!worker.writingReportFile.createMe(this->toStringWorkerToWorker() + "output file mutex", true)) {
    global << "Failed to create process mutex: "
    << worker.writingReportFile.name << "\n";
    return false;
  }
  return true;
}

void WebServer::computeActiveWorkerId() {
  List<unsigned char> incomingId;
  Crypto::Random::getRandomBytesSecureInternalMayLeaveTracesInMemory(incomingId, 32);
  WebWorker& worker = this->getActiveWorker();
  if (worker.workerId != "") {
    this->workerIds.removeKey(worker.workerId);
  }
  worker.workerId = Crypto::convertListUnsignedCharsToHex(incomingId);
  this->workerIds.setKeyValue(worker.workerId, this->activeWorker);
  if (this->workerIds.size() > 2 * this->theWorkers.size) {
    global << Logger::red
    << "Warning: worker ids exceeds twice the number of workers. "
    << "This may be a memory leak. " << Logger::endL;
  }
}

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
    global.randomBytesCurrent.setSize(static_cast<signed>(global.maximumExtractedRandomBytes));
    // Forget previous random bytes, and gain a little extra entropy.
    Crypto::Random::acquireAdditionalRandomness(millisecondsAtfork);

    // Set death signal of the parent trigger death signal of the child.
    // If the parent process was killed before the prctl executed,
    // this will not work.
    int success = prctl(PR_SET_PDEATHSIG, SIGKILL);
    if (success == -1) {
      global << Logger::red << "Failed to set parent death signal. " << Logger::endL;
      exit(1);
    }
  }
  if (result < 0) {
    global << Logger::red
    << "Negative result while forking: " << result << "." << Logger::endL;
  }
  // if the result is negative, this is an error.
  return result;
}
