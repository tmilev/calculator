// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#include "transport_layer_security.h"
#include "general_logging_global_variables.h"
#include "general_file_operations_encodings.h"
#include "crypto.h"
#include "abstract_syntax_notation_one_decoder.h"

static ProjectInformationInstance projectInfoInstanceTransportLayerSecurityTest(__FILE__, "TSL/ssl implementation.");

std::string SSLRecord::Test::sampleClientHelloHex =
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

bool SSLRecord::Test::SerializationClientHello(TransportLayerSecurityServer& testServer) {
  MacroRegisterFunctionWithName("SSLRecord::Test::SerializationClientHello");
  std::string inputHex = SSLRecord::Test::sampleClientHelloHex;
  SSLRecord theRecord;
  theRecord.owner = &testServer;
  std::stringstream comments;
  if (!Crypto::ConvertHexToListUnsignedChar(
    inputHex, theRecord.incomingBytes, &comments
  )) {
    global.fatal << "Bad hard-coded test hex string!" << global.fatal;
  }
  if (!theRecord.Decode(&comments)) {
    global.fatal << "Failed to decode built-in message. " << comments.str() << global.fatal;
  }
  List<unsigned char> encoded;
  theRecord.WriteBytes(encoded, nullptr);
  if (encoded != theRecord.incomingBytes) {
    global << "Decoded:\n" << theRecord.content.getStringHighlighter()
    << Crypto::ConvertListUnsignedCharsToHex(theRecord.incomingBytes) << logger::endL;
    global << "Encoded:\n" << theRecord.content.getStringHighlighter()
    << Crypto::ConvertListUnsignedCharsToHex(encoded) << logger::endL;
    global.fatal << "Decode->Encode did not reproduce the original input. " << global.fatal;
  }
  return true;
}

bool SSLRecord::Test::Serialization() {
  MacroRegisterFunctionWithName("SSLRecord::Test::Serialization");

  TransportLayerSecurityServer server;
  server.initialize();
  server.session.initialize();
  if (!SSLRecord::Test::SerializationClientHello(server)) {
    global.fatal << "Failed serialization of client hello. " << global.fatal;
  }
  return true;
}
