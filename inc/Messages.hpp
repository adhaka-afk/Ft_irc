#pragma once

#include <string>

#define SPECIAL		"[]\\`_^'{|}"

#define NICKNAME(nick, user, host)											nick + (user.empty() ? "" : "!" + user) + (host.empty() ? "" : "@" + host)
#define MESSAGE(source, message)											std::string(":") + source + " " + message + "\r\n"

/* Reply codes */
#define RPL_WELCOME(client)													"001 " + client + " :Welcome to the Internet Relay Chat Network " + client
#define RPL_YOURHOST(client, server, version)								"002 " + client + " :Your host is " + server + ", running version " + version
#define RPL_CREATED(client, date)											"003 " + client + " :This server was created at " + date
#define RPL_MYINFO(client, server, version, userModes, channelModes)		"004 " + client + " " + server + " " + version + " " + userModes + " " + channelModes
#define RPL_ISUPPORT(client, options)										"005 " + client + " " + options + " :are supported by this server"

#define RPL_UMODEIS(client, modes)											"221 " + client + " +" + modes

#define RPL_AWAY(client, nick, message)										"301 " + client + " " + nick + " :" + message
#define RPL_UNAWAY(client)													"305 " + client + " :You are no longer marked as being away"
#define RPL_NOWAWAY(client)													"306 " + client + " :You have been marked as being away"

#define RPL_LISTSTART(client)												"321 " + client + " Channel :Users  Name"
#define RPL_LIST(client, channel, count, topic)								"322 " + client + " " + channel + " " + count + " :" + topic
#define RPL_LISTEND(client)													"323 " + client + " :End of /LIST"
#define RPL_CHANNELMODEIS(client, channel, mode)							"324 " + client + " " + channel + " +" + mode
#define RPL_CREATIONTIME(client, channel, created)							"329 " + client + " " + channel + " " + created

#define RPL_NOTOPIC(client, channel)										"331 " + client + " " + channel + " :No topic is set"
#define RPL_TOPIC(client, channel, topic)									"332 " + client + " " + channel + " :" + topic
#define RPL_TOPICWHOTIME(client, channel, nick, setat)						"333 " + client + " " + channel + " " + nick + " " + setat

#define RPL_INVITING(client, nick, channel)									"341 " + client + " " + nick + " " + channel

#define RPL_NAMREPLY(client, symbol, channel, nicks)						"353 " + client + " " + symbol + " " + channel + " :" + nicks

#define RPL_ENDOFNAMES(client, channel)										"366 " + client + " " + channel + " :End of /NAMES list"

/* Error codes */
#define ERR_NOSUCHNICK(client, nick)										"401 " + client + " " + nick + " :No such nick/channel"
#define ERR_NOSUCHSERVER(client, server)									"402 " + client + " " + server + " :No such server"
#define ERR_NOSUCHCHANNEL(client, channel)									"403 " + client + " " + channel + " :No such channel"
#define ERR_CANNOTSENDTOCHAN(client, channel)								"404 " + client + " " + channel + " :Cannot send to channel"
#define ERR_TOOMANYCHANNELS(client, channel)								"405 " + client + " " + channel + " :You have joined too many channels"

#define ERR_NORECIPIENT(client, command)									"411 " + client + " :No recipient given (" + command + ")"
#define ERR_NOTEXTTOSEND(client)											"412 " + client + " :No text to send"
#define ERR_INPUTTOOLONG(client)											"417 " + client + " :Input line too long"

#define ERR_UNKNOWNCOMMAND(client, command)									"421 " + client + " " + command + " :Unknown command"
#define ERR_NOMOTD(client)													"422 " + client + " :MOTD File is missing"

#define ERR_NONICKNAMEGIVEN(client)											"431 " + client + " :No nickname given"
#define ERR_ERRONEUSNICKNAME(client, nick)									"432 " + client + " " + nick + " :Erroneous nickname"
#define ERR_NICKNAMEINUSE(client, nick)										"433 " + client + " " + nick + " :Nickname is already in use"
#define ERR_NICKCOLLISION(client, nick, user, host)							"436 " + client + " " + nick + " :Nickname collision KILL from " + user + "@" + host

#define ERR_USERNOTINCHANNEL(client, nick, channel)							"441 " + client + " " + nick + " " + channel + " :They aren't on that channel"
#define ERR_NOTONCHANNEL(client, channel)									"442 " + client + " " + channel + " :You're not on that channel"
#define ERR_USERONCHANNEL(client, nick, channel)							"443 " + client + " " + nick + " " + channel + " :is already on channel"

#define ERR_NOTREGISTERED(client)											"451 " + client + " :You have not registered"

#define ERR_NEEDMOREPARAMS(client, command)									"461 " + client + " " + command + " :Not enough parameters"
#define ERR_ALREADYREGISTRED(client)										"462 " + client + " :You may not reregister"
#define ERR_PASSWDMISMATCH(client)											"464 " + client + " :Password incorrect"

#define ERR_CHANNELISFULL(client, channel)									"471 " + client + " " + channel + " :Cannot join channel (+l)"
#define ERR_UNKOWNMODE(client, mode)										"472 " + client + " " + mode + " :is unknown mode char to me"
#define ERR_INVITEONLYCHAN(client, channel)									"473 " + client + " " + channel + " :Cannot join channel (+i)"
#define ERR_BADCHANNELKEY(client, channel)									"475 " + client + " " + channel + " :Cannot join channel (+k)"

#define ERR_CHANOPRIVSNEEDED(client, channel)								"482 " + client + " " + channel + " :You're not channel operator"

#define ERR_UMODEUNKNOWNFLAG(client)										"501 " + client + " :Unknown MODE flag"
#define ERR_USERSDONTMATCH(client)											"502 " + client + " :Cant change mode for other users"

#define ERR_INVALIDKEY(client, channel)										"525 " + client + " " + channel + " :Key is not well-formed"

#define ERR_INVALIDMODEPARAM(client, target, mode, parameter, description)	"696 " + client + " " + target + " " + mode + " " + parameter + " :" + description

