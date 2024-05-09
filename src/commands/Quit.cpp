#include "ICommand.hpp"

void	Quit::execute(Client& client, vector<string>& params) {
	log(DEBUG) << "Executing QUIT command";

	string	reason = "Quit: " + (params.empty() ? "Client quit" : params[0]);
	client.announce(MESSAGE(NICKNAME(client.getNick(), client.getUser(), client.getHostname()), "QUIT :" + reason));
	client.dispatch("ERROR :Closing Link: " + NICKNAME(client.getNick(), string(""), client.getHostname()) + " (" + reason + ")");

	set<string>	channels = client.getChannels();
	for (set<string>::iterator it = channels.begin(); it != channels.end(); ++it) {
		client.removeChannel(*it);
		Server::instance().getChannel(*it).removeClient(client.getSocket());
	}
	shutdown(client.getSocket(), SHUT_RDWR);
}