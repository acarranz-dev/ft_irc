/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/02 22:15:06 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/20 20:42:43 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <cctype>
#include <csignal>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

Server::Server(int port, const std::string& password)
: _port(port), _password(password), _socketFd(-1), _running(false) {}

Server::~Server() {
	_running = false;

	for (std::map<int, Client>::iterator it = _clients.begin();
		 it != _clients.end(); ++it)
		close(it->first);

	if (_socketFd != -1)
		close(_socketFd);
}

void Server::setNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	
	if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1){
		close(_socketFd);
		_socketFd = -1;
		throw std::runtime_error("Error setting fcntl() to non-blocking mode.");
	}
}

void Server::addPollFd(int fd, short events) {

	struct pollfd pFd;

	pFd.fd = fd;
	pFd.events = events;
	pFd.revents = 0;
	_pollFds.push_back(pFd);
}

void Server::removePollFd(int fd) {

	for (std::vector<pollfd>::iterator it = _pollFds.begin();
	it != _pollFds.end(); ++it) {
		if (it->fd == fd) {
			_pollFds.erase(it);
			return;
		}
	}
}

void Server::updatePollFd(int fd) {
	for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
		if (it->fd == fd) {
			it->events = POLLIN;
			std::map<int, Client>::iterator c = _clients.find(fd);
			if (c != _clients.end() && !c->second.output.empty()) {
				it->events |= POLLOUT;
				return;
			}
		}
	}
}

void Server::init() {

	_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFd == -1)
		throw std::runtime_error("Error creating server socket.");

	int opt = 1;
	if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {

	}

	setNonBlocking(_socketFd);

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(_port);

	if (bind(_socketFd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		close(_socketFd);
		_socketFd = -1;
		throw std::runtime_error("Error in bind(): The port may already be in use.");
	}

	if (listen(_socketFd, SOMAXCONN) == -1) {
		close(_socketFd);
		_socketFd = -1;
		throw std::runtime_error("Error in listen().");
	}

	std::signal(SIGPIPE, SIG_IGN);

	addPollFd(_socketFd, POLLIN);

	_running = true;

	std::cout << "IRC server listening on port "
			  << _port << std::endl;
}

void Server::run() {
	std::cout << "Server running. Waiting for events..." << std::endl;
	while (_running) {
		int pollCount = poll(&_pollFds[0], _pollFds.size(), -1);

		if (pollCount == -1) {
			if (errno == EINTR)
				continue;
			throw std::runtime_error("Error in poll() call.");
		}

		/*
		 * Snapshot the ready events. acceptClient() and closeClient()
		 * modify _pollFds, so do not iterate that vector while mutating it.
		 */
		std::vector<pollfd> ready(_pollFds);

		for (std::vector<pollfd>::const_iterator it = ready.begin(); it != ready.end(); ++it) {

			int fd = it->fd;
			short events = it->revents;

			if (events == 0)
				continue;

			if (fd == _socketFd) {
				if (events & POLLIN)
					handleNewClient();
				continue;
			}

			if (_clients.find(fd) == _clients.end())
				continue;

			if (events & (POLLERR | POLLHUP | POLLNVAL)) {
				handleCloseClient(fd);
				continue;
			}

			if (events & POLLIN) {
				handleReadClient(fd);
				if (_clients.find(fd) == _clients.end())
					continue;
			}

			if (events & POLLOUT)
				handleWriteClient(fd);
		}
	}
}

void Server::handleNewClient() {
	while (true) {
		int fd = accept(_socketFd, NULL, NULL);

		if (fd == -1){
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return;
			if (errno == EINTR)
				continue;
			std::cerr << "accept failed: "
					  << std::strerror(errno) << std::endl;
			return;
		}

		try	{
			setNonBlocking(fd);
			_clients[fd] = Client(fd);
			addPollFd(fd, POLLIN);
			queueMessage(fd, ":ircserv NOTICE * :Connected. Send PASS, NICK and USER.");
		} catch (...) {
			close(fd);
			throw;
		}
	}
}

