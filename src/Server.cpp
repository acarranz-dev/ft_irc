/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/02 22:15:06 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/02 22:15:06 by sarodrig         ###   ########.fr       */
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

/* -------------------------------------------------------------------------- */
/* Client and Channel                                                         */
/* -------------------------------------------------------------------------- */
Client::Client() 
: passOK(false), registered(false) {
}

Channel::Channel()
: inviteOnly(false), topicOps(false), userLimit(0) {
}

/* -------------------------------------------------------------------------- */
/* Server                                                                     */
/* -------------------------------------------------------------------------- */
Server::Server(int port, const std::string& password)
: _port(port), _serverFd(-1), _password(password), _running(false) {
}

Server::~Server() {
	_running = false;

	for (std::map<int, Client>::iterator it = _clients.begin();
		 it != _clients.end(); ++it)
		close(it->first);

	if (_serverFd != -1)
		close(_serverFd);
}

// void Server::setNonBlocking(int fd) {
// 	int flags = fcntl(fd, F_GETFL, 0);
	
// 	if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
// 		throw std::runtime_error("fcntl failed");
// }

void Server::init() {
	// _serverFd = socket(AF_INET, SOCK_STREAM, 0);
	// if (_serverFd == -1)
	// 	throw std::runtime_error("socket failed");

	// int yes = 1;
	// if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
	// 	throw std::runtime_error("setsockopt failed");

	// sockaddr_in addr;
	// std::memset(&addr, 0, sizeof(addr));
	// addr.sin_family = AF_INET;
	// addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// addr.sin_port = htons(_port);

	// if (bind(_serverFd, (sockaddr *)&addr, sizeof(addr)) == -1)
	// 	throw std::runtime_error("bind failed");

	// if (listen(_serverFd, SOMAXCONN) == -1)
	// 	throw std::runtime_error("listen failed");

	// try {
	// 	setNonBlocking(_serverFd);
	// }
	// catch (...) {
	// 	throw;
	// }

	// std::signal(SIGPIPE, SIG_IGN);

	// addPollFd(_serverFd, POLLIN);
	// _running = true;

	// std::cout << "IRC server listening on port "
	// 		  << _port << std::endl;
}

// void Server::addPollFd(int fd, short events) {
// 	pollfd p;
// 	p.fd = fd;
// 	p.events = events;
// 	p.revents = 0;
// 	_pollFds.push_back(p);
// }

// void Server::removePollFd(int fd) {
// 	for (std::vector<pollfd>::iterator it = _pollFds.begin();
// 	it != _pollFds.end(); ++it) {
// 		if (it->fd == fd) {
// 			_pollFds.erase(it);
// 			return;
// 		}
// 	}
// }

// void Server::updatePollEvents(int fd) {
// 	for (std::vector<pollfd>::iterator it = _pollFds.begin();
// 	it != _pollFds.end(); ++it) {
// 		if (it->fd == fd) {
// 			it->events = POLLIN;
// 			std::map<int, Client>::iterator c = _clients.find(fd);
// 			if (c != _clients.end() && !c->second.output.empty()) {
// 				it->events |= POLLOUT;
// 				return;
// 			}
// 		}
// 	}
// }

void Server::run() {
// 	while (_running) {
// 		int result = poll(&_pollFds[0], _pollFds.size(), -1);

// 		if (result == -1) {
// 			if (errno == EINTR)
// 				continue;
// 			throw std::runtime_error("poll failed");
// 		}

// 		/*
// 		 * Snapshot the ready events. acceptClient() and closeClient()
// 		 * modify _pollFds, so do not iterate that vector while mutating it.
// 		 */
// 		std::vector<pollfd> ready(_pollFds);

// 		for (std::vector<pollfd>::const_iterator it = ready.begin();
// 			 it != ready.end(); ++it) {
// 			int fd = it->fd;
// 			short events = it->revents;

// 			if (events == 0)
// 				continue;

// 			if (fd == _serverFd) {
// 				if (events & POLLIN)
// 					acceptClient();
// 				continue;
// 			}

// 			if (_clients.find(fd) == _clients.end())
// 				continue;

// 			if (events & (POLLERR | POLLHUP | POLLNVAL)) {
// 				closeClient(fd);
// 				continue;
// 			}

// 			if (events & POLLIN) {
// 				readClient(fd);
// 				if (_clients.find(fd) == _clients.end())
// 					continue;
// 			}

// 			if (events & POLLOUT)
// 				writeClient(fd);
// 		}
// 	}
// }

// void Server::acceptClient() {
// 	while (true) {
// 		int fd = accept(_serverFd, NULL, NULL);

// 		if (fd == -1){
// 			if (errno == EAGAIN || errno == EWOULDBLOCK)
// 				return;
// 			if (errno == EINTR)
// 				continue;
// 			std::cerr << "accept failed: "
// 					  << std::strerror(errno) << std::endl;
// 			return;
// 		}

// 		try	{
// 			setNonBlocking(fd);
// 			_clients[fd] = Client();
// 			addPollFd(fd, POLLIN);
// 			queueMessage(fd, ":ircserv NOTICE * :Connected. Send PASS, NICK and USER.");
// 		} catch (...) {
// 			close(fd);
// 			throw;
// 		}
// 	}
}

