#include "../../include/Server.hpp"

void Server::_joinOneChannel(int clientFd, const std::string& requestedName, const std::string& key)
{
	if (requestedName.empty())
		return;
	std::string channelName = requestedName;
	if (channelName[0] != '#')
		channelName = "#" + channelName;

	Client& client = _clients.at(clientFd);
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it != _channels.end() && it->second.isMember(clientFd))
	{
		_sendNumericReply(clientFd, "443", client.getNickname() + " " + channelName + " :is already on channel");
		return;
	}

	if (it == _channels.end())
	{
		_channels.insert(std::make_pair(channelName, Channel(channelName)));
		it = _channels.find(channelName);
		it->second.addMember(&client);
		it->second.addOperator(&client);
		std::cout << "Channel " << channelName << " created by fd " << clientFd << " (Operator)." << std::endl;
	}
	else
	{
		if (it->second.isInviteOnly() && !it->second.isInvited(client.getNickname()))
		{
			_sendNumericReply(clientFd, "473", channelName + " :Cannot join channel (+i)");
			return;
		}
		if (!it->second.getKey().empty() && key != it->second.getKey())
		{
			_sendNumericReply(clientFd, "475", channelName + " :Cannot join channel (+k) - bad key");
			return;
		}
		if (it->second.getUserLimit() > 0 && it->second.getMemberCount() >= it->second.getUserLimit())
		{
			_sendNumericReply(clientFd, "471", channelName + " :Cannot join channel (+l)");
			return;
		}
		it->second.addMember(&client);
		it->second.removeInvitation(client.getNickname());
	}

	std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost JOIN :" + channelName + "\r\n";
	_broadcastChannel(it->second, joinMsg);
	if (it->second.getTopic().empty())
		_sendNumericReply(clientFd, "331", channelName + " :No topic is set");
	else
		_sendNumericReply(clientFd, "332", channelName + " :" + it->second.getTopic());
	_sendNames(clientFd, it->second);
}

void Server::_executeJoin(int clientFd, const std::vector<std::string>& args)
{
	if (args.empty())
	{
		_sendNumericReply(clientFd, "461", "JOIN :Not enough parameters");
		return;
	}
	std::vector<std::string> channels = _splitCommaList(args[0]);
	std::vector<std::string> keys;
	if (args.size() > 1)
		keys = _splitCommaList(args[1]);

	for (size_t i = 0; i < channels.size(); ++i)
	{
		std::string key = (i < keys.size()) ? keys[i] : "";
		_joinOneChannel(clientFd, channels[i], key);
	}
}
