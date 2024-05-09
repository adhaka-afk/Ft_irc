#include "ICommand.hpp"

void	Nick::execute(Client& client, vector<string>& params) {
	log(DEBUG) << "Executing NICK command";

	// Check empty parameters
	if (params.empty() || params[0].empty())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NONICKNAMEGIVEN(client.getNick())));
	// Check valid nickname
	string	nick = params[0].substr(0, NICKLEN);
	for (size_t i = 0; i < nick.size(); i++) {
		if (isdigit(nick[0]) || nick[0] == '-'
			|| (string(SPECIAL).find(nick[i]) == string::npos
			    && !isalnum(nick[i]) && nick[i] != '-'))
			return client.dispatch(MESSAGE(SERVER_NAME, ERR_ERRONEUSNICKNAME(client.getNick(), nick)));
	}
	// Check nickname used
	if (!Server::instance().updateNick(client, nick))
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NICKNAMEINUSE(client.getNick(), nick)));
	// Set nickname
	if (client.isRegistered())
		client.announce(MESSAGE(NICKNAME(client.getNick(), client.getUser(), client.getHostname()), "NICK :" + nick), true);
	client.setNick(nick);
	client.registrate();
}