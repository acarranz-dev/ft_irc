/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 18:31:43 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/19 18:31:44 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::cmdPart(Client& c, const std::vector<std::string>& args) {
	if (!c.registered) {
		sendNumericReply(c.fd, "451", ":You have not registered");
		return;
	}
	if (args.empty()) {
		sendNumericReply(c.fd, "461", "PART :Not enough parameters");
		return;
	}
	std::string reason = args.size() > 1 ? " :" + args[1] : "";
	std::vector<std::string> names = splitComma(args[0]);

	for (std::vector<std::string>::size_type i = 0; i < names.size(); ++i) {
		std::map<std::string, Channel>::iterator ch = _channels.find(names[i]);
		if (ch == _channels.end()) {
			sendNumericReply(c.fd, "403", names[i] + " :No such channel");
			continue;
		}
		if (!ch->second.hasMember(c.fd)) {
			sendNumericReply(c.fd, "442", names[i] + " :You're not on that channel");
			continue;
		}
		broadcast(ch->second, prefix(c) + " PART " + names[i] + reason, -1);
		ch->second.removeMember(c.fd);
		if (ch->second.members.empty())
			_channels.erase(ch);
	}
}
