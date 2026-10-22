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
	if (args.empty() || args[0].empty()) {
		sendNumericReply(c.fd, "411", ":No recipient given (PRIVMSG)");
		return;
	}
	if (args.size() < 2 || args[1].empty()) {
		sendNumericReply(c.fd, "412", ":No text to send");
		return;
	}
	std::vector<std::string> targets = splitComma(args[0]);

	for (std::vector<std::string>::size_type i = 0; i < targets.size(); ++i) {
		const std::string& target = targets[i];
		std::string msg = prefix(c) + " PRIVMSG " + target + " :" + args[1];
		if (!target.empty() && target[0] == '#') {
			std::map<std::string, Channel>::iterator ch = _channels.find(target);
			if (ch == _channels.end()) {
				sendNumericReply(c.fd, "403", target + " :No such channel");
				continue;
			}
			if (!ch->second.hasMember(c.fd)) {
				sendNumericReply(c.fd, "404", target + " :Cannot send to channel");
				continue;
			}
			broadcast(ch->second, msg, c.fd);
			continue;
		}
		int targetFd = findFdByNick(target);
		if (targetFd == -1)
			sendNumericReply(c.fd, "401", target + " :No such nick/channel");
		else
			queueMessage(targetFd, msg);
	}
}
