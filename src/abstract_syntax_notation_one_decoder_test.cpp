// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#include "abstract_syntax_notation_one_decoder.h"
#include "crypto.h"
#include "math_extra_algebraic_numbers.h"

bool PrivateKeyRSA::Test::all() {
  PrivateKeyRSA::Test::loadFromPEMFile();
  PrivateKeyRSA::Test::loadFromPEM();
  return true;
}

bool PrivateKeyRSA::Test::loadFromPEMFile() {
  std::string fileName = "test/private_key.pem";
  PrivateKeyRSA key;
  std::stringstream commentsOnFailure;
  if (!key.loadFromPEMFile(fileName, &commentsOnFailure)) {
    global.fatal << "Failed to load pem file. " << commentsOnFailure.str() << global.fatal;
  }
  return true;
}

bool PrivateKeyRSA::Test::loadFromPEM() {
  std::string pemContent =
  "-----BEGIN PRIVATE KEY-----\n"
  "MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDvlOX7G1JZ/TxJ\n"
  "/gPTl+VNT8j2SwpJ3f/Il2CFk6sWHuSEhnErchl5xcC4EdLI+KkJvgp/CDIDAfie\n"
  "dS6uEaUzu0TAjJXSTmngQQuibyfUSqZFy5cBRZJkdzBF06b1kaieVZjp665lTp+r\n"
  "+33Fu6a32pyRJB509UjSxSokkYzfCgzJdKF6UtQ6sXiDLcZYafGCkuAXoE47KDGX\n"
  "XU4+rlq51OxRQ1t3kq0G4vrVYWIW5iYebRVCml2iIBpFZEu5Kf/hqoddTFVQMME3\n"
  "clKEUJtWIe0la6DgnKXjyHIuPRZvMnX5t8Tli3/UATfj+gBiw52wZVPpekedsxV7\n"
  "EGLgjy8ZAgMBAAECggEAVhR+qUFAznpxPfiZcV4DEksXXn7JvI6aB310Pao372O8\n"
  "IW2Ync6KHrmRq9EjlffZ7FkutkPAjFoZ9P0Pf5VxcOvZIQGiFwT7f4p/XqZnfhww\n"
  "L6WdpORVW+mDG6OXOmZSWklQ/sMiTt33+rgKkZABr89JL9D1bHpw5uHWaXsuTThC\n"
  "LL5iFaz3R8GOTA0y6P6t0LhaDOFVqnDPKiKWFgicKrM72darY6noBKIiQlaJ21Zy\n"
  "fxYZb3adQnkHH0dFvhlqODZNlkhnAaUljxqHQRYriWiuoG8lEQq1yyODBKOicJBs\n"
  "b5MG0Gr8ITliVOgHTVsdabrufAQwDYt+WPIOSJcSfQKBgQD8/K90hsWo8wc/WdTG\n"
  "+KCqGxaxq9GAfml8k5GbjjroFbyxLLI7xzbz1i3UU7GFtmdk1yAoEAYo+dgJ3dOQ\n"
  "GdwAAbLvFsDXw9mWrwUfAB5mvln0BMQolNeJlluo3qXCXrWQt68IgaI3v7jI/ukQ\n"
  "Qkw0f76Fl10RYJN2jy7D/w60zwKBgQDyb1eWxw1kdUj8E6RZ1kAqOERH2RDjSawA\n"
  "OiAKIshrkibWOrUNh3zafCVZbOFhTwVOnjFh5qj8H2bIEhYeo6bXS1I8XodCFBtW\n"
  "BIcuXgf+ei9+p5ZlR288RHPMCcEFakTwIT1RpTiFS+XaE9J6EKHpXDjpJGmNrFvM\n"
  "Owj5bPknlwKBgQDdXTklbDwOXkDsNO1TAQCMd9F+u4pAtdwVjoZtJ6Hxq/uEyEhY\n"
  "uDMkXzeQv37pMjb8bKSNiZ/qQNvBnmFdiNMRCmuIe09YPN3q1NBVVORFILBTLcx6\n"
  "eeqzUGZ5uAAwuHCE5cT5W30W7OKCtVI30NsfYNvTxyM/blL5/cV86HP0zwKBgB52\n"
  "hTAKBRHJdwlNwooTVO65MUNB8FBPEpDYvOvpIPv7Wmvy5oszqimeDedluhBLKKSP\n"
  "Vi8yF6vOmlCrjSH+eFl3Onchfad89U5itUXLRP/iBJPKpShGyKBvDYnm890lVFJE\n"
  "KvtWYnrFZbV4Zis2adHz56bMD4Ng1AweousMmWYzAoGAOwdmiIdMzNga6g1MOnOt\n"
  "OGHS4h9r7EZyDcaXgOfLSG79hJApxFyMJ6xztEGQv6faplaanXYhxmj5iz+G0Ddk\n"
  "exoRjqJPEb58+k18LOMio0bqgUi21ZPEGs+r/zq0Mis2XFTTjJm2COyCnD0c+mna\n"
  "gujWsxdtYAoSSPb1ZsJ423Y=\n"
  "-----END PRIVATE KEY-----";
  PrivateKeyRSA key;
  std::stringstream commentsOnFailure;
  if (!key.loadFromPEM(pemContent, &commentsOnFailure)) {
    global.fatal << "Failed to load pem content. " << global.fatal;
  }
  if (!key.basicChecks(&commentsOnFailure)) {
    global.fatal << "Hard coded private key did not pass basic checks. " << global.fatal;
  }
  return true;
}

