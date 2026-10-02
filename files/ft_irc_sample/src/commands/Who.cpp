#include "../../include/Server.hpp"

void Server::_executeWho(int clientFd, const std::vector<std::string>& args)
{
	std::string mask = args.empty() ? "*" : args[0];
	std::map<std::string, Channel>::iterator ch = _channels.find(mask);
	if (ch != _channels.end())
	{
		std::vector<int> members = ch->second.getMemberFds();
		for (size_t i = 0; i < members.size(); ++i)
		{
			Client& c = _clients.at(members[i]);
			std::string flags = ch->second.isOperator(members[i]) ? "H@" : "H";
			_sendNumericReply(clientFd, "352", mask + " " + c.getUsername()
				+ " localhost localhost " + c.getNickname() + " " + flags + " :0 " + c.getNickname());
		}
	}
	_sendNumericReply(clientFd, "315", mask + " :End of /WHO list");
}
