byte ftpUploadFile(String filename) {

	if (global_ftpclient.connect(global_ftpserver, 21)) {  // 21 = FTP server
		Serial.println(F("Command connected"));
	} else {
		Serial.println(F("Command connection failed"));
		return 0;
	}
	if (!eRcv())
		return 0;
	if (debug)
		Serial.println("Send USER");
	global_ftpclient.println("USER " + FTP_USERNAME);

	if (!eRcv())
		return 0;
	if (debug)
		Serial.println("Send PASSWORD");
	global_ftpclient.println("PASS " + FTP_PASSWORD);

	if (!eRcv())
		return 0;
	if (debug)
		Serial.println("Send SYST");
	global_ftpclient.println(F("SYST"));

	if (!eRcv())
		return 0;
	if (debug)
		Serial.println("Send Type I");
	global_ftpclient.println(F("Type I"));

	if (!eRcv())
		return 0;
	if (debug)
		Serial.println("Send PASV");
	global_ftpclient.println(F("PASV"));

	if (debug)
		Serial.println("Send PASV2");
	if (!eRcv())
		return 0;
	if (debug)
		Serial.println("Send PASV3");

	char *tStr = strtok(global_ftpoutBuf, "(,");
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

	if (debug)
		Serial.print(F("Data port: "));
	hiPort = hiPort | loPort;
	if (debug)
		Serial.println(hiPort);

	if (global_ftpdclient.connect(global_ftpserver, hiPort)) {
		Serial.println(F("Data connected"));
	} else {
		Serial.println(F("Data connection failed"));
		global_ftpclient.stop();
		return 0;
	}

	if (debug)
		Serial.println("Send STOR filename");
	global_ftpclient.print(F("STOR "));
	global_ftpclient.println(filename);

	if (!eRcv()) {
		global_ftpdclient.stop();
		return 0;
	}

	if (debug)
		Serial.println(F("Writing"));
#define bufSizeFTP 1460
	uint8_t clientBuf[bufSizeFTP];
	size_t clientCount = 0;

	File fh = SPIFFS.open(filename, "r");
	while (fh.available()) {
		clientBuf[clientCount] = fh.read();
		clientCount++;
		if (clientCount > (bufSizeFTP - 1)) {
			global_ftpdclient.write((const uint8_t *) &clientBuf[0],
					bufSizeFTP);
			clientCount = 0;
			delay(1);
		}
	}
	if (clientCount > 0)
		global_ftpdclient.write((const uint8_t *) &clientBuf[0], clientCount);
	fh.close();

	global_ftpdclient.stop();
	Serial.println(F("Data disconnected"));

	if (!eRcv())
		return 0;

	global_ftpclient.println(F("QUIT"));

	if (!eRcv())
		return 0;

	global_ftpclient.stop();
	if (debug)
		Serial.println(F("SPIFS closed"));
	return 1;
}

//----------------- FTP fail
void efail() {
	byte thisByte = 0;

	global_ftpclient.println(F("QUIT"));

	while (!global_ftpclient.available())
		delay(1);

	while (global_ftpclient.available()) {
		thisByte = global_ftpclient.read();
		Serial.write(thisByte);
	}

	global_ftpclient.stop();
	Serial.println(F("Command disconnected"));
}  // efail

//-------------- FTP receive
byte eRcv() {
	byte respCode;
	byte thisByte;

	while (!global_ftpclient.available())
		delay(1);

	respCode = global_ftpclient.peek();

	global_ftpoutCount = 0;

	while (global_ftpclient.available()) {
		thisByte = global_ftpclient.read();
		Serial.write(thisByte);

		if (global_ftpoutCount < 127) {
			global_ftpoutBuf[global_ftpoutCount] = thisByte;
			global_ftpoutCount++;
			global_ftpoutBuf[global_ftpoutCount] = 0;
		}
	}

	if (respCode >= '4') {
		efail();
		return 0;
	}
	return 1;
}  // eRcv()
