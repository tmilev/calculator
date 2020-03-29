// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "webserver.h"
#include <unistd.h>
#include <sys/prctl.h> //<- prctl here
#include <signal.h> // <-signals here

void WebServer::initializeRandomBytes() {
  Crypto::Random::initializeRandomBytes();
}

bool WebServer::CreateProcessMutex() {
  WebWorker& worker = this->GetActiveWorker();
  if (!worker.PauseWorker.CreateMe(this->ToStringWorkerToWorker() + "pause mutex", true)) {
    global << "Failed to create process mutex: "
    << worker.PauseWorker.name << "\n";
    return false;
  }
  if (!worker.writingReportFile.CreateMe(this->ToStringWorkerToWorker() + "output file mutex", true)) {
    global << "Failed to create process mutex: "
    << worker.writingReportFile.name << "\n";
    return false;
  }
  return true;
}

void WebServer::ComputeActiveWorkerId() {
  List<unsigned char> incomingId;
  Crypto::Random::GetRandomBytesSecureInternalMayLeaveTracesInMemory(incomingId, 32);
  WebWorker& worker = this->GetActiveWorker();
  if (worker.workerId != "") {
    this->workerIds.RemoveKey(worker.workerId);
  }
  worker.workerId = Crypto::ConvertListUnsignedCharsToHex(incomingId);
  this->workerIds.SetKeyValue(worker.workerId, this->activeWorker);
  if (this->workerIds.size() > 2 * this->theWorkers.size) {
    global << logger::red
    << "Warning: worker ids exceeds twice the number of workers. "
    << "This may be a memory leak. " << logger::endL;
  }
}

int WebServer::Fork() {
  if (!this->CreateProcessMutex()) {
    return - 1;
  }
  this->ComputeActiveWorkerId();
  // timer taken at server level:
  int64_t millisecondsAtFork = global.GetElapsedMilliseconds();
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
    Crypto::Random::acquireAdditionalRandomness(millisecondsAtFork);
  } else if (result == 0) {
    global.logs.logType = GlobalVariables::LogData::type::worker;

    // child process
    // lose 256 bits of entropy from the server:
    global.randomBytesCurrent.SetSize(static_cast<signed>(global.maximumExtractedRandomBytes));
    // Forget previous random bytes, and gain a little extra entropy:
    Crypto::Random::acquireAdditionalRandomness(millisecondsAtFork);

    // Set death signal of the parent trigger death signal of the child.
    // If the parent process was killed before the prctl executed,
    // this will not work.
    int success = prctl(PR_SET_PDEATHSIG, SIGKILL);
    if (success == -1) {
      global << logger::red << "Failed to set parent death signal. " << logger::endL;
      exit(1);
    }
  }
  // if the result is negative, this is an error.
  return result;
}
