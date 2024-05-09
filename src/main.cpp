#include <iostream>
#include <string>
#include "Server.hpp"

#ifndef LOG_LEVEL
 #define LOG_LEVEL INFO
#endif
logLevel logLvl = LOG_LEVEL;

using namespace std;

int main(int narg, char *args[]) {
	if (narg != 3) {
		log(ERROR) << "Usage: ./ircserv <port> <password>";
		return 1;
	}

	Server::instance().init(args[1], args[2]);
	Server::instance().start();
	return 0;
}