/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 18:31:13 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/19 18:31:15 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::cmdKick(Client& c, const std::vector<std::string>& args) {
	if (!c.registered) {
		sendNumericReply(c.fd, "451", ":You have not registered");
		return;
	}
	if (args.size() < 2) {
		sendNumericReply(c.fd, "461", "KICK :Not enough parameters");
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
	std::string target = args[1];
	for (std::set<int>::const_iterator m = ch->second.members.begin(); m != ch->second.members.end(); ++m) {
		std::map<int, Client>::iterator it = _clients.find(*m);
		if (it != _clients.end() && it->second.nick == target) {
			ch->second.members.erase(*m);
			ch->second.operators.erase(*m);
			std::string reason = args.size() > 2 ? " :" + args[2] : "";
			queueMessage(*m, ":" + it->second.nick + "!" + it->second.user + "@localhost KICK " + chName + " " + target + reason);
			return;
		}
	}
	sendNumericReply(c.fd, "441", target + " " + chName + " :They aren't on that channel");
}
