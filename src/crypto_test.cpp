#include "crypto.h"

static ProjectInformationInstance projectInfoCryptoFile2(__FILE__, "Tests for crypto functions.");

bool Crypto::Test::All() {
  Crypto::Test::Sha256();
  return true;
}

bool Crypto::Test::Sha256() {
  List<std::string> inputs;
  List<std::string> outputs;
  inputs.AddOnTop("abc");
  outputs.AddOnTop("ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 96177a9c b410ff61 f20015ad");
  inputs.AddOnTop("");
  outputs.AddOnTop("e3b0c442 98fc1c14 9afbf4c8 996fb924 27ae41e4 649b934c a495991b 7852b855");
  inputs.AddOnTop("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
  outputs.AddOnTop("248d6a61 d20638b8 e5c02693 0c3e6039 a33ce459 64ff2167 f6ecedd4 19db06c1");
  inputs.AddOnTop("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu");
  outputs.AddOnTop("cf5b16a7 78af8380 036ce59e 7b049237 0b249b11 e8f07a51 afac4503 7afee9d1");
  std::stringstream commentsOnFailure;
  for (int i = 0; i < inputs.size; i ++) {
    std::string result, desiredResult;
    Crypto::computeSha256(inputs[i], result);
    if (!Crypto::ConvertHexToString(outputs[i], desiredResult, &commentsOnFailure)) {
      crash << "Conversion of hard-coded hex string failed. " << crash;
    }
    if (result != desiredResult) {
      crash
      << "Hash of:\n" << inputs[i] << "\nresulted in:\n"
      << Crypto::ConvertStringToHex(result, 50, false)
      << "\ninstead of the desired:\n" << outputs[i] << crash;
    }
  }
  return true;
}
