#include "ICommand.hpp"

void	Invite::execute(Client& client, vector<string>& params) {
	log(DEBUG) << "Executing INVITE command";

	// Check if registered
	if (!client.isRegistered())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOTREGISTERED(client.getNick())));
	// Check empty parameters
	if (params.size() < 2 || params[1].empty())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NEEDMOREPARAMS(client.getNick(), "INVITE")));
	if (!Server::instance().hasClient(params[0]))
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOSUCHNICK(client.getNick(), params[0])));
	if (!Server::instance().hasChannel(params[1]))
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOSUCHCHANNEL(client.getNick(), params[1])));

	Channel&	chan = Server::instance().getChannel(params[1]);
	if (!chan.hasClient(client.getSocket()))
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOTONCHANNEL(client.getNick(), params[1])));
	if (chan.getMode().find("i") != string::npos && chan.getClientType(client.getSocket()) < OPER)
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_CHANOPRIVSNEEDED(client.getNick(), params[1])));

	Client&	target = Server::instance().getClient(params[0]);
	if (chan.hasClient(target.getSocket()))
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_USERONCHANNEL(client.getNick(), params[0], params[1])));
	chan.inviteClient(target.getSocket());
	client.dispatch(MESSAGE(SERVER_NAME, RPL_INVITING(client.getNick(), params[0], params[1])));
	target.dispatch(MESSAGE(NICKNAME(client.getNick(), client.getUser(), client.getHostname()), "INVITE " + params[0] + " " + params[1]));
}