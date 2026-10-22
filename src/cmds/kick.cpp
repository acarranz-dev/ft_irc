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
	if (!ch->second.hasMember(c.fd)) {
		sendNumericReply(c.fd, "442", chName + " :You're not on that channel");
		return;
	}
	if (!ch->second.isOperator(c.fd)) {
		sendNumericReply(c.fd, "482", chName + " :You're not channel operator");
		return;
	}
	std::string reason = args.size() > 2 ? args[2] : c.nick;
	std::vector<std::string> targets = splitComma(args[1]);

	for (std::vector<std::string>::size_type i = 0; i < targets.size(); ++i) {
		int targetFd = findFdByNick(targets[i]);
		if (targetFd == -1 || !ch->second.hasMember(targetFd)) {
			sendNumericReply(c.fd, "441", targets[i] + " " + chName + " :They aren't on that channel");
			continue;
		}
		broadcast(ch->second, prefix(c) + " KICK " + chName + " " + targets[i] + " :" + reason, -1);
		ch->second.removeMember(targetFd);
	}
	if (ch->second.members.empty())
		_channels.erase(ch);
}
