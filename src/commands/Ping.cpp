#include "ICommand.hpp"

void	Ping::execute(Client& client, vector<string>& params) {
	log(DEBUG) << "Executing PING command";

	// Check if registered
	if (!client.isRegistered())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOTREGISTERED(client.getNick())));
	// Check empty parameters
	if (params.empty() || params[0].empty())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NEEDMOREPARAMS(client.getNick(), "PING")));
	// Check server name
	if (params.size() > 1 && params[1] != SERVER_NAME)
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOSUCHSERVER(client.getNick(), SERVER_NAME)));
	client.dispatch(MESSAGE(SERVER_NAME, "PONG " + SERVER_NAME + " :" + params[0]));
}

void	Pong::execute(Client& client, vector<string>& params) {
	(void)client;
	(void)params;
	log(DEBUG) << "Executing PONG command";
}