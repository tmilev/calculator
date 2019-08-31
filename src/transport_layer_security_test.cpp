//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".

#include "transport_layer_security.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"
#include "vpfHeader1General7FileOperations_Encodings.h"
#include "vpfHeader5Crypto.h"
#include "abstract_syntax_notation_one_decoder.h"

extern ProjectInformationInstance projectInfoInstanceTransportLayerSecurityTest;
ProjectInformationInstance projectInfoInstanceTransportLayerSecurityTest(__FILE__, "TSL/ssl implementation.");

extern logger logServer;

bool SSLRecord::TestSerialization() {
  MacroRegisterFunctionWithName("SSLRecord::TestSerialization");
  std::string inputHex =
  "1603010200010001fc03031e70e7e6d1ccf06a234cf5b6f4c207609d824787e70a63808cacbe5fb2b67ea1"
  "202ae1460ab9498ccc0b624b4eb0f859a840d98724873a214a64c8b0b09c431cc6"
  "00228a8a130113021303c02bc02fc02cc030cca9cca8c013c014009c009d002f0035000a"
  "01000191"
  "2a2a00000000000e000c0000096c6f63616c686f737400170000ff01000100000a000a"
  "0008baba001d00170018000b00020100002300000010000e000c02683208687474702f312e31000500050100000000000d"
  "00140012040308040401050308050501080606010201001200000033"
  "002b0029baba000100001d002009a372f41610b1daa123c42f7042e09c457dd6af0f195df6b1813ba93b154a24002d00020101"
  "002b000b0aaaaa0304030303020301001b00030200029a9a000100001500cf"
  "000000000000000000000000000000000000000000000000000000000000000"
  "000000000000000000000000000000000000000000000000000000000000000"
  "000000000000000000000000000000000000000000000000000000000000000"
  "000000000000000000000000000000000000000000000000000000000000000"
  "000000000000000000000000000000000000000000000000000000000000000"
  "000000000000000000000000000000000000000000000000000000000000000"
  "000000000000000000000000000000000000";
  SSLRecord theRecord;
  std::stringstream comments;
  if (!Crypto::ConvertHexToListUnsignedChar(inputHex, theRecord.body, &comments)) {
    crash << "Bad hard-coded test hex string!" << crash;
  }
  logServer << "DEBUG: got to ehre!! " << logger::endL;

  if (!theRecord.Decode(&comments)) {
    crash << "Failed to decode built-in message." << crash;
  }
  logServer << "DEBUG: got to ehre 2!! " << logger::endL;
  List<unsigned char> encoded;
  theRecord.WriteBytes(encoded);
  if (encoded != theRecord.body) {
    logServer << "Decoded:\n" << theRecord.hello.getStringHighlighter()
    << Crypto::ConvertListUnsignedCharsToHex(theRecord.body, 0, false) << logger::endL;
    logServer << "Encoded:\n" << theRecord.hello.getStringHighlighter()
    << Crypto::ConvertListUnsignedCharsToHex(encoded, 0, false) << logger::endL;
    crash << "Decode->Encode did not reproduce the original input. " << crash;
  }
  return true;
}