void Server::handleReadClient(int fd) {
	char buffer[4096];

	while (true) {
		ssize_t n = recv(fd, buffer, sizeof(buffer), 0);

		if (n > 0) {
			Client& client = _clients[fd];
			client.input.append(buffer, static_cast<std::string::size_type>(n));

			if (client.input.size() > 65536) {
				handleCloseClient(fd);
				return;
			}

			processInput(fd);

			if (_clients.find(fd) == _clients.end())
				return;
		} else if (n == 0) {
			handleCloseClient(fd);
			return;
		} else {
			if (errno == EINTR)
				continue;
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			handleCloseClient(fd);
			return;
		}
	}
}

void Server::handleWriteClient(int fd) {
	std::map<int, Client>::iterator it = _clients.find(fd);
	if (it == _clients.end())
	return;

	std::string& out = it->second.output;

	while (!out.empty()) {
		ssize_t n = send(fd, out.data(), out.size(), 0);

		if (n > 0)
			out.erase(0, static_cast<std::string::size_type>(n));
		else if (n == -1 && errno == EINTR)
			continue;
		else if (n == -1 &&
				 (errno == EAGAIN || errno == EWOULDBLOCK))
			break;
		else {
			handleCloseClient(fd);
			return;
		}
	}

	updatePollFd(fd);
}

void Server::queueMessage(int fd, const std::string& message) {
	std::map<int, Client>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return;

	it->second.output += message;
	it->second.output += "\r\n";
	updatePollFd(fd);
}

void Server::handleCloseClient(int fd) {
	std::map<int, Client>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return;

	for (std::map<std::string, Channel>::iterator ch = _channels.begin(); ch != _channels.end();) {
		ch->second.members.erase(fd);
		ch->second.operators.erase(fd);
		ch->second.invited.erase(fd);

		if (ch->second.members.empty())	{
			std::map<std::string, Channel>::iterator doomed = ch++;
			_channels.erase(doomed);
		} else 
			++ch;
	}

	close(fd);
	removePollFd(fd);
	_clients.erase(it);
}

void Server::processInput(int fd) {

	std::map<int, Client>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return;

	std::string& input = it->second.input;
	std::string::size_type pos;

	while ((pos = input.find('\n')) != std::string::npos) {
		std::string line = input.substr(0, pos);
		input.erase(0, pos + 1);

		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);

		if (!line.empty())
			handleCommand(fd, line);

		if (_clients.find(fd) == _clients.end())
			return;
	}
}

void Server::sendNumericReply(int fd, const std::string& code, const std::string& msg) {
	std::map<int, Client>::iterator it = _clients.find(fd);
	if (it == _clients.end()) return;
	std::string nick = it->second.nick.empty() ? "*" : it->second.nick;
	queueMessage(fd, ":ircserv " + code + " " + nick + " " + msg);
}

void Server::handleCommand(int fd, const std::string& line) {
	std::istringstream stream(line);
	std::string cmd;
	stream >> cmd;
	for (std::string::size_type i = 0; i < cmd.size(); ++i)
		cmd[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(cmd[i])));

	std::vector<std::string> args;
	std::string token;
	while (stream >> token) {
		if (!token.empty() && token[0] == ':') {
			std::string trailing;
			std::getline(stream, trailing);
			args.push_back(token.substr(1) + trailing);
			break;
		}
		args.push_back(token);
	}
	Client& c = _clients[fd];

	if (cmd == "PASS") cmdPass(c, args);
	else if (cmd == "NICK") cmdNick(c, args);
	else if (cmd == "USER") cmdUser(c, args);
	else if (cmd == "JOIN") cmdJoin(c, args);
	else if (cmd == "PART") cmdPart(c, args);
	else if (cmd == "TOPIC") cmdTopic(c, args);
	else if (cmd == "KICK") cmdKick(c, args);
	else if (cmd == "INVITE") cmdInvite(c, args);
	else if (cmd == "MODE") cmdMode(c, args);
	else if (cmd == "PRIVMSG") cmdPrivmsg(c, args);
	else if (cmd == "QUIT") cmdQuit(c, args);

	/*
	 * Complete registration once PASS, NICK and USER have arrived.
	 */
	if (!c.registered && c.passOK && !c.nick.empty() && !c.user.empty()) {
		c.registered = true;
		queueMessage(fd, ":ircserv 001 " + c.nick + " :Welcome to the IRC server");
	}
}
