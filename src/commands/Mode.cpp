#include "ICommand.hpp"

void	Mode::execute(Client& client, vector<string>& params) {
	log(DEBUG) << "Executing MODE command";

	// Check if registered
	if (!client.isRegistered())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOTREGISTERED(client.getNick())));
	// Check empty parameters
	if (params.empty())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NEEDMOREPARAMS(client.getNick(), "MODE")));

	bool	isNick = string(CHANTYPES).find(params[0][0]) == string::npos;
	if (isNick) {
		if (!Server::instance().hasClient(params[0]))
			return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOSUCHNICK(client.getNick(), params[0])));
		if (params[0] != client.getNick())
			return client.dispatch(MESSAGE(SERVER_NAME, ERR_USERSDONTMATCH(client.getNick())));
		if (params.size() < 2)
			return client.dispatch(MESSAGE(SERVER_NAME, RPL_UMODEIS(client.getNick(), client.getMode())));
	} else {
		if (!Server::instance().hasChannel(params[0]))
			return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOSUCHCHANNEL(client.getNick(), params[0])));

		Channel&	chan = Server::instance().getChannel(params[0]);
		if (params.size() < 2) {
			string	mode = chan.getMode(), msg = RPL_CHANNELMODEIS(client.getNick(), params[0], mode);
			if (chan.hasClient(client.getSocket())) {
				for (size_t i = 0; i < mode.size(); i++) {
					if (mode[i] == 'l')	{
						stringstream ss;
						ss << chan.getLimit();
						msg += " " + ss.str();
					}
					if (mode[i] == 'k')	msg += " " + chan.getKey();
				}
			}
			stringstream	ss;
			ss << chan.getCreated();
			client.dispatch(MESSAGE(SERVER_NAME, msg));
			client.dispatch(MESSAGE(SERVER_NAME, RPL_CREATIONTIME(client.getNick(), params[0], ss.str())));
			return;
		}
		if (!chan.hasClient(client.getSocket()) || chan.getClientType(client.getSocket()) < OPER)
			return client.dispatch(MESSAGE(SERVER_NAME, ERR_CHANOPRIVSNEEDED(client.getNick(), params[0])));
	}

	size_t	off = 0;
	bool	add = true, unknown = false, arg, res;
	multimap<char, string>	added, removed;
	for (size_t i = 0; i < params[1].size(); i++) {
		log(DEBUG) << "Processing char " << params[1][i];
		arg = false;
		if (params[1][i] == '+')	{ add = true; continue; }
		if (params[1][i] == '-')	{ add = false; continue; }
		if (isNick && string(USERMODE).find(params[1][i]) == string::npos) {
			if (!unknown) {
				client.dispatch(MESSAGE(SERVER_NAME, ERR_UMODEUNKNOWNFLAG(client.getNick())));
				unknown = true;
			}
			continue;
		}
		if (!isNick && string(CHANMODE MEMBERMODE).find(params[1][i]) == string::npos) {
			client.dispatch(MESSAGE(SERVER_NAME, ERR_UNKOWNMODE(client.getNick(), params[1][i])));
			continue;
		}
		if ((added.find(params[1][i]) != added.end() || removed.find(params[1][i]) != removed.end())
			&& string(MEMBERMODE).find(params[1][i]) == string::npos)
			continue;
		if (isNick)	res = client.toggleMode(params[1][i], add);
		else {
			Channel&	chan = Server::instance().getChannel(params[0]);
			res = chan.toggleMode(params[1][i], add, params.size() > 2 + off ? (params[1][i] == 'k' && !add ? "*" : params[2 + off]) : "");
			// Channel modes group C
			if (string(CHANMODE_C).find(params[1][i]) != string::npos && add)	arg = true;
			// Channel modes group B
			if (string(CHANMODE_B).find(params[1][i]) != string::npos) {
				arg = true;
				if (add && params.size() > 2 + off && params[2 + off].find(" ")	!= string::npos)
					client.dispatch(MESSAGE(SERVER_NAME, ERR_INVALIDKEY(client.getNick(), params[0])));
			}
			// Channel membership modes
			if (string(MEMBERMODE).find(params[1][i]) != string::npos) {
				arg = true;
				if (!Server::instance().hasClient(params[2 + off])) {
					client.dispatch(MESSAGE(SERVER_NAME, ERR_NOSUCHNICK(client.getNick(), params[2 + off])));
				} else if (!chan.hasClient(Server::instance().getClient(params[2 + off]).getSocket()))
					client.dispatch(MESSAGE(SERVER_NAME, ERR_USERNOTINCHANNEL(client.getNick(), params[0], params[2 + off])));
			}
		}
		if (res)	(add ? added : removed).insert(make_pair(params[1][i], arg ? params[2 + off] : ""));
		off += arg;
	}

	// Broadcast mode change
	string	args, mode;
	for (multimap<char, string>::iterator it = removed.begin(); it != removed.end(); ++it) {
		if (it == removed.begin())	mode += "-";
		mode += it->first;
		if (isNick)	continue;
		if (!args.empty())	args += " ";
		if (it->first == 'k')	args += "*";
		if (string(PREFIX).find(it->first) != string::npos)	args += it->second;
	}
	for (multimap<char, string>::iterator it = added.begin(); it != added.end(); ++it) {
		if (it == added.begin())	mode += "+";
		mode += it->first;
		if (isNick)	continue;
		Channel&	chan = Server::instance().getChannel(params[0]);
		if (!args.empty())	args += " ";
		if (it->first == 'l') {
			stringstream ss;
			ss << (long)chan.getLimit();
			args += ss.str();
		}
		if (it->first == 'k')	args += chan.getKey();
		if (string(PREFIX).find(it->first) != string::npos)	args += it->second;
	}
	if (added.empty() && removed.empty())	return;
	if (isNick)	return client.dispatch(MESSAGE(NICKNAME(client.getNick(), string(), string()), "MODE " + client.getNick() + " :" + mode));
	Channel&	chan = Server::instance().getChannel(params[0]);
	log(INFO) << "Broadcasting mode change " << mode << " " << args;
	chan.broadcast(MESSAGE(NICKNAME(client.getNick(), client.getUser(), client.getHostname()), "MODE " + params[0] + " " + mode + (args.empty() ? "" : " " + args)));
}