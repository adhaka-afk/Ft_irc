#include "ICommand.hpp"

void	User::execute(Client& client, vector<string>& params) {
	log(DEBUG) << "Executing USER command";

	// Check if registered
	if (client.isRegistered())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_ALREADYREGISTRED(client.getNick())));
	// Check empty parameters
	if (params.size() < 4)
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NEEDMOREPARAMS(client.getNick(), "USER")));
	// Set user
	string	user = params[0].substr(0, params[0].find_first_of("@")).substr(0, USERLEN);
	if (user.empty())
		return client.dispatch(MESSAGE(SERVER_NAME, "NOTICE " + client.getNick() + " :*** Invalid username, please make sure username does not contain '@'"));
	client.setUser(user);
	client.setReal(params[3]);
	client.registrate();
}