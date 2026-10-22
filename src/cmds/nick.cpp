/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 18:31:34 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/19 18:31:37 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <set>

void Server::cmdNick(Client& c, const std::vector<std::string>& args) {
	if (args.empty() || args[0].empty()) {
		sendNumericReply(c.fd, "431", ":No nickname given");
		return;
	}
	const std::string& nick = args[0];
	if (nick[0] == '#' || nick[0] == ':' || nick.find_first_of(" ,*?!@") != std::string::npos) {
		sendNumericReply(c.fd, "432", nick + " :Erroneous nickname");
		return;
	}
	int owner = findFdByNick(nick);
	if (owner != -1 && owner != c.fd) {
		sendNumericReply(c.fd, "433", nick + " :Nickname is already in use");
		return;
	}
	if (c.registered && nick != c.nick) {
		/* Notify the client itself and everyone sharing a channel, once each */
		std::set<int> peers;
		peers.insert(c.fd);
		for (std::map<std::string, Channel>::iterator ch = _channels.begin(); ch != _channels.end(); ++ch) {
			if (ch->second.hasMember(c.fd))
				peers.insert(ch->second.members.begin(), ch->second.members.end());
		}
		std::string msg = prefix(c) + " NICK :" + nick;
		for (std::set<int>::const_iterator p = peers.begin(); p != peers.end(); ++p)
			queueMessage(*p, msg);
	}
	c.nick = nick;
}
