#include "../../include/Server.hpp"

std::string Server::_truncateNickname(const std::string& nick) const
{
	const size_t maxNickLength = 9;
	if (nick.size() <= maxNickLength)
		return nick;
	return nick.substr(0, maxNickLength);
}

bool Server::_isValidNickname(const std::string& nick) const
{
	if (nick.empty())
		return false;

	char first = nick[0];
	bool firstOk = std::isalpha(static_cast<unsigned char>(first))
		|| first == '[' || first == ']' || first == '\\' || first == '`'
		|| first == '_' || first == '^' || first == '{' || first == '|' || first == '}';
	if (!firstOk)
		return false;

	for (size_t i = 1; i < nick.size(); ++i)
	{
		char c = nick[i];
		bool ok = std::isalnum(static_cast<unsigned char>(c))
			|| c == '-' || c == '[' || c == ']' || c == '\\' || c == '`'
			|| c == '_' || c == '^' || c == '{' || c == '|' || c == '}';
		if (!ok)
			return false;
	}
	return true;
}

void Server::_executeNick(int clientFd, const std::vector<std::string>& args)
{
	Client& client = _clients.at(clientFd);
	if (!client.hasValidPassword())
	{
		_sendNumericReply(clientFd, "451", ":You have not registered (Password required)");
		return;
	}
	if (args.empty() || args[0].empty())
	{
		_sendNumericReply(clientFd, "431", ":No nickname given");
		return;
	}

	std::string requestedNick = args[0];
	std::string newNick = _truncateNickname(requestedNick);
	if (!_isValidNickname(newNick))
	{
		_sendNumericReply(clientFd, "432", requestedNick + " :Erroneous Nickname");
		return;
	}

	std::map<int, Client>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->first != clientFd && it->second.getNickname() == newNick)
		{
			_sendNumericReply(clientFd, "433", newNick + " :Nickname is already in use");
			return;
		}
	}

	std::string oldNick = client.getNickname();
	std::string username = client.getUsername();
	bool wasRegistered = client.isRegistered();

	if (oldNick == newNick)
		return;

	client.setNickname(newNick);
	std::cout << "fd " << clientFd << " changed nickname to: " << newNick << std::endl;

	for (std::map<std::string, Channel>::iterator ch = _channels.begin(); ch != _channels.end(); ++ch)
		ch->second.renameInvitation(oldNick, newNick);

	if (wasRegistered && !oldNick.empty())
	{
		std::string prefixUser = username.empty() ? "unknown" : username;
		std::string nickMsg = ":" + oldNick + "!" + prefixUser + "@localhost NICK :" + newNick + "\r\n";
		std::set<int> recipients;
		recipients.insert(clientFd);
		for (std::map<std::string, Channel>::iterator ch = _channels.begin(); ch != _channels.end(); ++ch)
		{
			if (!ch->second.isMember(clientFd))
				continue;
			std::vector<int> members = ch->second.getMemberFds();
			for (size_t i = 0; i < members.size(); ++i)
				recipients.insert(members[i]);
		}
		for (std::set<int>::iterator r = recipients.begin(); r != recipients.end(); ++r)
			_queueMessage(*r, nickMsg);
	}

	_tryRegister(clientFd);
}
