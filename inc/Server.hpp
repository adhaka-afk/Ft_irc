#pragma once

#include "Client.hpp"
#include "Channel.hpp"
#include "Logger.hpp"
#include "ICommand.hpp"

#include <map>
#include <set>
#include <string>
#include <vector>
#include <iomanip>
#include <utility>
#include <iostream>
#include <algorithm>

#include <poll.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define HOSTLEN			64
#define KICKLEN			320
#define MAXMSG			512
#define SERVER_NAME		"irc.42.fr.chat"
#define MAX_CONNECTIONS	1024
#define DIS_JOIN_0		false
#define SUPPORTED_0		"AWAYLEN=" << AWAYLEN << " CASEMAPPING=ascii CHANLIMIT=" << CHANLIMIT << " CHANMODES=" << CHANMODES << " CHANNELLEN=" << CHANNELLEN << "CHANTYPES=" << CHANTYPES
#define SUPPORTED_1		"DEAF=" << DEAF << " HOSTLEN=" << HOSTLEN << " KEYLEN=" << KEYLEN << " KICKLEN=" << KICKLEN << " MAXCHANNELS= MAXNICKLEN=" << NICKLEN << "MODES= NETWORK=42.FR NICKLEN=" << NICKLEN
#define SUPPORTED_2		"PREFIX=" << PREFIX << " SAFELIST STATUSMSG=" << STATUSMSG << " TARGMAX=JOIN:,KICK:,LIST:,NAMES:,NOTICE:,PART:,PRIVMSG: TOPICLEN=" << TOPICLEN << " USERLEN=" << USERLEN

class Client;
class Channel;

class Server {
	private:
		Server();
		Server(Server const&);
		Server& operator=(Server const&);
		~Server();

		vector<pollfd>			fds;
		map<string, int>		nicks;
		map<int, Client>		clients;
		map<string, Channel>	channels;
		string	port, password;
		int		sockFd;
		bool	ready;
		time_t	created;

		static void	intHandler(int signum);
		void	console();
		void	attach();
		void	detach(int i);

	public:
		class InvalidArgumentException : public exception {
			public:
				virtual char const *what(void) const throw() {
					return "Given argument is invalid.";
				}
		};
		class RuntimeException : public exception {
			public:
				virtual char const *what(void) const throw() {
					return "Fatal error occured.";
				}
		};

		static Server&	instance();

		void	init(string const& port, string const& password);
		void	start();
		void	stop();
		void	invoke(Client& client, string& msg);
		void	broadcast(string const& msg, int id = 0);
		void 	parse(string& msg, string& command, vector<string>& params);

		bool	authenticate(string const& pass) const;
		bool	updateNick(Client const& client, string const& nick);

		string	getCreated() const;

		bool		hasClient(int id) const;
		bool		hasClient(string const& nick) const;
		bool		hasChannel(string const& name) const;

		Client&		getClient(int id);
		Client&		getClient(string const& nick);
		Channel&	getChannel(string const& name);

		map<string, int> const& 	getNicks() const;
		map<string, Channel> const&	getChannels() const;

		void	addChannel(string const& name);
		void	removeChannel(string const& name);
};