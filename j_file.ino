// List the content of the directory.
// parameters:
//   directory ... the directory to list
// returns nothing
void listDirectory(String directory) {

	Serial.println("all files in " + directory + " :");

	Dir dir = SPIFFS.openDir(directory);
	while (dir.next()) {
		Serial.print(dir.fileName());
		Serial.print(" size: ");
		File f = dir.openFile("r");
		Serial.println(f.size());
	}
}

// Get the name for the log file backup.
// parameters:
//   logfileName ... base name of the log files
//   num ... number of the log file
// returns the name of the log file
String getLogfileBackupName(String logfileName, int num) {
	return logfileName + "." + String(num);
}

// Rotate the log files.
// parameters:
//   logfileName ... base name for log files
//   maxBackup ... maximum backups
// returns nothing
void rotateLogfiles(String logfileName, int maxBackup) {

	Serial.println("rotating log files: " + logfileName);

	// remove last file
	String lastBackupName = getLogfileBackupName(logfileName, maxBackup);
	if (SPIFFS.exists(lastBackupName)) {
		SPIFFS.remove(lastBackupName);
	}
	// rotate all backup files
	for (int i = maxBackup - 1; i >= 0; i--) {
		String currentBackupName = getLogfileBackupName(logfileName, i);
		if (SPIFFS.exists(currentBackupName)) {
			String nextBackupName = getLogfileBackupName(logfileName, i + 1);
			SPIFFS.rename(currentBackupName, nextBackupName);
		}
	}
	// rotate current logfile to first backup file
	if (SPIFFS.exists(logfileName)) {
		String firstBackupName = getLogfileBackupName(logfileName, 0);
		SPIFFS.rename(logfileName, firstBackupName);
	}

	// ready to log in log file
	global_log_enabled = true;
	// dump the current log buffer to the log file
	dumpLogBuffer();
}

// Prints all log files.
// parameters:
//   logfileName ... base name for log files
// maxBackup ... maximum backups
// returns nothing
void printLogfiles(String logfileName, int maxBackup) {

	Serial.println("printing log files: " + logfileName);

	// print current logfile
	if (SPIFFS.exists(logfileName)) {
		printFileToSerial(logfileName);
	}

	// print last good logfile
	if (SPIFFS.exists(logfileName + LOGFILE_LASTGOOD_EXTENSION)) {
		printFileToSerial(logfileName + LOGFILE_LASTGOOD_EXTENSION);
	}

	// print last bad logfile
	if (SPIFFS.exists(logfileName + LOGFILE_LASTBAD_EXTENSION)) {
		printFileToSerial(logfileName + LOGFILE_LASTBAD_EXTENSION);
	}

	// print all backup files
	for (int i = 0; i <= maxBackup; i++) {
		String currentBackupName = getLogfileBackupName(logfileName, i);
		if (SPIFFS.exists(currentBackupName)) {
			printFileToSerial(currentBackupName);
		}
	}
}

// Deletes all log files.
// parameters:
//   directory ... the directory to delete the logfiles in
//   logfileName ... base name for log files
// returns nothing
void deleteLogfiles(String directory, String logfileName) {

	Serial.println(
			"deleting in " + directory + " the log files: " + logfileName);

	Dir dir = SPIFFS.openDir(directory);
	while (dir.next()) {
		String filename = dir.fileName();
		if (filename.indexOf(logfileName) > -1) {
			Serial.println("  deleting: " + filename);
			SPIFFS.remove(filename);
		}
	}
}

// Writes message with logLevel to serial and log file if global_log_enabled is true.
// If global_log_enabled is false, the content is written to an internal log buffer.
// Use dumpLogBuffer() to write the content to the log file.
// parameters:
//   logLevel ... log level
//   message ... message to log
// returns nothing
void doLog(String logLevel, String message) {

	String logLine = getDateTime() + " " + String(millis()) + " " + logLevel
			+ ": " + message;
	Serial.println(logLine);

	if (global_log_enabled) {
		writeLog(global_logfileName, logLine);
	} else {
		if (global_logBuffer == "") {
			global_logBuffer += logLine;
		} else {
			global_logBuffer += "\n" + logLine;
		}
	}

}


// Logs the message with LOGLEVEL_INFO.
// parameters:
//   message ... the message to log
// returns nothing
void doLogInfo(String message) {
	doLog(LOGLEVEL_INFO, message);
}


// Logs the message with LOGLEVEL_WARN.
// parameters:
//   message ... the message to log
// returns nothing
void doLogWarn(String message) {
	doLog(LOGLEVEL_WARN, message);
}


// Logs the message with LOGLEVEL_ERROR.
// parameters:
//   message ... the message to log
// returns nothing
void doLogError(String message) {
	doLog(LOGLEVEL_ERROR, message);
}


// Logs the message with LOGLEVEL_TIME.
// parameters:
//   message ... the message to log
// returns nothing
void doLogTime(String message) {
	doLog(LOGLEVEL_TIME, message);
}


// Dumps the content of the log buffer to the log file.
// parameters: none
// returns nothing
void dumpLogBuffer() {
	writeLog(global_logfileName, global_logBuffer);
}


// Write to log file.
// parameters:
//   logfileName ... base name of the log file
//   logmessage ... message to log
// returns nothing
void writeLog(String logfileName, String logmessage) {

	//open file for appending new blank line to EOF.
	File f = SPIFFS.open(logfileName, "a");
	f.print(logmessage);
	f.print('\n');
	f.flush();
	delay(1);
	f.close();
}


// Print the contents of the file to the serial.
// parameters:
//   fileName ... name of the log file
// returns nothing
void printFileToSerial(String fileName) {

	Serial.println("------------------------------------------------------");

	File f = SPIFFS.open(fileName, "r");
	if (!f) {
		Serial.println("file \"" + fileName + "\" open failed");
		Serial.println(
				"------------------------------------------------------");
		return;
	}

	Serial.println("Content of file \"" + fileName + "\":");
	while (f.available()) {
		//Lets read line by line from the file
		String line = f.readStringUntil('\n');
		Serial.println(line);
	}
	f.close();
	Serial.println("------------------------------------------------------");
}


// Copies the source file to the destination file. Overwrites the destination file if it already!
// parameters:
//   sourceFilename ... name of the source file
//   destFilename ... name of the destination file
// returns nothing
void copyFile(String sourceFilename, String destFilename) {

	if (!SPIFFS.exists(sourceFilename)) {
		Serial.println("could not find \"" + sourceFilename + "\"");
		return;
	}
	File fSrc = SPIFFS.open(sourceFilename, "r");
	if (!fSrc) {
		Serial.println("error opening \"" + sourceFilename + "\" for reading");
		return;
	}
	File fDst = SPIFFS.open(destFilename, "w");
	if (!fDst) {
		Serial.println("error opening \"" + destFilename + "\" for writing");
		return;
	}
	while (fSrc.available()) {
		fDst.print(fSrc.readStringUntil('\n') + "\n");
	}
	fDst.close();
	fSrc.close();
}


// Renames the file. Overwrites the destination file if it already exists!
// parameters:
//   sourceFilename ... name of the source file
//   destFilename ... name of the destination file
// returns nothing
void renameFile(String sourceFilename, String destFilename) {

	if (SPIFFS.exists(destFilename)) {
		SPIFFS.remove(destFilename);
	}

	SPIFFS.rename(sourceFilename, destFilename);
}

// Deletes the file if it exists.
// parameters:
//   filename ... name of the file
// returns nothing
void deleteFile(String filename) {

	if (SPIFFS.exists(filename)) {
		SPIFFS.remove(filename);
	}
}
