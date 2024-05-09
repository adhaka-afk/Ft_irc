#include "Bot.hpp"
#include "Logger.hpp"

#include <arpa/inet.h>

Bot::Bot(string const& name, string const& channel, string const& pass):
	name(name), channel(channel), pass(pass) { log(DEBUG) << "Bot " << name << " created"; }

void	Bot::start(string const& host, string const& port, string const& pass) {
	log(INFO) << "Starting bot " << this->name;


	log(DEBUG) << "Connecting to server on " << host << ":" << port << " with password " << pass;
	// Port validation
	char *end;
	long portNum = strtol(port.c_str(), &end, 10);
	if (*end != 0)
		log(WARN) << "Port number contains non-numeric character and set to " << portNum;
	if (portNum < 1024 || portNum > 65535) {
		log(ERROR) << "Port number must be within 1024 and 65535";
		throw Bot::InvalidArgumentException();
	}

	// Create socket
	log(DEBUG) << "Creating socket";
	this->sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->sockFd < 0) {
		log(ERROR) << "Unable to create socket with error " << errno;
		return;
	}
	log(DEBUG) << "Socket created on file descriptor " << this->sockFd;

	// IP validation
	log(DEBUG) << "Resolving hostname " << host;
	addrinfo hints = {};
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	addrinfo *res;
	if (getaddrinfo(host.c_str(), NULL, &hints, &res) != 0) {
		log(ERROR) << "Unable to resolve hostname";
		return;
	}

	if (res->ai_family != AF_INET) {
		log(ERROR) << "Unable to resolve hostname to supported IPv4 address";
		freeaddrinfo(res);
		return;
	}

	// Setup and bind socket to port
	log(DEBUG) << "Binding socket to port";
	sockaddr_in	sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(portNum);
	sin.sin_addr = ((sockaddr_in*)res->ai_addr)->sin_addr;
	freeaddrinfo(res);
	if (connect(this->sockFd, (sockaddr*)&sin, sizeof(sin)) < 0) {
		log(ERROR) << "Unable to bind socket to port with error " << errno;
		return;
	}

	// Register to server
	log(DEBUG) << "Registering to server";
	string	msg = "PASS " + pass + "\r\n";
	msg += "NICK " + this->name + "\r\n";
	msg += "USER " + this->name + " 0 * :" + this->name + "\r\n";
	msg += "JOIN " + this->channel + " " + this->pass + "\r\n";
	msg += "MODE " + this->name + " +B\r\n";
	send(this->sockFd, msg.c_str(), msg.length(), 0);

	// Listen to channel messages
	log(DEBUG) << "Listening to channel messages";
	char buff[MAXMSG];
	while (true) {
		int len = recv(this->sockFd, buff, MAXMSG - 1, 0);
		if (len < 0) {
			log(ERROR) << "Unable to receive message with error " << errno;
			return;
		}
		buff[len] = 0;
		stringstream	ss(buff);
		string			token;
		getline(ss, token, ' ');
		getline(ss, token, ' ');

		if (token == "PING") {
			log(DEBUG) << "Received PING message";
			getline(ss, token, '\n');
			msg = "PONG " + token + "\n";
			send(this->sockFd, msg.c_str(), msg.length(), 0);
			continue;
		}
		if (token == "PRIVMSG" || token == "NOTICE") {
			getline(ss, token, ' ');
			if (token != this->channel)
				continue;
			log(DEBUG) << "Received channel message";
			getline(ss, token, '\n');
			transform(token.begin(), token.end(), token.begin(), ::tolower);
			if (token.find("bitcoin") != string::npos) {
				log(DEBUG) << "Sending message to channel";
				msg = "PRIVMSG " + this->channel + " :I'm a bot and I love bitcoins\r\n";
				send(this->sockFd, msg.c_str(), msg.length(), 0);
			}
		}
	}
}

#ifndef LOG_LEVEL
 #define LOG_LEVEL DEBUG
#endif
logLevel logLvl = LOG_LEVEL;

int main(int narg, char **args) {
	if (narg != 7) {
		cout << "Usage: ./ircbot <ircserver> <port> <password> <botname> <channel> <channel password>" << endl;
		return 1;
	}
	Bot bot(args[4], args[5], args[6]);
	bot.start(args[1], args[2], args[3]);
	return 0;
}