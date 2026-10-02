#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <set>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <arpa/inet.h>
#include <map>

class Server
{
	private:
		int _port;
		std::string _password;
		int _serverSocketFd;
		std::vector<struct pollfd> _pollFds;
		std::map<int, Client> _clients;
		std::map<std::string, Channel> _channels;

		Server(const Server& src);
		Server& operator=(const Server& src);

		void _handleNewConnection();
		void _handleClientData(size_t index);
		void _flushClientOutput(size_t index);
		void _disconnectClient(size_t index);
		void _leaveAllChannels(int clientFd);
		void _processCommand(int clientFd, const std::string& command);

		void _queueMessage(int clientFd, const std::string& message);
		void _broadcastChannel(Channel& channel, const std::string& message, int excludeFd = -1);
		void _sendNumericReply(int clientFd, const std::string& numeric, const std::string& message);
		void _sendNames(int clientFd, Channel& channel);
		void _tryRegister(int clientFd);
		bool _isValidNickname(const std::string& nick) const;
		std::string _truncateNickname(const std::string& nick) const;
		void _joinOneChannel(int clientFd, const std::string& requestedName, const std::string& key);
		std::vector<std::string> _splitCommaList(const std::string& value) const;

		void _executePass(int clientFd, const std::vector<std::string>& args);
		void _executeNick(int clientFd, const std::vector<std::string>& args);
		void _executeUser(int clientFd, const std::vector<std::string>& args);
		void _executeJoin(int clientFd, const std::vector<std::string>& args);
		void _executePrivmsg(int clientFd, const std::vector<std::string>& args);
		void _executeTopic(int clientFd, const std::vector<std::string>& args);
		void _executeKick(int clientFd, const std::vector<std::string>& args);
		void _executeInvite(int clientFd, const std::vector<std::string>& args);
		void _executeMode(int clientFd, const std::vector<std::string>& args);
		void _executePing(int clientFd, const std::vector<std::string>& args);
		void _executeCap(int clientFd, const std::vector<std::string>& args);
		void _executeNames(int clientFd, const std::vector<std::string>& args);
		void _executeWho(int clientFd, const std::vector<std::string>& args);

	public:
		Server(int port, const std::string& password);
		~Server();
		void init();
		void run();
};

#endif
