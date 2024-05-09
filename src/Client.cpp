#include "Client.hpp"

Client::Client(string const& hostname, int port, int socket):
	registered(false), socket(socket), port(port), nick("*"), hostname(hostname), mode(UMODE_INIT) {}

int	Client::getPort() const { return this->port; }
int	Client::getSocket() const { return this->socket; }

string const&	Client::getAway() const { return this->away; }
string const&	Client::getMode() const { return this->mode; }
string const&	Client::getNick() const { return this->nick; }
string const&	Client::getUser() const { return this->user; }
string const&	Client::getHostname() const { return this->hostname; }

void	Client::setAway(string const& away) { this->away = away; }
void	Client::setPass(string const& pass) { this->pass = pass; }
void	Client::setNick(string const& nick) { this->nick = nick; }
void	Client::setUser(string const& user) { this->user = user; }
void	Client::setReal(string const& real) { this->real = real; }

bool	Client::isRegistered() const { return this->registered; }

bool	Client::toggleMode(char mode, bool on, string const& param) {
	(void) param;
	log(DEBUG) << "Toggling mode " << mode << " to " << (on ? "on" : "off") << " for " << this->nick;

	size_t	pos;
	if (string(USERMODE).find(mode) == string::npos
		|| ((pos = this->mode.find(mode)) != string::npos) == on)
		return false;

	if (string(BOT).find(mode) != string::npos) {
		if (on == this->bot)	return false;
		this->bot = on;
		return true;
	}

	if (!on)	this->mode.erase(pos, 1);
	else {
		pos = this->mode.find_last_of(string(CHANMODE).substr(0, string(CHANMODE).find(mode)));
		this->mode.insert(pos == string::npos ? 0 : pos + 1, 1, mode);
	}
	return true;
}

void	Client::collect() {
	log(DEBUG) << "File descriptor " << this->socket << " readable";
	log(DEBUG) << "Receiving message from " << this->hostname << ":" << this->port;

	char buffer[1024];
	int rc = recv(this->socket, buffer, sizeof(buffer) - 1, 0);
	log(DEBUG) << "Received " << rc << " bytes";
	if (rc < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			log(FATAL) << "Receive failed with error " << errno;
			throw Server::RuntimeException();
		}
		log(WARN) << "Unable to receive message at the moment";
		return;
	}
	if (rc == 0)	shutdown(this->socket, SHUT_WR);
	buffer[rc] = 0;
	this->iBuff.append(buffer);

	if (this->iBuff.find_last_of("\r\n") < this->iBuff.length()) {
		stringstream ss;
		for (size_t i = 0; i < this->iBuff.length(); ++i) {
			if (this->iBuff[i] < 32 || this->iBuff[i] > 126)
				ss << "\\x" << setfill('0') << setw(2) << hex << (uint16_t)this->iBuff[i];
			else
				ss << this->iBuff[i];
		}
		log(INFO) << "Received message \"" << ss.str() << "\"";
		Server::instance().invoke(*this, this->iBuff);
		this->iBuff.clear();
	}
}

void	Client::dispatch() {
	log(DEBUG) << "File descriptor " << this->socket << " writable";
	log(DEBUG) << "Sending message to " << this->hostname << ":" << this->port;

	int rc = send(this->socket, this->oBuff.c_str(), this->oBuff.length(), 0);
	if (rc < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			log(FATAL) << "Send failed with error " << errno;
			throw Server::RuntimeException();
		}
		log(WARN) << "Unable to send message at the moment";
		return;
	}
	log(DEBUG) << "Sent " << rc << " bytes";
	this->oBuff.erase(0, rc);
}

void	Client::dispatch(string const& msg) {
	this->oBuff.append(msg);
	this->dispatch();
}

void	Client::announce(string const& msg, bool reflect) {
	log(DEBUG) << "Announcing message \"" << msg << "\" from " << this->hostname << ":" << this->port;
	if (reflect)	this->dispatch(msg);
	Server::instance().broadcast(msg, this->socket);
}

void	Client::registrate() {
	if (this->registered)	return;
	if (this->nick != "*" && !this->user.empty()) {
		this->registered = Server::instance().authenticate(this->pass);
		if (this->registered) {
			log(INFO) << this->hostname << ":" << this->port << " has registered as " << this->user;
			this->dispatch(MESSAGE(SERVER_NAME, RPL_WELCOME(this->nick)));
			this->dispatch(MESSAGE(SERVER_NAME, RPL_YOURHOST(this->nick, SERVER_NAME, "1.0")));
			this->dispatch(MESSAGE(SERVER_NAME, RPL_CREATED(this->nick, Server::instance().getCreated() + " UTC")));
			this->dispatch(MESSAGE(SERVER_NAME, RPL_MYINFO(this->nick, SERVER_NAME, "1.0", USERMODE, CHANMODE)));

			stringstream ss;
			ss << SUPPORTED_0;
			this->dispatch(MESSAGE(SERVER_NAME, RPL_ISUPPORT(this->nick, ss.str())));
			ss.str("");
			ss << SUPPORTED_1;
			this->dispatch(MESSAGE(SERVER_NAME, RPL_ISUPPORT(this->nick, ss.str())));
			ss.str("");
			ss << SUPPORTED_2;
			this->dispatch(MESSAGE(SERVER_NAME, RPL_ISUPPORT(this->nick, ss.str())));

			this->dispatch(MESSAGE(SERVER_NAME, ERR_NOMOTD(this->nick)));
			this->dispatch(MESSAGE(SERVER_NAME, RPL_UMODEIS(this->nick, this->mode)));
		} else {
			log(DEBUG) << this->hostname << ":" << this->port << " registration failed.";
			this->dispatch(MESSAGE(SERVER_NAME, ERR_PASSWDMISMATCH(this->nick)));
		}
	}
}

void	Client::addChannel(string const& channel) {	this->channels.insert(channel); }
void	Client::removeChannel(string const& channel) { this->channels.erase(channel); }

set<string> const&	Client::getChannels() const { return this->channels; }