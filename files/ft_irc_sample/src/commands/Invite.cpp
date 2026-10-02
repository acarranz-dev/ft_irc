#include "../../include/Server.hpp"

void Server::_executeInvite(int clientFd, const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		_sendNumericReply(clientFd, "461", "INVITE :Not enough parameters");
		return;
	}
	std::string targetNick = args[0];
	std::string channelName = args[1];
	std::map<std::string, Channel>::iterator chanIt = _channels.find(channelName);
	if (chanIt == _channels.end())
	{
		_sendNumericReply(clientFd, "403", channelName + " :No such channel");
		return;
	}
	if (!chanIt->second.isMember(clientFd))
	{
		_sendNumericReply(clientFd, "442", channelName + " :You're not on that channel");
		return;
	}
	if (!chanIt->second.isOperator(clientFd))
	{
		_sendNumericReply(clientFd, "482", channelName + " :You're not channel operator");
		return;
	}

	int targetFd = -1;
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second.getNickname() == targetNick)
		{
			targetFd = it->first;
			break;
		}
	}
	if (targetFd == -1)
	{
		_sendNumericReply(clientFd, "401", targetNick + " :No such nick/channel");
		return;
	}
	if (chanIt->second.isMember(targetFd))
	{
		_sendNumericReply(clientFd, "443", targetNick + " " + channelName + " :is already on channel");
		return;
	}

	chanIt->second.addInvitation(targetNick);
	_sendNumericReply(clientFd, "341", targetNick + " " + channelName);
	Client& sender = _clients.at(clientFd);
	std::string inviteMsg = ":" + sender.getNickname() + "!" + sender.getUsername()
		+ "@localhost INVITE " + targetNick + " :" + channelName + "\r\n";
	_queueMessage(targetFd, inviteMsg);
	std::cout << sender.getNickname() << " invited " << targetNick << " to join " << channelName << std::endl;
}
