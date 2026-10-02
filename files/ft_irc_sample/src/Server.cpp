#include "../include/Server.hpp"

Server::Server(int port, const std::string& password)
	: _port(port), _password(password), _serverSocketFd(-1) {}

Server::~Server()
{
	for (size_t i = 0; i < _pollFds.size(); ++i)
	{
		if (_pollFds[i].fd >= 0)
			close(_pollFds[i].fd);
	}
	_pollFds.clear();
	_serverSocketFd = -1;
	std::cout << "Server closed successfully." << std::endl;
}

void Server::init()
{
	_serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocketFd == -1)
		throw std::runtime_error("Error creating server socket.");

	int opt = 1;
	if (setsockopt(_serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		close(_serverSocketFd);
		_serverSocketFd = -1;
		throw std::runtime_error("Error in setsockopt().");
	}
	if (fcntl(_serverSocketFd, F_SETFL, O_NONBLOCK) == -1)
	{
		close(_serverSocketFd);
		_serverSocketFd = -1;
		throw std::runtime_error("Error setting fcntl() to non-blocking mode.");
	}

	struct sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(_port);

	if (bind(_serverSocketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
	{
		close(_serverSocketFd);
		_serverSocketFd = -1;
		throw std::runtime_error("Error in bind(): The port may already be in use.");
	}
	if (listen(_serverSocketFd, SOMAXCONN) == -1)
	{
		close(_serverSocketFd);
		_serverSocketFd = -1;
		throw std::runtime_error("Error in listen().");
	}

	struct pollfd serverPollFd;
	serverPollFd.fd = _serverSocketFd;
	serverPollFd.events = POLLIN;
	serverPollFd.revents = 0;
	_pollFds.push_back(serverPollFd);

	std::cout << "Server listening on port " << _port << "..." << std::endl;
}

void Server::run()
{
	std::cout << "Server running. Waiting for events..." << std::endl;
	while (true)
	{
		int pollCount = poll(&_pollFds[0], _pollFds.size(), -1);
		if (pollCount == -1)
			throw std::runtime_error("Error in poll() call.");

		for (int i = static_cast<int>(_pollFds.size()) - 1; i >= 0; --i)
		{
			short revents = _pollFds[i].revents;
			if (revents == 0)
				continue;

			if (_pollFds[i].fd == _serverSocketFd)
			{
				if (revents & POLLIN)
					_handleNewConnection();
				continue;
			}

			if (revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				_disconnectClient(i);
				continue;
			}

			if (revents & POLLIN)
			{
				size_t oldSize = _pollFds.size();
				_handleClientData(i);
				if (_pollFds.size() != oldSize)
					continue;
			}
			if (i < static_cast<int>(_pollFds.size()) && (_pollFds[i].revents & POLLOUT))
				_flushClientOutput(i);
		}
	}
}

void Server::_handleNewConnection()
{
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientFd = accept(_serverSocketFd, (struct sockaddr*)&clientAddr, &clientAddrLen);
	if (clientFd == -1)
	{
		std::cerr << "Error accepting new connection." << std::endl;
		return;
	}
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Error in fcntl() for new client." << std::endl;
		close(clientFd);
		return;
	}

	struct pollfd clientPollFd;
	clientPollFd.fd = clientFd;
	clientPollFd.events = POLLIN;
	clientPollFd.revents = 0;
	_pollFds.push_back(clientPollFd);
	_clients.insert(std::make_pair(clientFd, Client(clientFd)));

	std::cout << "New connection accepted from IP: " << inet_ntoa(clientAddr.sin_addr)
			  << " on fd: " << clientFd << std::endl;
}

void Server::_handleClientData(size_t index)
{
	int clientFd = _pollFds[index].fd;
	char buffer[1024];
	int bytesReceived = recv(clientFd, buffer, sizeof(buffer), 0);
	if (bytesReceived == 0)
	{
		std::cout << "Client on fd " << clientFd << " disconnected." << std::endl;
		_disconnectClient(index);
		return;
	}
	if (bytesReceived < 0)
		return;

	_clients.at(clientFd).appendToBuffer(std::string(buffer, bytesReceived));
	std::string& clientBuffer = _clients.at(clientFd).getBuffer();
	size_t pos;
	while ((pos = clientBuffer.find('\n')) != std::string::npos)
	{
		std::string command = clientBuffer.substr(0, pos);
		if (!command.empty() && command[command.size() - 1] == '\r')
			command.erase(command.size() - 1);
		clientBuffer.erase(0, pos + 1);
		if (!command.empty())
		{
			std::cout << "Processing from fd " << clientFd << ": [" << command << "]" << std::endl;
			_processCommand(clientFd, command);
		}
	}
}

void Server::_flushClientOutput(size_t index)
{
	int clientFd = _pollFds[index].fd;
	std::map<int, Client>::iterator it = _clients.find(clientFd);
	if (it == _clients.end())
		return;
	std::string& out = it->second.getOutBuffer();
	if (out.empty())
	{
		_pollFds[index].events &= ~POLLOUT;
		return;
	}

	ssize_t sent = send(clientFd, out.c_str(), out.size(), 0);
	if (sent < 0)
	{
		_disconnectClient(index);
		return;
	}
	if (sent > 0)
		out.erase(0, static_cast<size_t>(sent));
	if (out.empty())
		_pollFds[index].events &= ~POLLOUT;
}

void Server::_disconnectClient(size_t index)
{
	if (index >= _pollFds.size())
		return;
	int clientFd = _pollFds[index].fd;
	_leaveAllChannels(clientFd);
	close(clientFd);
	_clients.erase(clientFd);
	_pollFds.erase(_pollFds.begin() + index);
}

void Server::_leaveAllChannels(int clientFd)
{
	std::map<std::string, Channel>::iterator it = _channels.begin();
	while (it != _channels.end())
	{
		if (it->second.isMember(clientFd))
			it->second.removeMember(clientFd);
		if (it->second.getMemberCount() == 0)
		{
			std::map<std::string, Channel>::iterator doomed = it++;
			_channels.erase(doomed);
		}
		else
			++it;
	}
}

void Server::_queueMessage(int clientFd, const std::string& message)
{
	std::map<int, Client>::iterator clientIt = _clients.find(clientFd);
	if (clientIt == _clients.end())
		return;
	clientIt->second.appendToOutBuffer(message);
	for (size_t i = 0; i < _pollFds.size(); ++i)
	{
		if (_pollFds[i].fd == clientFd)
		{
			_pollFds[i].events |= POLLOUT;
			break;
		}
	}
}

void Server::_broadcastChannel(Channel& channel, const std::string& message, int excludeFd)
{
	std::vector<int> members = channel.getMemberFds();
	for (size_t i = 0; i < members.size(); ++i)
	{
		if (members[i] != excludeFd)
			_queueMessage(members[i], message);
	}
}

void Server::_sendNumericReply(int clientFd, const std::string& numeric, const std::string& message)
{
	std::string nick = _clients.at(clientFd).getNickname();
	if (nick.empty())
		nick = "*";
	_queueMessage(clientFd, ":localhost " + numeric + " " + nick + " " + message + "\r\n");
}

void Server::_sendNames(int clientFd, Channel& channel)
{
	_sendNumericReply(clientFd, "353", "= " + channel.getName() + " :" + channel.getNamesList());
	_sendNumericReply(clientFd, "366", channel.getName() + " :End of /NAMES list");
}

void Server::_tryRegister(int clientFd)
{
	Client& client = _clients.at(clientFd);
	if (!client.isRegistered() && client.hasValidPassword()
		&& !client.getNickname().empty() && !client.getUsername().empty())
	{
		client.setRegistered(true);
		_sendNumericReply(clientFd, "001", ":Welcome to the Internet Relay Network " + client.getNickname());
		std::cout << "Client fd " << clientFd << " registered successfully." << std::endl;
	}
}

std::vector<std::string> Server::_splitCommaList(const std::string& value) const
{
	std::vector<std::string> result;
	size_t start = 0;
	while (start <= value.size())
	{
		size_t comma = value.find(',', start);
		if (comma == std::string::npos)
		{
			result.push_back(value.substr(start));
			break;
		}
		result.push_back(value.substr(start, comma - start));
		start = comma + 1;
	}
	return result;
}

void Server::_processCommand(int clientFd, const std::string& commandLine)
{
	if (commandLine.empty())
		return;

	std::stringstream ss(commandLine);
	std::string token;
	std::vector<std::string> args;
	ss >> token;
	for (size_t i = 0; i < token.length(); ++i)
		token[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(token[i])));
	std::string cmd = token;

	while (ss >> token)
	{
		if (!token.empty() && token[0] == ':')
		{
			std::string trailing;
			std::getline(ss, trailing);
			args.push_back(token.substr(1) + trailing);
			break;
		}
		args.push_back(token);
	}

	std::cout << "Executing command [" << cmd << "] with " << args.size() << " arguments." << std::endl;

	if (cmd == "CAP")
		_executeCap(clientFd, args);
	else if (cmd == "PING")
		_executePing(clientFd, args);
	else if (cmd == "PASS")
		_executePass(clientFd, args);
	else if (cmd == "NICK")
		_executeNick(clientFd, args);
	else if (cmd == "USER")
		_executeUser(clientFd, args);
	else if (cmd == "JOIN" || cmd == "PRIVMSG" || cmd == "TOPIC" || cmd == "KICK"
		|| cmd == "INVITE" || cmd == "MODE" || cmd == "NAMES" || cmd == "WHO")
	{
		if (!_clients.at(clientFd).isRegistered())
		{
			_sendNumericReply(clientFd, "451", ":You have not registered");
			return;
		}
		if (cmd == "JOIN") _executeJoin(clientFd, args);
		else if (cmd == "PRIVMSG") _executePrivmsg(clientFd, args);
		else if (cmd == "TOPIC") _executeTopic(clientFd, args);
		else if (cmd == "KICK") _executeKick(clientFd, args);
		else if (cmd == "INVITE") _executeInvite(clientFd, args);
		else if (cmd == "MODE") _executeMode(clientFd, args);
		else if (cmd == "NAMES") _executeNames(clientFd, args);
		else if (cmd == "WHO") _executeWho(clientFd, args);
	}
	else if (cmd == "QUIT")
	{
		// The socket will normally close immediately after /QUIT from the client.
		_queueMessage(clientFd, ":localhost ERROR :Closing Link\r\n");
	}
	else
	{
		if (!_clients.at(clientFd).isRegistered())
			_sendNumericReply(clientFd, "451", ":You have not registered");
		else
			_sendNumericReply(clientFd, "421", cmd + " :Unknown command");
	}
}
