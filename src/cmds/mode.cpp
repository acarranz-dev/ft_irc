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
	std::map<std::string, Channel>::iterator ch = _channels.find(chName);
	if (ch == _channels.end()) {
		sendNumericReply(c.fd, "403", chName + " :No such channel");
		return;
	}
	if (!ch->second.operators.count(c.fd)) {
		sendNumericReply(c.fd, "482", chName + " :You are not channel operator");
		return;
	}
	if (args.size() < 2) {
		std::string cur = "+";
		if (ch->second.inviteOnly)
			cur += "i";
		if (ch->second.topicOps)
			cur += "t";
		std::ostringstream oss;
		oss << ch->second.userLimit;
		cur += "l" + oss.str();
		sendNumericReply(c.fd, "324", chName + " " + cur);
		return;
	}
	std::string flags = args[1];
	bool adding = true;
	for (std::string::size_type i = 0; i < flags.size(); ++i) {
		char f = flags[i];
		if (f == '+') {
			adding = true;
			continue;
		}
		if (f == '-') {
			adding = false;
			continue;
		}
		if (f == 'i') {
			ch->second.inviteOnly = adding;
			continue;
		}
		if (f == 't') {
			ch->second.topicOps = adding;
			continue;
		}
		if (f == 'k' && i + 1 < flags.size()) {
			ch->second.key = flags[++i];
			continue;
		}
		if (f == 'l') {
			ch->second.userLimit = adding ? 10 : 0;
			continue;
		}
		if (f == 'o' && args.size() > 2) {
			for (std::map<int, Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
				if (it->second.nick != args[2])
					continue;
				if (adding)
					ch->second.operators.insert(it->first);
				else
					ch->second.operators.erase(it->first);
				break;
			}
		}
	}
}
