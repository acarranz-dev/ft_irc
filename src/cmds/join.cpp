/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 18:30:58 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/19 18:31:01 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sstream>

void Server::cmdJoin(Client& c, const std::vector<std::string>& args) {
	if (!c.registered) {
		sendNumericReply(c.fd, "451", ":You have not registered");
		return;
	}
	if (args.empty()) {
		sendNumericReply(c.fd, "461", "JOIN :Not enough parameters");
		return;
	}
	std::vector<std::string> names = splitComma(args[0]);
	std::vector<std::string> keys;
	if (args.size() > 1)
		keys = splitComma(args[1]);

	for (std::vector<std::string>::size_type i = 0; i < names.size(); ++i) {
		std::string name = names[i];
		std::string key = i < keys.size() ? keys[i] : "";
		if (name.size() < 2 || name[0] != '#') {
			sendNumericReply(c.fd, "476", name + " :Bad Channel Mask");
			continue;
		}
		std::map<std::string, Channel>::iterator found = _channels.find(name);
		if (found != _channels.end()) {
			Channel& ex = found->second;
			if (ex.hasMember(c.fd))
				continue;
			if (ex.inviteOnly && !ex.isInvited(c.fd)) {
				sendNumericReply(c.fd, "473", name + " :Cannot join channel (+i)");
				continue;
			}
			if (!ex.key.empty() && ex.key != key) {
				sendNumericReply(c.fd, "475", name + " :Cannot join channel (+k)");
				continue;
			}
			if (ex.isFull()) {
				sendNumericReply(c.fd, "471", name + " :Cannot join channel (+l)");
				continue;
			}
		}
		Channel& ch = _channels[name];
		ch.addMember(c.fd);
		ch.removeInvitation(c.fd);
		if (ch.members.size() == 1)
			ch.addOperator(c.fd);

		broadcast(ch, prefix(c) + " JOIN " + name, -1);
		if (!ch.topic.empty())
			sendNumericReply(c.fd, "332", name + " :" + ch.topic);
		std::ostringstream list;
		for (std::set<int>::const_iterator m = ch.members.begin(); m != ch.members.end(); ++m) {
			std::map<int, Client>::iterator it = _clients.find(*m);
			if (it == _clients.end())
				continue;
			if (m != ch.members.begin())
				list << " ";
			if (ch.isOperator(*m))
				list << "@";
			list << it->second.nick;
		}
		sendNumericReply(c.fd, "353", "= " + name + " :" + list.str());
		sendNumericReply(c.fd, "366", name + " :End of NAMES list");
	}
}
