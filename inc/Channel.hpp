#pragma once

#include "Client.hpp"

#include <map>
#include <set>
#include <string>
#include <iostream>

using std::map;
using std::set;
using std::cout;
using std::endl;
using std::string;

#define EN_LIST_MODE	true
#define EN_EMPTY_NAMES	true
#define EN_OFF_INVLIST	true

#define CHANNELLEN	32
#define CHANTYPES	"#"
#define CHANLIMIT	"#:"
#define CHANMODE	"imnstlk"
#define CHANMODE_A	""
#define CHANMODE_B	"k"
#define CHANMODE_C	"l"
#define CHANMODE_D	"imnst"
#define CHANMODES	CHANMODE_A "," CHANMODE_B "," CHANMODE_C "," CHANMODE_D
#define MODE_INIT	"nst"
#define MEMBERMODE	"ov"
#define PREFIX		"(" MEMBERMODE ")@+"
#define STATUSMSG	"@+"
#define KEYLEN		32
#define TOPICLEN	320

#define REGULAR	1
#define VOICED	2
#define OPER	4

class Channel {
	private:
		size_t		limit;
		time_t		created, topicSetat;
		string		key, name, mode, topic, topicNick;
		set<int>	invited;
		map<int, int>	clients;

	public:
		Channel(string const& name);

		time_t	getCreated() const;
		time_t	getTopicSetat() const;

		size_t	getSize() const;
		size_t	getLimit() const;

		string const&	getMode() const;
		string const&	getKey() const;
		string const&	getName() const;
		string const&	getTopic() const;
		string const&	getTopicNick() const;

		string	getNames(int who) const;
		bool	toggleMode(char mode, bool on, string const& param = "");
		void	setTopic(string const& nick, string const& topic);

		bool	isInvited(int id) const;
		void	inviteClient(int id);

		bool	hasClient(int id) const;
		void	addClient(int id, int type = REGULAR);
		void	removeClient(int id);
		map<int, int> const&	getClients() const;

		int		getClientType(int id) const;
		void	setClientType(int id, int type);

		void	broadcast(string const& msg, int id = 0, int type = REGULAR);
};