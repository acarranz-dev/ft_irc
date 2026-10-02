#include "../../include/Server.hpp"

void Server::_executeCap(int clientFd, const std::vector<std::string>& args)
{
	std::string nick = _clients.at(clientFd).getNickname();
	if (nick.empty())
		nick = "*";
	if (args.empty())
		return;

	std::string sub = args[0];
	for (size_t i = 0; i < sub.size(); ++i)
		sub[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(sub[i])));

	if (sub == "LS")
		_queueMessage(clientFd, ":localhost CAP " + nick + " LS :\r\n");
	else if (sub == "REQ")
	{
		std::string requested = (args.size() > 1) ? args[1] : "";
		_queueMessage(clientFd, ":localhost CAP " + nick + " NAK :" + requested + "\r\n");
	}
	// CAP END intentionally needs no reply.
}
