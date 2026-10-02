#include "../../include/Server.hpp"

void Server::_executeKick(int clientFd, const std::vector<std::string>& args)
{
	if (args.size() < 2)
	{
		_sendNumericReply(clientFd, "461", "KICK :Not enough parameters");
		return;
	}
	std::string channelName = args[0];
	std::string targetNick = args[1];
	std::string reason = (args.size() > 2) ? args[2] : "No reason specified";
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
	if (targetFd == -1 || !chanIt->second.isMember(targetFd))
	{
		_sendNumericReply(clientFd, "441", targetNick + " " + channelName + " :They aren't on that channel");
		return;
	}

	Client& operatorUser = _clients.at(clientFd);
	std::string kickMsg = ":" + operatorUser.getNickname() + "!" + operatorUser.getUsername()
		+ "@localhost KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";
	_broadcastChannel(chanIt->second, kickMsg);
	chanIt->second.removeMember(targetFd);
	std::cout << "User " << targetNick << " was kicked from " << channelName
		<< " by " << operatorUser.getNickname() << std::endl;
}
