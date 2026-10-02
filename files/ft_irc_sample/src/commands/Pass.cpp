#include "../../include/Server.hpp"

void Server::_executePass(int clientFd, const std::vector<std::string>& args)
{
	Client& client = _clients.at(clientFd);
	if (client.isRegistered()) 
	{
		_sendNumericReply(clientFd, "462", ":You may not reregister");
		return;
	}
	if (args.empty()) 
	{
		_sendNumericReply(clientFd, "461", "PASS :Not enough parameters");
		return;
	}
	if (args[0] != _password) 
	{
		_sendNumericReply(clientFd, "464", ":Password incorrect");
		std::cout << "fd " << clientFd << " failed authentication (Incorrect password)." << std::endl;
		return;
	}
	client.setValidPassword(true);	
	std::cout << "fd " << clientFd << " passed password check successfully." << std::endl;
}