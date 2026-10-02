#include "../../include/Server.hpp"

void Server::_executeMode(int clientFd, const std::vector<std::string>& args)
{
	if (args.empty())
	{
		_sendNumericReply(clientFd, "461", "MODE :Not enough parameters");
		return;
	}

	std::string target = args[0];
	Client& sender = _clients.at(clientFd);

	// Irssi sends MODE <nick> +i after registration. User modes are not part of
	// the mandatory project, so accept the common +i/-i form as a no-op.
	if (target.empty() || target[0] != '#')
	{
		if (target != sender.getNickname())
		{
			_sendNumericReply(clientFd, "502", ":Cannot change mode for other users");
			return;
		}
		if (args.size() >= 2)
			_queueMessage(clientFd, ":" + sender.getNickname() + " MODE " + target + " " + args[1] + "\r\n");
		return;
	}

	std::map<std::string, Channel>::iterator chanIt = _channels.find(target);
	if (chanIt == _channels.end())
	{
		_sendNumericReply(clientFd, "403", target + " :No such channel");
		return;
	}

	// MODE #channel query must be available to regular members too.
	if (args.size() == 1)
	{
		std::string modes = "+";
		std::string params;
		if (chanIt->second.isInviteOnly()) modes += "i";
		if (chanIt->second.isTopicRestricted()) modes += "t";
		if (!chanIt->second.getKey().empty())
		{
			modes += "k";
			params += " " + chanIt->second.getKey();
		}
		if (chanIt->second.getUserLimit() > 0)
		{
			modes += "l";
			std::stringstream ss;
			ss << chanIt->second.getUserLimit();
			params += " " + ss.str();
		}
		_sendNumericReply(clientFd, "324", target + " " + modes + params);
		return;
	}

	if (!chanIt->second.isOperator(clientFd))
	{
		_sendNumericReply(clientFd, "482", target + " :You're not channel operator");
		return;
	}

	std::string modeString = args[1];
	if (modeString.empty())
		return;

	bool adding = true;
	size_t paramIndex = 2;
	std::string appliedModes;
	std::vector<std::string> appliedParams;

	for (size_t i = 0; i < modeString.size(); ++i)
	{
		char mode = modeString[i];
		if (mode == '+')
		{
			adding = true;
			appliedModes += '+';
			continue;
		}
		if (mode == '-')
		{
			adding = false;
			appliedModes += '-';
			continue;
		}

		if (mode == 'i')
			chanIt->second.setInviteOnly(adding);
		else if (mode == 't')
			chanIt->second.setTopicRestricted(adding);
		else if (mode == 'k')
		{
			if (adding)
			{
				if (paramIndex >= args.size())
				{
					_sendNumericReply(clientFd, "461", "MODE +k :Needs a parameter");
					return;
				}
				chanIt->second.setKey(args[paramIndex]);
				appliedParams.push_back(args[paramIndex++]);
			}
			else
				chanIt->second.setKey("");
		}
		else if (mode == 'l')
		{
			if (adding)
			{
				if (paramIndex >= args.size())
				{
					_sendNumericReply(clientFd, "461", "MODE +l :Needs a parameter");
					return;
				}
				int limit = std::atoi(args[paramIndex].c_str());
				if (limit <= 0)
				{
					_sendNumericReply(clientFd, "461", "MODE +l :Invalid limit");
					return;
				}
				chanIt->second.setUserLimit(static_cast<size_t>(limit));
				appliedParams.push_back(args[paramIndex++]);
			}
			else
				chanIt->second.setUserLimit(0);
		}
		else if (mode == 'o')
		{
			if (paramIndex >= args.size())
			{
				_sendNumericReply(clientFd, "461", "MODE +/-o :Needs a nickname");
				return;
			}
			std::string nick = args[paramIndex++];
			int targetFd = -1;
			for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
			{
				if (it->second.getNickname() == nick)
				{
					targetFd = it->first;
					break;
				}
			}
			if (targetFd == -1 || !chanIt->second.isMember(targetFd))
			{
				_sendNumericReply(clientFd, "441", nick + " " + target + " :They aren't on that channel");
				return;
			}
			if (adding)
				chanIt->second.addOperator(&_clients.at(targetFd));
			else
				chanIt->second.removeOperator(targetFd);
			appliedParams.push_back(nick);
		}
		else
		{
			_sendNumericReply(clientFd, "472", std::string(1, mode) + " :is unknown mode char to me");
			return;
		}
		appliedModes += mode;
	}

	std::string modeMsg = ":" + sender.getNickname() + "!" + sender.getUsername()
		+ "@localhost MODE " + target + " " + appliedModes;
	for (size_t i = 0; i < appliedParams.size(); ++i)
		modeMsg += " " + appliedParams[i];
	modeMsg += "\r\n";
	_broadcastChannel(chanIt->second, modeMsg);
}
