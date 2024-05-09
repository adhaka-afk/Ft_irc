#include "ICommand.hpp"

void	Names::execute(Client& client, vector<string>& params) {
	log(DEBUG) << "Executing NAMES command";

	// Check if registered
	if (!client.isRegistered())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOTREGISTERED(client.getNick())));

	if (params.empty() || params[0].empty()) {
		/***     Secret channel first     ***/
		// // Loop through all channels
		// for (set<string>::iterator it = client.getChannels().begin(); it != client.getChannels().end(); ++it) {
		// 	Channel&	chan = Server::instance().getChannel(*it);
		// 	if (chan.getMode().find("s") == string::npos)	continue;
		// 	string	names = chan.getNames(client.getSocket()), symbol = "@";
		// 	size_t	size = MAXMSG - (MESSAGE(SERVER_NAME, RPL_NAMREPLY(client.getNick(), " ", *it, ""))).length();
		// 	size_t	len = names.length(), pos = 0, i;
		// 	do {
		// 		if (!EN_EMPTY_NAMES && names.empty())	break;
		// 		i = len > size ? names.find_last_of(' ', pos + size + 1) : len;
		// 		if (i == string::npos)	i = names.find(' ', pos + size);
		// 		if (i == string::npos || i == pos - 1)	i = len;
		// 		client.dispatch(MESSAGE(SERVER_NAME, RPL_NAMREPLY(client.getNick(), symbol, *it, names.substr(pos, i - pos))));
		// 		pos = i + 1;
		// 	} while (pos < len);
		// }

		// for (map<string, Channel>::const_iterator it = Server::instance().getChannels().begin(); it != Server::instance().getChannels().end(); ++it) {
		// 	if (it->second.getMode().find("s") != string::npos)	continue;
		// 	string	names = it->second.getNames(client.getSocket()), symbol = "=";
		// 	size_t	size = MAXMSG - (MESSAGE(SERVER_NAME, RPL_NAMREPLY(client.getNick(), " ", it->first, ""))).length();
		// 	size_t	len = names.length(), pos = 0, i;
		// 	do {
		// 		if (!EN_EMPTY_NAMES && names.empty())	break;
		// 		i = len > size ? names.find_last_of(' ', pos + size + 1) : len;
		// 		if (i == string::npos)	i = names.find(' ', pos + size);
		// 		if (i == string::npos || i == pos - 1)	i = len;
		// 		client.dispatch(MESSAGE(SERVER_NAME, RPL_NAMREPLY(client.getNick(), symbol, it->first, names.substr(pos, i - pos))));
		// 		pos = i + 1;
		// 	} while (pos < len);
		// }

		// Loop through all channels
		/***     Alphabetical     ***/
		for (map<string, Channel>::const_iterator it = Server::instance().getChannels().begin(); it != Server::instance().getChannels().end(); ++it) {
			if (it->second.getMode().find("s") != string::npos && !it->second.hasClient(client.getSocket()))	continue;
			string	names = it->second.getNames(client.getSocket()), symbol = it->second.getMode().find("s") != string::npos ? "@" : "=";
			size_t	size = MAXMSG - (MESSAGE(SERVER_NAME, RPL_NAMREPLY(client.getNick(), " ", it->first, ""))).length();
			size_t	len = names.length(), pos = 0, i;
			do {
				if (!EN_EMPTY_NAMES && names.empty())	break;
				i = len > size ? names.find_last_of(' ', pos + size + 1) : len;
				if (i == string::npos)	i = names.find(' ', pos + size);
				if (i == string::npos || i == pos - 1)	i = len;
				client.dispatch(MESSAGE(SERVER_NAME, RPL_NAMREPLY(client.getNick(), symbol, it->first, names.substr(pos, i - pos))));
				pos = i + 1;
			} while (pos < len);
		}

		string	names;
		size_t	size = MAXMSG - (MESSAGE(SERVER_NAME, RPL_NAMREPLY(client.getNick(), "*", "*", ""))).length();
		for (map<string, int>::const_iterator it = Server::instance().getNicks().begin(); it != Server::instance().getNicks().end(); ++it) {
			Client&	targ = Server::instance().getClient(it->first);
			if (!names.empty())	names += " ";
			if (targ.getMode().find("i") != string::npos)	continue;
			if (targ.getChannels().empty())	{ names += it->first; continue; }
			for (set<string>::iterator chIt = targ.getChannels().begin(); chIt != targ.getChannels().end(); ++chIt) {
				log(INFO) << "Checking channel " << *chIt;
				if (Server::instance().getChannel(*chIt).getMode().find("s") != string::npos) { names += it->first; break; }
			}
		}
		size_t	len = names.length(), pos = 0, i;
		do {
			if (!EN_EMPTY_NAMES && names.empty())	break;
			i = len > size ? names.find_last_of(' ', pos + size + 1) : len;
			if (i == string::npos)	i = names.find(' ', pos + size);
			if (i == string::npos || i == pos - 1)	i = len;
			client.dispatch(MESSAGE(SERVER_NAME, RPL_NAMREPLY(client.getNick(), "*", "*", names.substr(pos, i - pos))));
			pos = i + 1;
		} while (pos < len);
		client.dispatch(MESSAGE(SERVER_NAME, RPL_ENDOFNAMES(client.getNick(), "*")));
	} else {
		// Loop through channels
		string			channel;
		stringstream	ssChan(params[0]);
		while (getline(ssChan, channel, ',')) {
			// Check channel name validity
			if (channel.empty())	continue;

			// Get existing channel
			if (Server::instance().hasChannel(channel)) {
				Channel&	chan = Server::instance().getChannel(channel);
				if (chan.getMode().find("s") == string::npos || chan.hasClient(client.getSocket())) {
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
				}
			}
			client.dispatch(MESSAGE(SERVER_NAME, RPL_ENDOFNAMES(client.getNick(), channel)));
		}
	}
}