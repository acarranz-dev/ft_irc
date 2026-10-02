#include "../../include/Server.hpp"

void Server::_executePing(int clientFd, const std::vector<std::string>& args)
{
	if (args.empty())
	{
		_sendNumericReply(clientFd, "409", ":No origin specified");
		return;
	}
	_queueMessage(clientFd, ":localhost PONG localhost :" + args[0] + "\r\n");
}
