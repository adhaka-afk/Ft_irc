#include "ICommand.hpp"

void	List::execute(Client& client, vector<string>& params) {
	log(DEBUG) << "Executing LIST command";

	// Check if registered
	if (!client.isRegistered())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOTREGISTERED(client.getNick())));

	client.dispatch(MESSAGE(SERVER_NAME, RPL_LISTSTART(client.getNick())));
	if (params.empty() || params[0].empty()) {
		for (map<string, Channel>::const_iterator it = Server::instance().getChannels().begin(); it != Server::instance().getChannels().end(); ++it) {
			Channel&	chan = Server::instance().getChannel(it->first);
			if (chan.getMode().find("s") != string::npos && !chan.hasClient(client.getSocket()))	continue;
			string	topic = ((!EN_LIST_MODE || chan.getMode().empty()) ? "" : "[+" + chan.getMode() + "] ") + chan.getTopic();
			stringstream	ss;
			ss << chan.getSize();
			client.dispatch(MESSAGE(SERVER_NAME, RPL_LIST(client.getNick(), it->first, ss.str(), topic)));
		}
	} else {
		string			channel;
		stringstream	ssChan(params[0]);
		while (getline(ssChan, channel, ',')) {
			// Check channel name validity
			if (channel.empty() || !Server::instance().hasChannel(channel))	continue;
			Channel&	chan = Server::instance().getChannel(channel);
			if (chan.getMode().find("s") != string::npos && !chan.hasClient(client.getSocket()))	continue;
			string	topic = ((!EN_LIST_MODE || chan.getMode().empty()) ? "" : "[+" + chan.getMode() + "] ") + chan.getTopic();
			stringstream	ss;
			ss << chan.getSize();
			client.dispatch(MESSAGE(SERVER_NAME, RPL_LIST(client.getNick(), channel, ss.str(), topic)));
		}
	}
	client.dispatch(MESSAGE(SERVER_NAME, RPL_LISTEND(client.getNick())));
}