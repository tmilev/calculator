// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "webserver.h"
#include <unistd.h>
#include "crypto.h"
// sys/random.h is missing from older distros/kernels.
// #include <sys/random.h>
// Used in place of sys/random.h:
#include <syscall.h> // <- SYS_getrandom defined here.
#include <linux/random.h> // <- GRND_NONBLOCK defined here.
#include <unistd.h> // <- syscall defined here.
#include <sys/prctl.h> //<- prctl here
#include <signal.h> // <-signals here

static ProjectInformationInstance projectInfoInstanceWebServer(__FILE__, "Web server fork.");
extern WebServer theWebServer;

void WebServer::initializeRandomBytes() {
  static bool alreadyRan = false;
  if (alreadyRan) {
    global.fatal << "Random bytes initialization allowed only once. " << global.fatal;
  }
  if (global.numberOfRandomBytes > 256) {
    global.fatal << "The number of system random bytes must not exceed 256. " << global.fatal;
  }
  List<unsigned char>& output = global.randomBytesCurrent;
  output.SetSize(static_cast<signed>(global.numberOfRandomBytes));
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
    global.fatal << "Failed to get the necessary number of random bytes. " << global.fatal;
  }
}

bool WebServer::CreateProcessMutex() {
  WebWorker& worker = this->GetActiveWorker();
  if (!worker.PauseWorker.CreateMe(this->ToStringWorkerToWorker() + "pause mutex", false, true)) {
    global << "Failed to create process mutex: "
    << worker.PauseWorker.name << "\n";
    return false;
  }
  if (!worker.writingReportFile.CreateMe(this->ToStringWorkerToWorker() + "output file mutex", false, true)) {
    global << "Failed to create process mutex: "
    << worker.writingReportFile.name << "\n";
    return false;
  }
  return true;
}

void WebServer::ComputeActiveWorkerId() {
  List<unsigned char> incomingId;
  Crypto::GetRandomBytesSecureInternalMayLeaveTracesInMemory(incomingId, 32);
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
    Crypto::acquireAdditionalRandomness(millisecondsAtFork);
  } else if (result == 0) {
    global.logs.logType = GlobalVariables::LogData::type::worker;

    // child process
    // lose 256 bits of entropy from the server:
    global.randomBytesCurrent.SetSize(static_cast<signed>(global.maximumExtractedRandomBytes));
    // Forget previous random bytes, and gain a little extra entropy:
    Crypto::acquireAdditionalRandomness(millisecondsAtFork);

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
