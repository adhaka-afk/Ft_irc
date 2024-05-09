#pragma once

#include <time.h>

#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>

using namespace std;

enum logLevel { DEBUG, INFO, WARN, ERROR, FATAL };

class Logger {
	private:
		ostringstream buffer;

	public:
		Logger(logLevel level = INFO);
		~Logger();

		template <typename T>
		Logger& operator <<(T const & value) {
			this->buffer << value;
			return *this;
		}
};

extern logLevel logLvl;

#define log(level) if (level >= logLvl) Logger(level)