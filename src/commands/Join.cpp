#include "ICommand.hpp"

void	Join::execute(Client& client, vector<string>& params) {
	log(DEBUG) << "Executing JOIN command";

	// Check if registered
	if (!client.isRegistered())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOTREGISTERED(client.getNick())));
	// Check empty parameters
	if (params.empty() || params[0].empty())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NEEDMOREPARAMS(client.getNick(), "JOIN")));

	// Disable parting all channels
	if (DIS_JOIN_0 && params[0] == "0")
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOSUCHCHANNEL(client.getNick(), params[0])));
	if (params[0] == "0") {
		if (client.getChannels().empty())	return;
		params[0].clear();
		params.push_back("Left all channels");
		for (set<string>::iterator it = client.getChannels().begin(); it != client.getChannels().end(); ++it)
			params[0] += *it + ",";
		return Part().execute(client, params);;
	}

	// Loop through channels
	string			channel, key;
	stringstream	ssChan(params[0]), ssKey(params.size() > 1 ? params[1] : "");
	while (getline(ssChan, channel, ',')) {
		// Check channel name validity
		channel = channel.substr(0, CHANNELLEN);
		if (channel.empty())	continue;
		if (string(CHANTYPES).find(channel[0]) == string::npos) {
			client.dispatch(MESSAGE(SERVER_NAME, ERR_NOSUCHCHANNEL(client.getNick(), channel)));
			continue;
		}
		// Check channel limit
		string	chanLimit = CHANLIMIT;
		chanLimit = chanLimit.substr(0, chanLimit.find(",")).substr(chanLimit.find(":") + 1);
		if (!chanLimit.empty() && client.getChannels().size() >= (size_t) strtol(chanLimit.c_str(), NULL, 10)) {
			client.dispatch(MESSAGE(SERVER_NAME, ERR_TOOMANYCHANNELS(client.getNick(), channel)));
			continue;
		}

		// Loop through keys
		while(getline(ssKey, key, ',')) { if (key.empty())	continue; }

		// Add channel if not exists
		if (!Server::instance().hasChannel(channel))
			Server::instance().addChannel(channel);
		// Get existing channel
		Channel&	chan = Server::instance().getChannel(channel);
		if (chan.hasClient(client.getSocket()))	continue;
		// Check channel mode
		if (chan.getMode().find("k") != string::npos && chan.getKey() != key) {
			client.dispatch(MESSAGE(SERVER_NAME, ERR_BADCHANNELKEY(client.getNick(), channel)));
			continue;
		}
		if (chan.getMode().find("l") != string::npos && chan.getSize() >= chan.getLimit()) {
			client.dispatch(MESSAGE(SERVER_NAME, ERR_CHANNELISFULL(client.getNick(), channel)));
			continue;
		}
		if (chan.getMode().find("i") != string::npos && !chan.isInvited(client.getSocket())) {
			client.dispatch(MESSAGE(SERVER_NAME, ERR_INVITEONLYCHAN(client.getNick(), channel)));
			continue;
		}
		// Add client to channel
		client.addChannel(channel);
		chan.addClient(client.getSocket(), chan.getSize() == 0 ? OPER : REGULAR);
		chan.broadcast(MESSAGE(NICKNAME(client.getNick(), client.getUser(), client.getHostname()), "JOIN " + channel));
		if (chan.getSize() == 1)
			client.dispatch(MESSAGE(SERVER_NAME, "MODE " + channel + " +" + chan.getMode()));
		if (!chan.getTopic().empty()) {
			stringstream	ss;
			ss << chan.getTopicSetat();
			client.dispatch(MESSAGE(SERVER_NAME, RPL_TOPIC(client.getNick(), channel, chan.getTopic())));
			client.dispatch(MESSAGE(SERVER_NAME, RPL_TOPICWHOTIME(client.getNick(), channel, chan.getTopicNick(), ss.str())));
		}
		string	names = chan.getNames(client.getSocket()), symbol = chan.getMode().find("s") != string::npos ? "@" : "=";
		size_t	size = MAXMSG - (MESSAGE(SERVER_NAME, RPL_NAMREPLY(client.getNick(), " ", channel, ""))).length();
		size_t	len = names.length(), pos = 0, i;
		do {
			if (!EN_EMPTY_NAMES && names.empty())	break;
			i = len > size ? names.find_last_of(' ', pos + size + 1) : len;
			if (i == string::npos)	i = names.find(' ', pos + size);
			if (i == string::npos || i == pos - 1)	i = len;
			client.dispatch(MESSAGE(SERVER_NAME, RPL_NAMREPLY(client.getNick(), symbol, channel, names.substr(pos, i - pos))));
			pos = i + 1;
		} while (pos < len);
		client.dispatch(MESSAGE(SERVER_NAME, RPL_ENDOFNAMES(client.getNick(), channel)));
	}
}