bool X509Certificate::Test::all() {
  X509Certificate::Test::loadFromPEMFile();
  X509Certificate::Test::loadFromPEM();
  return true;
}

bool X509Certificate::Test::loadFromPEM() {
  std::string pemContent =
  "-----BEGIN CERTIFICATE-----"
  "MIID5TCCAs2gAwIBAgIUOFXi72uftmcTij+B7sbVOUdkOlcwDQYJKoZIhvcNAQEL"
  "BQAwgYExCzAJBgNVBAYTAkNBMRMwEQYDVQQIDApTb21lLVN0YXRlMSEwHwYDVQQK"
  "DBhJbnRlcm5ldCBXaWRnaXRzIFB0eSBMdGQxFDASBgNVBAMMC1RvZG9yIE1pbGV2"
  "MSQwIgYJKoZIhvcNAQkBFhV0b2Rvci5taWxldkBnbWFpbC5jb20wHhcNMTkwNzEy"
  "MjIxMzUzWhcNMjcwOTI5MjIxMzUzWjCBgTELMAkGA1UEBhMCQ0ExEzARBgNVBAgM"
  "ClNvbWUtU3RhdGUxITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDEU"
  "MBIGA1UEAwwLVG9kb3IgTWlsZXYxJDAiBgkqhkiG9w0BCQEWFXRvZG9yLm1pbGV2"
  "QGdtYWlsLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAO+U5fsb"
  "Uln9PEn+A9OX5U1PyPZLCknd/8iXYIWTqxYe5ISGcStyGXnFwLgR0sj4qQm+Cn8I"
  "MgMB+J51Lq4RpTO7RMCMldJOaeBBC6JvJ9RKpkXLlwFFkmR3MEXTpvWRqJ5VmOnr"
  "rmVOn6v7fcW7prfanJEkHnT1SNLFKiSRjN8KDMl0oXpS1DqxeIMtxlhp8YKS4Beg"
  "TjsoMZddTj6uWrnU7FFDW3eSrQbi+tVhYhbmJh5tFUKaXaIgGkVkS7kp/+Gqh11M"
  "VVAwwTdyUoRQm1Yh7SVroOCcpePIci49Fm8ydfm3xOWLf9QBN+P6AGLDnbBlU+l6"
  "R52zFXsQYuCPLxkCAwEAAaNTMFEwHQYDVR0OBBYEFMj1++vneAHzaYZy/ryDpTxQ"
  "DVDjMB8GA1UdIwQYMBaAFMj1++vneAHzaYZy/ryDpTxQDVDjMA8GA1UdEwEB/wQF"
  "MAMBAf8wDQYJKoZIhvcNAQELBQADggEBALh+D6HBP3JF5GE8ZKZ+U6RdggN5y1Hi"
  "xI1kWZUQ8Ywfs9NBKRLdMCa+7m9h9wiZmpPN0Tiv2lwQ1DJFWEqcE75q/L0TyCbN"
  "shgt5Hi6Cq+nql5DHvkZI02sLPHwTBfhOI1vu7nnxobDt0yEDgVIQp9zJiMCg7V0"
  "S4x13CYfn/6U0cDnSVmeZuw4CBfbl5UzC2F8OhVdbGMw1mKFmh8Z+2ufu6IIgjUe"
  "a6vAWelqRJ2bjvDMqPIU58gdV1hYO/wyw9i7uSXK51pD44HEsEWk4QMXs/85zjS3"
  "NQagG+jjGFKZwWBjMo/FIkJhYRGSoavnjZjax/Jz6MEktt/TAEJC6x4="
  "-----END CERTIFICATE-----"
  ;
  std::stringstream commentsOnFailure;
  X509Certificate certificate;
  if (!certificate.loadFromPEM(pemContent, &commentsOnFailure)) {
    global.fatal << "Failed to load built-in test certificate content. " << global.fatal;
  }
  return true;
}

bool X509Certificate::Test::loadFromPEMFile() {
  std::string fileName = "test/certificate_self_signed.pem";
  std::stringstream commentsOnFailure;
  X509Certificate certificate;
  if (!certificate.loadFromPEMFile(fileName, &commentsOnFailure)) {
    global.fatal << "Failed to load built-in test certificate file. "
    << commentsOnFailure.str() << global.fatal;
  }
  return true;
}
