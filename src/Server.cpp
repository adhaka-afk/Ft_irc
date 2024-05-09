#include "Server.hpp"

Server::~Server() {
	log(DEBUG) << "Closing all open file descriptor";
	for (vector<pollfd>::reverse_iterator rit = this->fds.rbegin(); rit != this->fds.rend(); ++rit)
		close(rit->fd);
}

Server::Server() {
	log(DEBUG) << "Server instance created";
}

Server&	Server::instance() {
	static Server server;
	return server;
}

void	Server::init(string const& port, string const& password) {
	log(DEBUG) << "Initializing server on port " << port << " and password " << password;
	this->port = port;
	this->password = password;
	this->created = time(NULL);

	// Port validation
	char *end;
	long portNum = strtol(this->port.c_str(), &end, 10);
	if (*end != 0)
		log(WARN) << "Port number contains non-numeric character and set to " << portNum;
	if (portNum < 1024 || portNum > 65535) {
		log(ERROR) << "Port number must be within 1024 and 65535";
		throw Server::InvalidArgumentException();
	}

	// Create socket
	log(DEBUG) << "Creating socket";
	this->sockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->sockFd < 0) {
		log(FATAL) << "Unable to create socket with error " << errno;
		throw Server::RuntimeException();
	}
	log(DEBUG) << "Socket created on file descriptor " << this->sockFd;

	// Setup socket options
	log(DEBUG) << "Setting socket option";
	int	opt = 1;
	if (setsockopt(this->sockFd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
		log(FATAL) << "Unable to attach socket to port with error " << errno;
		throw Server::RuntimeException();
	}

	// Make socket non-blocking
	log(DEBUG) << "Making socket non-blocking";
	if (fcntl(this->sockFd, F_SETFL, O_NONBLOCK) < 0) {
		log(FATAL) << "Unable to make socket non-blocking with error " << errno;
		throw Server::RuntimeException();
	}

	// Setup and bind socket to port
	log(DEBUG) << "Binding socket to port";
	sockaddr_in	sin = {};
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(portNum);
	if (bind(this->sockFd, (sockaddr*)&sin, sizeof(sin)) < 0) {
		log(FATAL) << "Unable to bind socket to port with error " << errno;
		throw Server::RuntimeException();
	}

	// Listen for connections
	log(DEBUG) << "Listening on socket";
	if (listen(this->sockFd, MAX_CONNECTIONS) < 0) {
		log(FATAL) << "Unable to listen on socket with error " << errno;
		throw Server::RuntimeException();
	}
	this->ready = true;
	signal(SIGINT, Server::intHandler);

	// Setup standard input
	log(DEBUG) << "Setting standard input";
	this->fds.push_back((pollfd){STDIN_FILENO, POLLIN, 0});

	// Setup initial listening socket
	log(DEBUG) << "Setting inital listening socket on " << this->sockFd;
	this->fds.push_back((pollfd){this->sockFd, POLLIN, 0});
}

void	Server::intHandler(int signum) {
	(void) signum;
	Server::instance().stop();
}

void	Server::stop() {
	log(INFO) << "Stopping server";
	this->ready = false;
}

void	Server::start() {
	log(INFO) << "Server is listening on port " << this->port;
	while (this->ready) {
		log(DEBUG) << "Polling for events";
		if (poll(this->fds.begin().base(), this->fds.size(), -1) < 0) {
			if (errno == EINTR)	continue;
			log(FATAL) << "Polling failed with error " << errno;
			throw Server::RuntimeException();
		}

		log(DEBUG) << "Looping and processing file descriptors";
		for (size_t i = 0; i < this->fds.size(); ++i) {
			log(DEBUG) << "Processing fd " << this->fds[i].fd << " with revents " << this->fds[i].revents;
			if (this->fds[i].revents == 0)		continue;
			if (this->fds[i].revents & POLLHUP)	{ this->detach(i); break; }
			if (this->fds[i].revents & POLLIN) {
				switch (i) {
					case 0:		this->console(); break;
					case 1:		this->attach(); break;
					default:	this->clients.find(this->fds[i].fd)->second.collect(); break;
				}
				break;
			}
			if (this->fds[i].revents & POLLOUT)	{ this->clients.find(this->fds[i].fd)->second.dispatch(); break; }
			log(WARN) << "Unexpected event on " << this->fds[i].fd;
		}
	}
}

