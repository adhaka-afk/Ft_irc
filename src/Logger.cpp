#include "Logger.hpp"

Logger::Logger(logLevel level) {
	char		timestamp[80];
	time_t		now = time(NULL);

	string	logLvl;
	switch(level) {
		case DEBUG:	logLvl = "DEBUG";	break;
		case INFO:	logLvl = "INFO";	break;
		case WARN:	logLvl = "WARN";	break;
		case ERROR:	logLvl = "ERROR";	break;
		case FATAL:	logLvl = "FATAL";	break;
	}

	strftime(timestamp, sizeof(timestamp), "%d-%m-%Y %H:%M:%S", localtime(&now));
	this->buffer << "[ " << left << setw(5) << logLvl << " " << timestamp << " ] ";
}

Logger::~Logger() {
	cerr << this->buffer.str() << endl;
}

