#include "ICommand.hpp"

void	Topic::execute(Client& client, vector<string>& params) {
	log(DEBUG) << "Executing TOPIC command";

	// Check if registered
	if (!client.isRegistered())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOTREGISTERED(client.getNick())));
	// Check empty parameters
	if (params.empty() || params[0].empty())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NEEDMOREPARAMS(client.getNick(), "TOPIC")));

	// Check channel
	if (!Server::instance().hasChannel(params[0]))
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOSUCHCHANNEL(client.getNick(), params[0])));

	Channel&	chan = Server::instance().getChannel(params[0]);
	if (!chan.hasClient(client.getSocket()))
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOTONCHANNEL(client.getNick(), params[0])));

	if (params.size() == 1) {
		if (chan.getTopic().empty())
			return client.dispatch(MESSAGE(SERVER_NAME, RPL_NOTOPIC(client.getNick(), params[0])));
		stringstream	ss;
		ss << chan.getTopicSetat();
		client.dispatch(MESSAGE(SERVER_NAME, RPL_TOPIC(client.getNick(), params[0], chan.getTopic())));
		client.dispatch(MESSAGE(SERVER_NAME, RPL_TOPICWHOTIME(client.getNick(), params[0], chan.getTopicNick(), ss.str())));
		return;
	}

	if (chan.getMode().find("t") != string::npos && chan.getClientType(client.getSocket()) < OPER)
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_CHANOPRIVSNEEDED(client.getNick(), params[0])));
	string topic = params[1].substr(0, TOPICLEN);
	chan.setTopic(client.getNick(), topic);
	chan.broadcast(MESSAGE(NICKNAME(client.getNick(), client.getUser(), client.getHostname()), "TOPIC " + params[0] + " :" + topic));
}