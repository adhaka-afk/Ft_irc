#pragma once

#include "Channel.hpp"
#include "Server.hpp"

#include <set>
#include <string>

#include <sys/socket.h>

#define AWAYLEN		320
#define NICKLEN		16
#define USERLEN		16
#define DEAF		"D"
#define BOT			"B"
#define USERMODE	"Di"
#define UMODE_INIT	"i"

using namespace std;

class Client {
	private:
		bool		registered, bot;
		int			socket, port;
		string		nick, user, real, hostname, pass, mode, away;
		string		iBuff, oBuff;
		set<string>	channels;

	public:
		Client(string const& hostname, int port, int socket);

		int	getPort() const;
		int	getSocket() const;

		string const&	getAway() const;
		string const&	getMode() const;
		string const&	getNick() const;
		string const&	getUser() const;
		string const&	getReal() const;
		string const&	getHostname() const;

		bool	toggleMode(char mode, bool on, string const& param = "");

		void	setAway(string const& away);
		void	setNick(string const& nick);
		void	setUser(string const& user);
		void	setReal(string const& real);
		void	setPass(string const& pass);

		bool	isRegistered() const;

		void	collect();
		void	dispatch();
		void	dispatch(string const& msg);
		void	announce(string const& msg, bool reflect = false);
		void	registrate();

		void	addChannel(string const& channel);
		void	removeChannel(string const& channel);

		set<string> const&	getChannels() const;
};