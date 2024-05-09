#include "ICommand.hpp"

void	Pass::execute(Client& client, vector<string>& params) {
	log(DEBUG) << "Executing PASS command";

	// Check if registered
	if (client.isRegistered())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_ALREADYREGISTRED(client.getNick())));
	// Check empty parameters
	if (params.empty())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NEEDMOREPARAMS(client.getNick(), "PASS")));
	client.setPass(params[0]);
	client.registrate();
}