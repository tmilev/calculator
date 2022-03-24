#include "crypto.h"

bool Crypto::Test::all() {
  Crypto::Test::sha256();
  return true;
}

bool Crypto::Test::sha256() {
  List<std::string> inputs;
  List<std::string> outputs;
  inputs.addOnTop("abc");
  outputs.addOnTop(
    "ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 96177a9c b410ff61 f20015ad"
  );
  inputs.addOnTop("");
  outputs.addOnTop(
    "e3b0c442 98fc1c14 9afbf4c8 996fb924 27ae41e4 649b934c a495991b 7852b855"
  );
  inputs.addOnTop("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
  outputs.addOnTop(
    "248d6a61 d20638b8 e5c02693 0c3e6039 a33ce459 64ff2167 f6ecedd4 19db06c1"
  );
  inputs.addOnTop(
    "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghij"
    "klmnhijklmnoijklmnopj"
    "klmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"
  );
  outputs.addOnTop(
    "cf5b16a7 78af8380 036ce59e 7b049237 0b249b11 e8f07a51 afac4503 7afee9d1"
  );
  std::stringstream commentsOnFailure;
  for (int i = 0; i < inputs.size; i ++) {
    std::string result, desiredResult;
    Crypto::computeSha256(inputs[i], result);
    if (
      !Crypto::convertHexToString(
        outputs[i], desiredResult, &commentsOnFailure
      )
    ) {
      global.fatal
      << "Conversion of hard-coded hex string failed. "
      << global.fatal;
    }
    if (result != desiredResult) {
      global.fatal
      << "Hash of:\n"
      << inputs[i]
      << "\nresulted in:\n"
      << Crypto::convertStringToHex(result, 50, false)
      << "\ninstead of the desired:\n"
      << outputs[i]
      << global.fatal;
    }
  }
  return true;
}
