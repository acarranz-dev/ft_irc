#include "../../include/Server.hpp"

void Server::_executeNames(int clientFd, const std::vector<std::string>& args)
{
	if (args.empty())
	{
		_sendNumericReply(clientFd, "366", "* :End of /NAMES list");
		return;
	}
	std::vector<std::string> channels = _splitCommaList(args[0]);
	for (size_t i = 0; i < channels.size(); ++i)
	{
		std::map<std::string, Channel>::iterator it = _channels.find(channels[i]);
		if (it != _channels.end())
			_sendNames(clientFd, it->second);
		else
			_sendNumericReply(clientFd, "366", channels[i] + " :End of /NAMES list");
	}
}
