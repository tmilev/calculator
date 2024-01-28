#include "crypto_calculator.h"
#include "general_logging_global_variables.h"
#include <syscall.h> // <- SYS_getrandom defined here.
#include <unistd.h> // <- syscall defined here.

#ifndef MACRO_use_wasm

#include <linux/random.h> // <- GRND_NONBLOCK defined here.

void Crypto::Random::initializeRandomBytesForTesting() {
  List<unsigned char>& output = global.randomBytesCurrent;
  output.initializeFillInObject(global.numberOfRandomBytes, 0);
}

void Crypto::Random::initializeRandomBytes() {
  static bool alreadyRan = false;
  if (alreadyRan) {
    global.fatal
    << "Random bytes initialization allowed only once. "
    << global.fatal;
  }
  alreadyRan = true;
  if (global.numberOfRandomBytes > 256) {
    global.fatal
    << "The number of system random bytes must not exceed 256. "
    << global.fatal;
  }
  List<unsigned char>& output = global.randomBytesCurrent;
  output.setSize(static_cast<signed>(global.numberOfRandomBytes));
  // According to the
  // documentation of getrandom, the following call
  // must not block or return fewer than the requested bytes in Linux
  // when the number of bytes is less than 256.
  int generatedBytes = static_cast<int>(
    syscall(
      SYS_getrandom,
      output.objects,
      static_cast<unsigned>(output.size),
      GRND_NONBLOCK
    ) // Does not compile on older linux systems:
    // getrandom(output.objects, static_cast<unsigned>(output.size), 0)
  );
  if (generatedBytes != output.size) {
    global.fatal
    << "Failed to get the necessary number of random bytes. "
    << global.fatal;
  }
}

#else

#include <sys/random.h> // <- GRND_NONBLOCK defined here.

void Crypto::Random::initializeRandomBytes() {
  static bool alreadyRan = false;
  if (alreadyRan) {
    global.fatal
    << "Random bytes initialization allowed only once. "
    << global.fatal;
  }
  alreadyRan = true;
  if (global.numberOfRandomBytes > 256) {
    global.fatal
    << "The number of system random bytes must not exceed 256. "
    << global.fatal;
  }
  List<unsigned char>& output = global.randomBytesCurrent;
  output.setSize(static_cast<signed>(global.numberOfRandomBytes));
  // According to the
  // documentation of getrandom, the following call
  // must not block or return fewer than the requested bytes in Linux
  // when the number of bytes is less than 256.
  int generatedBytes = static_cast<int>(
    getentropy(output.objects, static_cast<unsigned>(output.size))
  );
  if (generatedBytes != output.size) {
    global.fatal
    << "Failed to get the necessary number of random bytes. "
    << global.fatal;
  }
}

#endif
