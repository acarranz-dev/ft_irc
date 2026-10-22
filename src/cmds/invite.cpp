/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 18:30:49 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/19 18:30:52 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::cmdInvite(Client& c, const std::vector<std::string>& args) {
	if (!c.registered) {
		sendNumericReply(c.fd, "451", ":You have not registered");
		return;
	}
	if (args.size() < 2) {
		sendNumericReply(c.fd, "461", "INVITE :Not enough parameters");
		return;
	}
	std::string target = args[0];
	std::string chName = args[1];
	int targetFd = findFdByNick(target);
	if (targetFd == -1) {
		sendNumericReply(c.fd, "401", target + " :No such nick/channel");
		return;
	}
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
	if (ch->second.hasMember(targetFd)) {
		sendNumericReply(c.fd, "443", target + " " + chName + " :is already on channel");
		return;
	}
	ch->second.invite(targetFd);
	sendNumericReply(c.fd, "341", target + " " + chName);
	queueMessage(targetFd, prefix(c) + " INVITE " + target + " :" + chName);
}