void	Server::console() {
	log(DEBUG) << "Console event triggered";

	string command;
	getline(cin, command);
	if (command.empty())	return;

	transform(command.begin(), command.end(), command.begin(), ::toupper);
	if (command == "STOP")	{ this->stop(); }
	else if (command == "NICKS")	{
		log(INFO) << "Listing clients";
		for (map<int, Client>::iterator it = this->clients.begin(); it != this->clients.end(); ++it)
			log(INFO) << it->second.getNick() << " at " << it->second.getHostname() << ":" << it->second.getPort();
	}
	else if (command == "CHANNELS")	{
		log(INFO) << "Listing channels";
		for (map<string, Channel>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
			log(INFO) << it->first;
	}
	else					log(WARN) << "Unknown command " << command;
}

void	Server::attach() {
	log(DEBUG) << "New connection available";

	sockaddr_in	addr = {};
	socklen_t	size = sizeof(addr);
	int			clientFd = accept(this->sockFd, (sockaddr*)&addr, &size);
	if (clientFd < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			log(FATAL) << "Accept failed with error " << errno;
			throw Server::RuntimeException();
		}
		log(WARN) << "Unable to accept connection at the moment";
		return;
	}
	log(DEBUG) << "New incoming connection on " << clientFd;
	this->fds.push_back((pollfd){clientFd, POLLIN, 0});

	// Get client hostname
	char hostname[HOSTLEN];
	int res = getnameinfo((sockaddr*)&addr, size, hostname, HOSTLEN, NULL, 0, NI_NAMEREQD);
	if (res != 0 && (errno == EAI_NONAME || errno == EAI_AGAIN)) {
		log(WARN) << "Unable to get client hostname";
	} else if (res != 0) {
		log(FATAL) << "Get name info failed with error " << errno;
		throw Server::RuntimeException();
	}
	log(DEBUG) << "Adding client " << clientFd << " at " << hostname << ":" << ntohs(addr.sin_port);
	log(INFO) << hostname << ":" << ntohs(addr.sin_port) << " has connected";
	this->clients.insert(make_pair(clientFd, Client(hostname, ntohs(addr.sin_port), clientFd)));
}

void	Server::detach(int i) {
	close(this->fds[i].fd);
	log(DEBUG) << "Connection closed on " << this->fds[i].fd;

	Client&	client = this->clients.find(this->fds[i].fd)->second;
	log(INFO) << (client.getNick() == "*" ? "" : client.getNick() + "@")
			  << client.getHostname() << ":" << client.getPort()
			  << " has disconnected";
	client.announce(MESSAGE(NICKNAME(client.getNick(), client.getUser(), client.getHostname()), "QUIT :Connection closed"));
	set<string>	channels = client.getChannels();
	for (set<string>::iterator it = channels.begin(); it != channels.end(); ++it) {
		client.removeChannel(*it);
		Server::instance().getChannel(*it).removeClient(client.getSocket());
	}

	log(DEBUG) << "Cleaning up client resources at " << this->fds[i].fd;
	this->fds.erase(this->fds.begin() + i);
	this->nicks.erase(client.getNick());
	this->clients.erase(client.getSocket());
}

void	Server::broadcast(string const& msg, int id) {
	if (id == 0) {
		for (map<int, Client>::iterator it = this->clients.begin(); it != this->clients.end(); ++it)
			it->second.dispatch(msg);
	} else {
		set<int>	sent; sent.insert(id);
		set<string>	channels = this->clients.at(id).getChannels();
		for (set<string>::iterator chIt = channels.begin(); chIt != channels.end(); ++chIt) {
			map<int, int> const&	clients = this->channels.at(*chIt).getClients();
			for (map<int, int>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
				if (sent.find(it->first) != sent.end() || it->second < REGULAR)	continue;
				this->clients.at(it->first).dispatch(msg);
				sent.insert(it->first);
			}
		}
	}
}

void	Server::parse(string& msg, string& command, vector<string>& params) {
	log(DEBUG) << "Parsing message " << msg;
	if (msg.length() > MAXMSG)	{
		msg.replace(MAXMSG - 2, 2, "\r\n");
		log(DEBUG) << "Client message too long, truncated to " << msg;
	}
	msg.erase(msg.find_last_not_of("\r\n") + 1, string::npos);
	if (msg.empty()) {
		log(DEBUG) << "Empty message ignored";
		return;
	}

	stringstream	ss(msg);
	string			token;
	while(getline(ss, token, ' ')) {
		if (token.empty())	continue;
		if ((token[0] == '@' || token[0] == ':') && params.empty())	continue;
		if (token[0] == ':' && !params.empty()) {
			params.push_back(token.substr(1));
			if (getline(ss, token))
				params.back().append(" " + token);
		}
		else	params.push_back(token);
	}
	command = params[0];
	params.erase(params.begin());
}

