#include "ICommand.hpp"

void	Notice::execute(Client& client, vector<string>& params) {
	log(DEBUG) << "Executing NOTICE command";

	if (!client.isRegistered() || params.size() < 2 || params[1].empty())	return;

	// Loop through targets
	string			target;
	stringstream	ssTarget(params[0]);
	while (getline(ssTarget, target, ',')) {
		if (target.empty())	continue;
		if (string(STATUSMSG CHANTYPES).find(target[0]) == string::npos) {
			if (!Server::instance().hasClient(target))	return;
			Server::instance().getClient(target).dispatch(MESSAGE(NICKNAME(client.getNick(), client.getUser(), client.getHostname()), "NOTICE " + target + " :" + params[1]));
		} else {
			string	toType = target.substr(0, target.find_first_of(CHANTYPES)), channel = target.substr(target.find_first_of(CHANTYPES));
			if (!Server::instance().hasChannel(channel) || toType.find_first_not_of(STATUSMSG) != string::npos)
				return;
			Channel&	chan = Server::instance().getChannel(channel);
			if ((chan.getMode().find("n") != string::npos && !chan.hasClient(client.getSocket()))
				|| (chan.getMode().find("m") != string::npos && chan.getClientType(client.getSocket()) < VOICED))
				return;
			int	type;
			switch(toType.find_first_of(STATUSMSG)) {
				case 0:		type = OPER;	break;
				case 1:		type = VOICED;	break;
				default:	type = REGULAR;	break;
			}
			chan.broadcast(MESSAGE(NICKNAME(client.getNick(), client.getUser(), client.getHostname()), "NOTICE " + target + " :" + params[1]), client.getSocket(), type);
		}
	}
}