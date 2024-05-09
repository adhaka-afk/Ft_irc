#include "Channel.hpp"

Channel::Channel(string const& name):
	created(time(NULL)), name(name), mode(MODE_INIT) {}

time_t	Channel::getCreated() const { return this->created; }
time_t	Channel::getTopicSetat() const { return this->topicSetat; }

size_t	Channel::getSize() const { return this->clients.size(); }
size_t	Channel::getLimit() const { return this->limit; }

string const&	Channel::getMode() const { return this->mode; }
string const&	Channel::getKey() const { return this->key; }
string const&	Channel::getName() const { return this->name; }
string const&	Channel::getTopic() const { return this->topic; }
string const&	Channel::getTopicNick() const { return this->topicNick; }

string	Channel::getNames(int who) const {
	string	members;
	for (map<int, int>::const_iterator it = this->clients.begin(); it != this->clients.end(); ++it) {
		Client&	client = Server::instance().getClient(it->first);
		if ((client.getMode().find("i") != string::npos && this->clients.find(who) == this->clients.end()))
			continue;
		if (it != this->clients.begin())	members += " ";
		members += it->second & OPER ? "@" : it->second & VOICED ? "+" : "";
		members += client.getNick();
	}
	return members;
}

bool	Channel::toggleMode(char mode, bool on, string const& param) {
	size_t pos;

	if (string(CHANMODES MEMBERMODE).find(mode) == string::npos
		|| ((pos = this->mode.find(mode) != string::npos) == on && string(CHANMODE_D).find(mode) != string::npos))
		return false;

	// Channel membership modes
	if (string(MEMBERMODE).find(mode) != string::npos) {
		if (!Server::instance().hasClient(param))	return false;

		int	id = Server::instance().getClient(param).getSocket();
		if (this->clients.find(id) == this->clients.end())	return false;
		switch(mode) {
			case 'o':
				if (on == ((this->clients[id] & OPER) != 0))	return false;
				this->clients[id] ^= OPER;
				break;
			case 'v':
				if (on == ((this->clients[id] & VOICED) != 0))	return false;
				this->clients[id] ^= VOICED;
				break;
		}
		return true;
	}

	// Channel settings modes
	if (!on && pos == string::npos)	return false;
	if (mode == 'l') {
		long	limit = strtol(param.c_str(), NULL, 10);
		if ((on && limit < 1))	return false;
		this->limit = limit;
	}
	if (mode == 'k') {
		if (param.empty())	return false;
		if (on)	this->key = param;
		else	this->key.clear();
	}
	if (!on) {
		this->mode.erase(pos, 1);
	} else if (pos == string::npos) {
		pos = this->mode.find_last_of(string(CHANMODE).substr(0, string(CHANMODE).find(mode)));
		this->mode.insert(pos == string::npos ? 0 : pos + 1, 1, mode);
	}
	return true;
}

void	Channel::setTopic(string const& nick, string const& topic) {
	this->topic = topic;
	this->topicNick = nick;
	this->topicSetat = time(NULL);
}

bool	Channel::isInvited(int id) const { return this->invited.find(id) != this->invited.end(); }
void	Channel::inviteClient(int id) {
	if (this->clients.find(id) != this->clients.end())				return;
	if (!EN_OFF_INVLIST && this->mode.find('i') == string::npos)	return;
	this->invited.insert(id);
}

bool	Channel::hasClient(int id) const { return this->clients.find(id) != this->clients.end(); }
void	Channel::addClient(int id, int type) {
	if (this->clients.find(id) != this->clients.end())	return;
	if (this->mode.find('i') != string::npos && this->invited.find(id) == this->invited.end())	return;
	log(DEBUG) << "Adding client " << id << " to channel " << this->name << " as " << type;
	this->clients[id] = REGULAR | type;
	this->invited.erase(id);
}
void	Channel::removeClient(int id) {
	log(DEBUG) << "Removing client " << id << " from channel " << this->name;
	this->clients.erase(id);
	this->invited.erase(id);
	if (this->clients.size() == 0)
		Server::instance().removeChannel(this->name);
}
map<int, int> const&	Channel::getClients() const { return this->clients; }

int		Channel::getClientType(int id) const { return this->clients.find(id) == this->clients.end() ? 0 : this->clients.at(id); }

void	Channel::broadcast(string const& msg, int id, int type) {
	for (map<int, int>::iterator it = this->clients.begin(); it != this->clients.end(); ++it) {
		if (it->second < type || it->first == id
			|| (id > 0 && Server::instance().getClient(it->first).getMode().find(DEAF) != string::npos))
			continue;
		Server::instance().getClient(it->first).dispatch(msg);
	}
}
