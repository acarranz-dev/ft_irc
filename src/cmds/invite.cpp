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
		_sendNumericReply(c.fd, "451", ":You have not registered");
		return;
	}
	if (args.size() < 2) {
		_sendNumericReply(c.fd, "461", "INVITE :Not enough parameters");
		return;
	}
	std::string target = args[0];
	std::string chName = args[1];
	std::map<std::string, Channel>::iterator ch = _channels.find(chName);
	if (ch == _channels.end()) {
		_sendNumericReply(c.fd, "403", chName + " :No such channel");
		return;
	}
	if (!ch->second.operators.count(c.fd)) {
		_sendNumericReply(c.fd, "482", chName + " :You are not channel operator");
		return;
	}
	for (std::map<int, Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second.nick != target)
			continue;
		ch->second.invited.insert(it->first);
		queueMessage(it->first, ":" + c.nick + "!" + c.user + "@localhost INVITE " + target + " " + chName);
		return;
	}
	_sendNumericReply(c.fd, "401", target + " :No such nick/channel");
}
