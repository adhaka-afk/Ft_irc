#include "ICommand.hpp"

void	Part::execute(Client& client, vector<string>& params) {
	log(DEBUG) << "Executing PART command";

	// Check if registered
	if (!client.isRegistered())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOTREGISTERED(client.getNick())));
	// Check empty parameters
	if (params.empty() || params[0].empty())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NEEDMOREPARAMS(client.getNick(), "PART")));

	// Loop through channels
	string			channel;
	stringstream	ssChan(params[0]);
	while (getline(ssChan, channel, ',')) {
		// Check channel name validity
		if (channel.empty())	continue;
		if (string(CHANTYPES).find(channel[0]) == string::npos || !Server::instance().hasChannel(channel)) {
			client.dispatch(MESSAGE(SERVER_NAME, ERR_NOSUCHCHANNEL(client.getNick(), channel)));
			continue;
		}

		// Get existing channel
		Channel&	chan = Server::instance().getChannel(channel);
		if (!chan.hasClient(client.getSocket())) {
			client.dispatch(MESSAGE(SERVER_NAME, ERR_NOTONCHANNEL(client.getNick(), channel)));
			continue;
		}
		// Remove client from channel
		chan.broadcast(MESSAGE(NICKNAME(client.getNick(), client.getUser(), client.getHostname()), "PART " + channel + (params.size() > 1 ? " :" + params[1] : "")));
		chan.removeClient(client.getSocket());
		client.removeChannel(channel);
	}
}