#include "ICommand.hpp"

void	Privmsg::execute(Client& client, vector<string>& params) {
	log(DEBUG) << "Executing PRIVMSG command";

	// Check if registered
	if (!client.isRegistered())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOTREGISTERED(client.getNick())));
	// Check empty parameters
	if (params.size() < 1 || params[0].empty())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NORECIPIENT(client.getNick(), "PRIVMSG")));
	if (params.size() < 2 || params[1].empty())
		return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOTEXTTOSEND(client.getNick())));

	// File transfer to server
	size_t	start;
	if (params[0] == SERVER_NAME && (start = params[1].find("\x01""DCC")) != string::npos) {
		log(INFO) << "DCC request";
		string			command, msg = params[1].substr(start + 4, params[1].find('\x01', start + 4));
		vector<string>	parameters;
		Server::instance().parse(msg, command, parameters);

		transform(command.begin(), command.end(), command.begin(), ::toupper);
		if (command == "SEND" && parameters.size() >= 3) {
			log(INFO) << "DCC SEND request";

			// if (parameters[0].find('/') == string::npos) {
			// 	log(ERROR) << "Unable to parse filename";
			// 	return;
			// }

			// Port validation
			char *end;
			long portNum = strtol(parameters[2].c_str(), &end, 10);
			if (*end != 0)
				log(WARN) << "Port number contains non-numeric character and set to " << portNum;

			long ipNum = strtol(parameters[1].c_str(), &end, 10);
			if (*end != 0)
				log(WARN) << "IP address contains non-numeric character and set to " << ipNum;

			// Create socket
			log(DEBUG) << "Creating socket";
			int	connFd = socket(AF_INET, SOCK_STREAM, 0);
			if (connFd < 0) {
				log(ERROR) << "Unable to create socket with error " << errno;
				return;
			}
			log(DEBUG) << "Socket created on file descriptor " << connFd;

			// Setup and bind socket to port
			log(DEBUG) << "Binding socket to port";
			sockaddr_in	sin = {};
			sin.sin_family = AF_INET;
			sin.sin_port = htons(portNum);
			sin.sin_addr.s_addr = htonl(ipNum);
			if (connect(connFd, (sockaddr*)&sin, sizeof(sin)) < 0) {
				log(ERROR) << "Unable to bind socket to port with error " << errno;
				return;
			}

			// Receive file
			log(DEBUG) << "Receiving file";
			string filepath = string("./fileshare/").append(parameters[0].substr(parameters[0].find_last_of('/') == string::npos ? 0 : parameters[0].find_last_of('/') + 1));
			ofstream	file(filepath.c_str(), ofstream::out | ofstream::binary);
			if (!file) {
				log(ERROR) << "Unable to create file " << parameters[0] << " with error " << errno;
				return;
			}

			log(INFO) << "Receiving file " << parameters[0];
			char	buffer[1024];
			int		bytes;
			while ((bytes = recv(connFd, buffer, 1024, 0)) > 0)
				file.write(buffer, bytes);
			file.close();
			close(connFd);
			return;
		}

	}

	// Loop through targets
	string			target;
	stringstream	ssTarget(params[0]);
	while (getline(ssTarget, target, ',')) {
		if (target.empty())	continue;

		if (string(STATUSMSG CHANTYPES).find(target[0]) == string::npos) {
			if (!Server::instance().hasClient(target))
				return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOSUCHNICK(client.getNick(), target)));

			Client&	targ = Server::instance().getClient(target);
			if (!targ.getAway().empty())
				return client.dispatch(MESSAGE(SERVER_NAME, RPL_AWAY(client.getNick(), target, targ.getAway())));
			targ.dispatch(MESSAGE(NICKNAME(client.getNick(), client.getUser(), client.getHostname()), "PRIVMSG " + target + " :" + params[1]));
		} else {
			string	toType = target.substr(0, target.find_first_of(CHANTYPES)), channel = target.substr(target.find_first_of(CHANTYPES));
			if (!Server::instance().hasChannel(channel) || toType.find_first_not_of(STATUSMSG) != string::npos)
				return client.dispatch(MESSAGE(SERVER_NAME, ERR_NOSUCHNICK(client.getNick(), target)));

			Channel&	chan = Server::instance().getChannel(channel);
			if ((chan.getMode().find("n") != string::npos && !chan.hasClient(client.getSocket()))
				|| (chan.getMode().find("m") != string::npos && chan.getClientType(client.getSocket()) < VOICED))
				return client.dispatch(MESSAGE(SERVER_NAME, ERR_CANNOTSENDTOCHAN(client.getNick(), target)));

			int	type;
			switch(toType.find_first_of(STATUSMSG)) {
				case 0:		type = OPER;	break;
				case 1:		type = VOICED;	break;
				default:	type = REGULAR;	break;
			}
			chan.broadcast(MESSAGE(NICKNAME(client.getNick(), client.getUser(), client.getHostname()), "PRIVMSG " + target + " :" + params[1]), client.getSocket(), type);
		}
	}
}