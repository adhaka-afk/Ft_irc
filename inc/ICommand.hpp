#pragma once

#include "Client.hpp"
#include "Logger.hpp"
#include "Server.hpp"
#include "Messages.hpp"

#include <string>
#include <vector>
#include <fstream>

using namespace std;

class Server;
class Client;

class ICommand {
	public:
		virtual void execute(Client& client, vector<string>& params) = 0;
};

class Away : public ICommand {
	public:
		void execute(Client& client, vector<string>& params);
};

class Join : public ICommand {
	public:
		void execute(Client& client, vector<string>& params);
};

class Kick : public ICommand {
	public:
		void execute(Client& client, vector<string>& params);
};

class List : public ICommand {
	public:
		void execute(Client& client, vector<string>& params);
};

class Mode : public ICommand {
	public:
		void execute(Client& client, vector<string>& params);
};

class Nick : public ICommand {
	public:
		void execute(Client& client, vector<string>& params);
};

class Part : public ICommand {
	public:
		void execute(Client& client, vector<string>& params);
};

class Pass : public ICommand {
	public:
		void execute(Client& client, vector<string>& params);
};

class Ping : public ICommand {
	public:
		void execute(Client& client, vector<string>& params);
};

class Pong : public ICommand {
	public:
		void execute(Client& client, vector<string>& params);
};

class Quit : public ICommand {
	public:
		void execute(Client& client, vector<string>& params);
};

class User : public ICommand {
	public:
		void execute(Client& client, vector<string>& params);
};

class Names : public ICommand {
	public:
		void execute(Client& client, vector<string>& params);
};

class Topic : public ICommand {
	public:
		void execute(Client& client, vector<string>& params);
};

class Invite : public ICommand {
	public:
		void execute(Client& client, vector<string>& params);
};

class Notice : public ICommand {
	public:
		void execute(Client& client, vector<string>& params);
};

class Privmsg : public ICommand {
	public:
		void execute(Client& client, vector<string>& params);
};