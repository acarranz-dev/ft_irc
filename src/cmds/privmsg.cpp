/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 18:32:01 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/19 18:32:02 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::cmdPrivmsg(Client& c, const std::vector<std::string>& args) {
	if (!c.registered) {
		sendNumericReply(c.fd, "451", ":You have not registered");
		return;
	}
	if (args.size() < 2 || args[0].empty()) {
		sendNumericReply(c.fd, "461", "PRIVMSG :Not enough parameters");
		return;
	}
	std::string msg = args[1];
	std::string prefix = ":" + c.nick + "!" + c.user + "@localhost";
	if (args[0][0] == '#') {
		std::map<std::string, Channel>::iterator ch = _channels.find(args[0]);
		if (ch == _channels.end() || !ch->second.members.count(c.fd)) {
			sendNumericReply(c.fd, "404", args[0] + " :Cannot send to channel");
			return;
		}
		for (std::set<int>::const_iterator m = ch->second.members.begin(); m != ch->second.members.end(); ++m) {
			if (*m != c.fd)
				queueMessage(*m, prefix + " PRIVMSG " + args[0] + " :" + msg);
		}
		return;
	}
	bool found = false;
	for (std::map<int, Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second.nick != args[0])
			continue;
		queueMessage(it->first, prefix + " PRIVMSG " + args[0] + " :" + msg);
		found = true;
		break;
	}
	if (!found)
		sendNumericReply(c.fd, "401", args[0] + " :No such nick");
}