void	Server::invoke(Client& client, string& msg) {
	while (!msg.empty()) {
		size_t pos = msg.find_first_of("\r\n");
		pos += (msg[pos + 1] == '\n' || msg[pos + 1] == '\r');
		string message = msg.substr(0, pos);
		msg.erase(0, pos + 1);
		log(DEBUG) << "Invoking message " << message;

		string			original, command;
		vector<string>	params;
		this->parse(message, original, params);

		command = original;
		transform(command.begin(), command.end(), command.begin(), ::toupper);

		if (command.empty())			{ log(DEBUG) << "Empty command ignored"; }
		else if (command == "AWAY")		{ Away().execute(client, params); }
		else if (command == "JOIN")		{ Join().execute(client, params); }
		else if (command == "KICK")		{ Kick().execute(client, params); }
		else if (command == "LIST")		{ List().execute(client, params); }
		else if (command == "MODE")		{ Mode().execute(client, params); }
		else if (command == "NICK")		{ Nick().execute(client, params); }
		else if (command == "PART")		{ Part().execute(client, params); }
		else if (command == "PASS")		{ Pass().execute(client, params); }
		else if (command == "PING")		{ Ping().execute(client, params); }
		else if (command == "PONG")		{ Pong().execute(client, params); }
		else if (command == "QUIT")		{ Quit().execute(client, params); }
		else if (command == "USER")		{ User().execute(client, params); }
		else if (command == "NAMES")	{ Names().execute(client, params); }
		else if (command == "TOPIC")	{ Topic().execute(client, params); }
		else if (command == "INVITE")	{ Invite().execute(client, params); }
		else if (command == "NOTICE")	{ Notice().execute(client, params); }
		else if (command == "PRIVMSG")	{ Privmsg().execute(client, params); }
		else {
			log(DEBUG) << "Unknown command " << command;
			if (!client.isRegistered())
				client.dispatch(MESSAGE(SERVER_NAME, ERR_NOTREGISTERED(client.getNick())));
			else
				client.dispatch(MESSAGE(SERVER_NAME, ERR_UNKNOWNCOMMAND(client.getNick(), original)));
		}
	}
}

bool	Server::authenticate(string const& pass) const {
	return pass == this->password;
}

bool	Server::updateNick(Client const& client, string const& nick) {
	map<string, int>::iterator it = this->nicks.find(nick);
	if (it != this->nicks.end())	return false;
	this->nicks.insert(make_pair(nick, client.getSocket()));
	this->nicks.erase(client.getNick());
	return true;
}

string	Server::getCreated() const {
	string	timestamp = ctime(&this->created);
	timestamp.erase(timestamp.length() - 1);
	return timestamp;
}

bool	Server::hasClient(int fd) const { return this->clients.find(fd) != this->clients.end(); }
bool	Server::hasClient(string const &nick) const { return this->nicks.find(nick) != this->nicks.end(); }
bool	Server::hasChannel(string const& name) const { return this->channels.find(name) != this->channels.end(); }

Client&		Server::getClient(int fd) {
	if (this->clients.find(fd) == this->clients.end())
		throw Server::RuntimeException();
	return this->clients.at(fd);
}

Client&		Server::getClient(string const& nick) {
	if (this->nicks.find(nick) == this->nicks.end())
		throw Server::RuntimeException();
	return this->clients.at(this->nicks.at(nick));
}

Channel&	Server::getChannel(string const& name) {
	if (this->channels.find(name) == this->channels.end())
		throw Server::RuntimeException();
	return this->channels.at(name);
}

map<string, int> const&		Server::getNicks() const { return this->nicks; }
map<string, Channel> const&	Server::getChannels() const { return this->channels; }

void	Server::addChannel(string const& name) {
	if (this->channels.find(name) == this->channels.end()) {
		log(DEBUG) << "Adding channel " << name;
		this->channels.insert(make_pair(name, Channel(name)));
	}
}

void	Server::removeChannel(string const& name) {
	if (this->channels.find(name) != this->channels.end()) {
		log(DEBUG) << "Removing channel " << name;
		this->channels.erase(name);
	}
}