#include "ICommand.hpp"

void	Kick::execute(Client& client, vector<string>& params) {
	log(DEBUG) << "Executing KICK command";

	// Check if registered
	if (!client.isRegistered())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOTREGISTERED(client.getNick())));
	// Check empty parameters
	if (params.size() < 2 || params[2].empty())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NEEDMOREPARAMS(client.getNick(), "KICK")));

	// Check channel name validity
	string	channel = params[0].substr(0, params[0].find_first_of(","));
	if (!Server::instance().hasChannel(channel))
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOSUCHCHANNEL(client.getNick(), channel)));

	Channel&	chan = Server::instance().getChannel(channel);
	if (!chan.hasClient(client.getSocket()))
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOTONCHANNEL(client.getNick(), channel)));
	if (chan.getClientType(client.getSocket()) < OPER)
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_CHANOPRIVSNEEDED(client.getNick(), channel)));

	string			nick, reason = params.size() > 2 ? params[2].substr(0, KICKLEN) : "";
	stringstream	ssNick(params[1]);
	while (getline(ssNick, nick, ',')) {
		if (nick.empty())	continue;
		if (!Server::instance().hasClient(nick)) {
			client.dispatch(MESSAGE(SERVER_NAME, ERR_NOSUCHNICK(client.getNick(), nick)));
			continue;
		}
		Client&	target = Server::instance().getClient(nick);
		if (!chan.hasClient(target.getSocket())) {
			client.dispatch(MESSAGE(SERVER_NAME, ERR_USERNOTINCHANNEL(client.getNick(), nick, channel)));
			continue;
		}
		chan.removeClient(target.getSocket());
		target.removeChannel(channel);
		target.dispatch(MESSAGE(NICKNAME(client.getNick(), client.getUser(), client.getHostname()), "KICK " + channel + " " + nick + (reason.empty() ? " :" + reason : "")));
	}
}