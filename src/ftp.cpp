#include "ftp.h"

#include "LittleFS.h"

#include "config.h"
#include "constants.h"

arduino_temp::FTP::FTP(IPAddress ftpserverip, uint16_t ftpport, boolean debug)
    : filehandler_(LOG_ENABLED) {
  debug_ = debug;
  ftpserver_ = ftpserverip;
  ftpport_ = ftpport;
}

byte arduino_temp::FTP::ftpUploadFile(String filename) {
  if (ftpclient_.connect(ftpserver_, ftpport_)) {
    Serial.println(F("Command connected"));
  } else {
    Serial.println(F("Command connection failed"));
    return 0;
  }
  if (!eRcv()) return 0;
  if (debug_) Serial.println("Send USER");
  ftpclient_.println("USER " + FTP_USERNAME);

  if (!eRcv()) return 0;
  if (debug_) Serial.println("Send PASSWORD");
  ftpclient_.println("PASS " + FTP_PASSWORD);

  if (!eRcv()) return 0;
  if (debug_) Serial.println("Send SYST");
  ftpclient_.println(F("SYST"));

  if (!eRcv()) return 0;
  if (debug_) Serial.println("Send Type I");
  ftpclient_.println(F("Type I"));

  if (!eRcv()) return 0;
  if (debug_) Serial.println("Send PASV");
  ftpclient_.println(F("PASV"));

  if (debug_) Serial.println("Send PASV2");
  if (!eRcv()) return 0;
  if (debug_) Serial.println("Send PASV3");

  char *tStr = strtok(ftpoutBuf_, "(,");
  int array_pasv[6];
  for (int i = 0; i < 6; i++) {
    tStr = strtok(NULL, "(,");
    array_pasv[i] = atoi(tStr);
    if (tStr == NULL) {
      Serial.println(F("Bad PASV Answer"));
    }
  }
  unsigned int hiPort, loPort;
  hiPort = array_pasv[4] << 8;
  loPort = array_pasv[5] & 255;

  if (debug_) Serial.print(F("Data port: "));
  hiPort = hiPort | loPort;
  if (debug_) Serial.println(hiPort);

  if (ftpdclient_.connect(ftpserver_, hiPort)) {
    Serial.println(F("Data connected"));
  } else {
    Serial.println(F("Data connection failed"));
    ftpclient_.stop();
    return 0;
  }

  if (debug_) Serial.println("Send STOR filename");
  ftpclient_.print(F("STOR "));
  ftpclient_.println(filename);

  if (!eRcv()) {
    ftpdclient_.stop();
    return 0;
  }

  if (debug_) Serial.println(F("Writing"));
#define bufSizeFTP 1460
  uint8_t clientBuf[bufSizeFTP];
  size_t clientCount = 0;

  File fh = LittleFS.open(filename, "r");
  while (fh.available()) {
    clientBuf[clientCount] = fh.read();
    clientCount++;
    if (clientCount > (bufSizeFTP - 1)) {
      ftpdclient_.write((const uint8_t *)&clientBuf[0], bufSizeFTP);
      clientCount = 0;
      delay(1);
    }
  }
  if (clientCount > 0)
    ftpdclient_.write((const uint8_t *)&clientBuf[0], clientCount);
  fh.close();

  ftpdclient_.stop();
  Serial.println(F("Data disconnected"));

  if (!eRcv()) return 0;

  ftpclient_.println(F("QUIT"));

  if (!eRcv()) return 0;

  ftpclient_.stop();
  if (debug_) Serial.println(F("SPIFS closed"));
  return 1;
}

// FTP error handler
void arduino_temp::FTP::efail() {
  byte thisByte = 0;

  ftpclient_.println(F("QUIT"));

  while (!ftpclient_.available()) delay(1);

  while (ftpclient_.available()) {
    thisByte = ftpclient_.read();
    Serial.write(thisByte);
  }

  ftpclient_.stop();
  Serial.println(F("Command disconnected"));
}

// receive from FTP and put into ftpoutBuf_.
// returns the read byte
byte arduino_temp::FTP::eRcv() {
  byte respCode;
  byte thisByte;

  while (!ftpclient_.available()) delay(1);

  respCode = ftpclient_.peek();

  unsigned int ftpoutCount = 0;

  while (ftpclient_.available()) {
    thisByte = ftpclient_.read();
    Serial.write(thisByte);

    if (ftpoutCount < 127) {
      ftpoutBuf_[ftpoutCount] = thisByte;
      ftpoutCount++;
      ftpoutBuf_[ftpoutCount] = 0;
    }
  }

  if (respCode >= '4') {
    efail();
    return 0;
  }
  return 1;
}