// void Server::readClient(int fd) {
// 	char buffer[4096];

// 	while (true) {
// 		ssize_t n = recv(fd, buffer, sizeof(buffer), 0);

// 		if (n > 0) {
// 			Client& client = _clients[fd];
// 			client.input.append(buffer, static_cast<std::string::size_type>(n));

// 			if (client.input.size() > 65536) {
// 				closeClient(fd);
// 				return;
// 			}

// 			processInput(fd);

// 			if (_clients.find(fd) == _clients.end())
// 				return;
// 		} else if (n == 0) {
// 			closeClient(fd);
// 			return;
// 		} else {
// 			if (errno == EINTR)
// 				continue;
// 			if (errno == EAGAIN || errno == EWOULDBLOCK)
// 				break;
// 			closeClient(fd);
// 			return;
// 		}
// 	}
// }

// void Server::writeClient(int fd) {
// 	std::map<int, Client>::iterator it = _clients.find(fd);
// 	if (it == _clients.end())
// 	return;

// 	std::string& out = it->second.output;

// 	while (!out.empty()) {
// 		ssize_t n = send(fd, out.data(), out.size(), 0);

// 		if (n > 0)
// 			out.erase(0, static_cast<std::string::size_type>(n));
// 		else if (n == -1 && errno == EINTR)
// 			continue;
// 		else if (n == -1 &&
// 				 (errno == EAGAIN || errno == EWOULDBLOCK))
// 			break;
// 		else {
// 			closeClient(fd);
// 			return;
// 		}
// 	}

// 	updatePollEvents(fd);
// }

// void Server::queueMessage(int fd, const std::string& message) {
// 	std::map<int, Client>::iterator it = _clients.find(fd);
// 	if (it == _clients.end())
// 		return;

// 	it->second.output += message;
// 	it->second.output += "\r\n";
// 	updatePollEvents(fd);
// }

// void Server::closeClient(int fd) {
// 	std::map<int, Client>::iterator it = _clients.find(fd);
// 	if (it == _clients.end())
// 		return;


// for (std::map<std::string, Channel>::iterator ch = _channels.begin();
// 	 ch != _channels.end(); )
// {
// 	ch->second.members.erase(fd);
// 	ch->second.operators.erase(fd);
// 	ch->second.invited.erase(fd);

// 	if (ch->second.members.empty())
// 	{
// 		std::map<std::string, Channel>::iterator doomed = ch++;
// 		_channels.erase(doomed);
// 	}
// 	else
// 		++ch;
// }

// close(fd);
// removePollFd(fd);
// _clients.erase(it);


// }

// void Server::processInput(int fd)
// {
// std::map<int, Client>::iterator it = _clients.find(fd);
// if (it == _clients.end())
// return;


// std::string& input = it->second.input;
// std::string::size_type pos;

// while ((pos = input.find('\n')) != std::string::npos)
// {
// 	std::string line = input.substr(0, pos);
// 	input.erase(0, pos + 1);

// 	if (!line.empty() && line[line.size() - 1] == '\r')
// 		line.erase(line.size() - 1);

// 	if (!line.empty())
// 		handleCommand(fd, line);

// 	if (_clients.find(fd) == _clients.end())
// 		return;
// }


// }

// /* -------------------------------------------------------------------------- */
// /* Minimal IRC command handling                                               */
// /* -------------------------------------------------------------------------- */

// void Server::handleCommand(int fd, const std::string& line)
// {
// std::istringstream stream(line);
// std::string cmd;
// stream >> cmd;


// for (std::string::size_type i = 0; i < cmd.size(); ++i)
// 	cmd[i] = static_cast<char>(
// 		std::toupper(static_cast<unsigned char>(cmd[i])));

// std::vector<std::string> args;
// std::string token;

// while (stream >> token)
// {
// 	if (!token.empty() && token[0] == ':')
// 	{
// 		std::string trailing;
// 		std::getline(stream, trailing);
// 		args.push_back(token.substr(1) + trailing);
// 		break;
// 	}
// 	args.push_back(token);
// }

// Client& c = _clients[fd];

// if (cmd == "QUIT")
// {
// 	closeClient(fd);
// 	return;
// }
// else if (cmd == "PASS")
// {
// 	if (args.empty())
// 		queueMessage(fd, ":ircserv 461 * PASS :Not enough parameters");
// 	else if (args[0] != _password)
// 		queueMessage(fd, ":ircserv 464 * :Password incorrect");
// 	else
// 		c.passOK = true;
// }
// else if (cmd == "NICK")
// {
// 	if (args.empty())
// 	{
// 		queueMessage(fd, ":ircserv 431 * :No nickname given");
// 		return;
// 	}

