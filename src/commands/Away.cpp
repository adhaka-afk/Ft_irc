#include "ICommand.hpp"

void	Away::execute(Client& client, vector<string>& params) {
	log(DEBUG) << "Executing AWAY command";

	// Check if registered
	if (!client.isRegistered())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOTREGISTERED(client.getNick())));

	if (params.empty() || params[0].empty()) {
		client.setAway("");
		client.dispatch(MESSAGE(SERVER_NAME, RPL_UNAWAY(client.getNick())));
	} else {
		client.setAway(params[0].substr(0, AWAYLEN));
		client.dispatch(MESSAGE(SERVER_NAME, RPL_NOWAWAY(client.getNick())));
	}
}
