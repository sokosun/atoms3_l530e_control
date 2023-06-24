// L530EControl class controls TP-Link L530E Smart bulb.
// This class could spend much time because of blocking communication.
//
// Usage:
// 1. Create an instance
//   L530EControl l530e(IPAddress(192,168,xxx,xxx), "username@mail.com", "password");
// 2. Handshake and login to create a session
//   l530e.Handshake();
//   l530e.Login();
// 3. Call APIs
//   l530e.SetDeviceState(true);
// 4. Reconnect
// // Please check validity of the session if an application keeps connection long time (>24min.)
//   if(!l530e.HasActiveSession()){
//     l530e.Handshake();
//     l530e.Login();
//   }
//
// Communication Sequence:
//   L530E     Client
//    | handshake |
//    |<----------| ; key
//    |---------->| ; key, sessionID and timeout
//    |           |
//    |   login   |
//    |<----------| ; username(sha1), password and sessionID
//    |---------->| ; token
//    |           |
//    | API calls |
//    |<----------| ; token
//    |---------->|
//    |    ...    |

#include "HTTPClient.h"
#include "mbedtls/cipher.h"

class L530EControl {

public:
  L530EControl() = delete;
  L530EControl(const IPAddress l530e_ip, const String username, const String password);
  ~L530EControl();

  struct HSB {
    int hue         = -1; // 0 to 360
    int saturation  = -1; // 0 to 360
    int brightness  = -1; // 0 to 100

    HSB(int hue_in = -1, int saturation_in = -1, int brightness_in = -1){
      hue         = hue_in;
      saturation  = saturation_in;
      brightness  = brightness_in;
    };
  };

  struct WhiteLight {
    int temperature = -1; // 2500 to 6500
    int brightness  = -1; // 0 to 100

    WhiteLight(int temperature_in = -1, int brightness_in = -1){
      brightness  = brightness_in;
      temperature = temperature_in;
    };
  };

  bool HasActiveSession();

  /// @return true if succeeded
  bool Handshake();

  /// @return true if succeeded
  bool Login();

  /// @return true if succeeded
  bool SetDeviceInfo(bool on, const HSB & hsb);
  bool SetDeviceInfo(bool on, const WhiteLight & wl = WhiteLight());

  /// @return DeviceInfo json formatted
  String GetDeviceInfo();

private:
  struct httpResponse{
    String    header_set_cookie;
    String    body;
  };

  struct SessionInfo {
    String    id;
    String    token;
    uint32_t  timeout_ms = 1440 * 1000;
    uint32_t  start_ms = 0;
  };
  
  struct LoginInfo {
    IPAddress ip;
    String    username;
    String    password;
  };

  struct CipherInfo {
    uint8_t   key[16];
    uint8_t   iv[16];
  };

  CipherInfo ParseKey(const String &key);
  size_t Cipher(mbedtls_operation_t operation, const uint8_t *input, size_t ilen, uint8_t *output);

  // length of str must be 1K or less.
  String Encrypt(const String &str);
  String Decrypt(const String &str);
  httpResponse Request(const String &url, const String &payload);

private:
  // 'm_login' holds parameters to login to P105
  // username and password are not plain text.
  LoginInfo m_login;

  // 'm_session' holds parameters to keep a session with P105
  SessionInfo m_session;

  // 'm_cipher' holds parameters to encrypt/decrypt strings
  CipherInfo m_cipher;

};
