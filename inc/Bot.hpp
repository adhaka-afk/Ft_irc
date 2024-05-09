#pragma once

#include "Server.hpp"

#include <string>
#include <iostream>

using namespace std;

class Bot {
	private:
		string	name, channel, pass;
		int		sockFd;

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

		Bot(string const& name, string const& channel, string const& pass);

		void	start(string const& host, string const& port, string const& pass);
};