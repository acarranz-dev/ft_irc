/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/02 21:23:32 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/02 21:23:32 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <iostream>
#include "server.hpp"

bool isValidPort(const std::string& portStr) {
	if (portStr.empty())
		return false;

	for (size_t i = 0; i < portStr.length(); i++) {
		if (!std::isdigit(portStr[i]))
			return false;
	}

	int port = std::atoi(portStr.c_str());
	return (port >= 1024 && port <= 65535);
}

int main(int argc, char** argv) {
	if (argc != 3) {
		std::cerr << "Error: Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	if (!isValidPort(argv[1])) {
		std::cerr << "Error: Port must be a number between 1024 and 65535." << std::endl;
		return 1;
	}

	int port = std::atoi(argv[1]);
	std::string password = argv[2];

	if (password.empty()) {
		std::cerr << "Error: Password cannot be empty." << std::endl;
		return 1;
	}

	std::cout << "--- Starting ft_irc Server ---" << std::endl;

	try {
		Server ircServer(port, password);
		ircServer.init();
		ircServer.run();
	} 
	catch (const std::exception& e) {
		std::cerr << "Critical server error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
