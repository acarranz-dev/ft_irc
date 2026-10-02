#include "../../include/Server.hpp"

void Server::_executeUser(int clientFd, const std::vector<std::string>& args)
{
	Client& client = _clients.at(clientFd);
	if (!client.hasValidPassword())
	{
		_sendNumericReply(clientFd, "451", ":You have not registered (Password required)");
		return;
	}
	if (client.isRegistered())
	{
		_sendNumericReply(clientFd, "462", ":You may not reregister");
		return;
	}
	if (args.size() < 4)
	{
		_sendNumericReply(clientFd, "461", "USER :Not enough parameters");
		return;
	}
	client.setUsername(args[0]);
	std::cout << "fd " << clientFd << " set username to: " << args[0] << std::endl;
	_tryRegister(clientFd);
}