// 	for (std::map<int, Client>::const_iterator it = _clients.begin();
// 		 it != _clients.end(); ++it)
// 	{
// 		if (it->first != fd && it->second.nick == args[0])
// 		{
// 			queueMessage(fd, ":ircserv 433 * " + args[0] +
// 						 " :Nickname is already in use");
// 			return;
// 		}
// 	}

// 	c.nick = args[0];
// }
// else if (cmd == "USER")
// {
// 	if (args.size() < 4)
// 	{
// 		queueMessage(fd, ":ircserv 461 * USER :Not enough parameters");
// 		return;
// 	}
// 	c.user = args[0];
// }
// else if (cmd == "PING")
// {
// 	if (!args.empty())
// 		queueMessage(fd, ":ircserv PONG ircserv :" + args[0]);
// }
// else if (cmd == "JOIN")
// {
// 	if (!c.registered)
// 	{
// 		queueMessage(fd, ":ircserv 451 * :You have not registered");
// 		return;
// 	}
// 	if (args.empty())
// 	{
// 		queueMessage(fd, ":ircserv 461 " + c.nick +
// 					 " JOIN :Not enough parameters");
// 		return;
// 	}

// 	std::string name = args[0];
// 	if (name.empty() || name[0] != '#')
// 	{
// 		queueMessage(fd, ":ircserv 476 " + c.nick + " " + name +
// 					 " :Bad Channel Mask");
// 		return;
// 	}

// 	Channel& ch = _channels[name];
// 	if (ch.members.insert(fd).second && ch.members.size() == 1)
// 		ch.operators.insert(fd);

// 	std::string prefix = ":" + c.nick + "!" + c.user + "@localhost";
// 	for (std::set<int>::const_iterator m = ch.members.begin();
// 		 m != ch.members.end(); ++m)
// 		queueMessage(*m, prefix + " JOIN " + name);

// 	if (!ch.topic.empty())
// 		queueMessage(fd, ":ircserv 332 " + c.nick + " " +
// 					 name + " :" + ch.topic);

// 	std::ostringstream names;
// 	for (std::set<int>::const_iterator m = ch.members.begin();
// 		 m != ch.members.end(); ++m)
// 	{
// 		std::map<int, Client>::const_iterator member = _clients.find(*m);
// 		if (member != _clients.end())
// 		{
// 			if (m != ch.members.begin())
// 				names << " ";
// 			if (ch.operators.count(*m))
// 				names << "@";
// 			names << member->second.nick;
// 		}
// 	}
// 	queueMessage(fd, ":ircserv 353 " + c.nick + " = " +
// 				 name + " :" + names.str());
// 	queueMessage(fd, ":ircserv 366 " + c.nick + " " +
// 				 name + " :End of NAMES list");
// }
// else if (cmd == "PRIVMSG")
// {
// 	if (!c.registered)
// 	{
// 		queueMessage(fd, ":ircserv 451 * :You have not registered");
// 		return;
// 	}
// 	if (args.size() < 2 || args[0].empty())
// 	{
// 		queueMessage(fd, ":ircserv 461 " + c.nick +
// 					 " PRIVMSG :Not enough parameters");
// 		return;
// 	}

// 	std::string prefix = ":" + c.nick + "!" + c.user + "@localhost";
// 	std::string message = prefix + " PRIVMSG " +
// 						  args[0] + " :" + args[1];

// 	if (args[0][0] == '#')
// 	{
// 		std::map<std::string, Channel>::iterator ch =
// 			_channels.find(args[0]);
// 		if (ch == _channels.end() || !ch->second.members.count(fd))
// 		{
// 			queueMessage(fd, ":ircserv 404 " + c.nick + " " +
// 						 args[0] + " :Cannot send to channel");
// 			return;
// 		}
// 		for (std::set<int>::const_iterator m = ch->second.members.begin();
// 			 m != ch->second.members.end(); ++m)
// 			if (*m != fd)
// 				queueMessage(*m, message);
// 	}
// 	else
// 	{
// 		bool found = false;
// 		for (std::map<int, Client>::const_iterator it = _clients.begin();
// 			 it != _clients.end(); ++it)
// 		{
// 			if (it->second.nick == args[0])
// 			{
// 				queueMessage(it->first, message);
// 				found = true;
// 				break;
// 			}
// 		}
// 		if (!found)
// 			queueMessage(fd, ":ircserv 401 " + c.nick + " " +
// 						 args[0] + " :No such nick");
// 	}
// }
// else if (!cmd.empty())
// 	queueMessage(fd, ":ircserv 421 " + c.nick + " " +
// 				 cmd + " :Unknown command");

// /*
//  * Complete registration once PASS, NICK and USER have arrived.
//  */
// if (!c.registered && c.passOK && !c.nick.empty() && !c.user.empty())
// {
// 	c.registered = true;
// 	queueMessage(fd, ":ircserv 001 " + c.nick +
// 				 " :Welcome to the IRC server");
// }


// }
