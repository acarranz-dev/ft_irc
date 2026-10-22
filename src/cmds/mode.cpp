/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 18:31:22 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/19 18:31:24 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cstdlib>
#include <sstream>

void Server::cmdMode(Client& c, const std::vector<std::string>& args) {
	if (!c.registered) {
		sendNumericReply(c.fd, "451", ":You have not registered");
		return;
	}
	if (args.empty()) {
		sendNumericReply(c.fd, "461", "MODE :Not enough parameters");
		return;
	}
	std::string chName = args[0];
	if (chName[0] != '#') {
		/* User modes are not supported: answer clients like irssi with an empty mode set */
		if (chName == c.nick)
			sendNumericReply(c.fd, "221", "+");
		else
			sendNumericReply(c.fd, "502", ":Cant change mode for other users");
		return;
	}
	std::map<std::string, Channel>::iterator found = _channels.find(chName);
	if (found == _channels.end()) {
		sendNumericReply(c.fd, "403", chName + " :No such channel");
		return;
	}
	Channel& ch = found->second;
	if (args.size() < 2) {
		std::string modes = "+";
		std::string params;
		if (ch.inviteOnly)
			modes += "i";
		if (ch.topicOps)
			modes += "t";
		if (!ch.key.empty()) {
			modes += "k";
			params += " " + (ch.hasMember(c.fd) ? ch.key : "*");
		}
		if (ch.userLimit > 0) {
			std::ostringstream oss;
			oss << ch.userLimit;
			modes += "l";
			params += " " + oss.str();
		}
		sendNumericReply(c.fd, "324", chName + " " + modes + params);
		return;
	}
	if (args[1] == "b") {
		/* irssi asks for the ban list on join; there are no bans */
		sendNumericReply(c.fd, "368", chName + " :End of channel ban list");
		return;
	}
	if (!ch.isOperator(c.fd)) {
		sendNumericReply(c.fd, "482", chName + " :You're not channel operator");
		return;
	}

	std::string flags = args[1];
	std::vector<std::string>::size_type next = 2;
	bool adding = true;
	char lastSign = 0;
	std::string applied;
	std::string appliedParams;

	for (std::string::size_type i = 0; i < flags.size(); ++i) {
		char f = flags[i];
		if (f == '+' || f == '-') {
			adding = (f == '+');
			continue;
		}
		std::string param;
		if (f == 'i')
			ch.inviteOnly = adding;
		else if (f == 't')
			ch.topicOps = adding;
		else if (f == 'k') {
			if (adding) {
				if (next >= args.size() || args[next].empty()) {
					sendNumericReply(c.fd, "461", "MODE :Not enough parameters");
					continue;
				}
				ch.key = args[next++];
				param = ch.key;
			} else {
				if (next < args.size())
					++next;
				ch.key.clear();
			}
		}
		else if (f == 'l') {
			if (adding) {
				if (next >= args.size()) {
					sendNumericReply(c.fd, "461", "MODE :Not enough parameters");
					continue;
				}
				int limit = std::atoi(args[next++].c_str());
				if (limit <= 0)
					continue;
				ch.userLimit = limit;
				std::ostringstream oss;
				oss << limit;
				param = oss.str();
			} else
				ch.userLimit = 0;
		}
		else if (f == 'o') {
			if (next >= args.size()) {
				sendNumericReply(c.fd, "461", "MODE :Not enough parameters");
				continue;
			}
			std::string nick = args[next++];
			int targetFd = findFdByNick(nick);
			if (targetFd == -1) {
				sendNumericReply(c.fd, "401", nick + " :No such nick/channel");
				continue;
			}
			if (!ch.hasMember(targetFd)) {
				sendNumericReply(c.fd, "441", nick + " " + chName + " :They aren't on that channel");
				continue;
			}
			if (adding)
				ch.addOperator(targetFd);
			else
				ch.removeOperator(targetFd);
			param = nick;
		}
		else {
			sendNumericReply(c.fd, "472", std::string(1, f) + " :is unknown mode char to me");
			continue;
		}
		char sign = adding ? '+' : '-';
		if (sign != lastSign) {
			applied += sign;
			lastSign = sign;
		}
		applied += f;
		if (!param.empty())
			appliedParams += " " + param;
	}
	if (!applied.empty())
		broadcast(ch, prefix(c) + " MODE " + chName + " " + applied + appliedParams, -1);
}
