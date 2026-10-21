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
	std::string name = args[0];
	if (name.empty() || name[0] != '#') {
		sendNumericReply(c.fd, "476", name + " :Bad Channel Mask");
		return;
	}
	Channel& ch = _channels[name];
	if (ch.members.insert(c.fd).second) {
		if (ch.members.size() == 1)
			ch.operators.insert(c.fd);
	}
	std::string prefix = ":" + c.nick + "!" + c.user + "@localhost";
	for (std::set<int>::const_iterator m = ch.members.begin(); m != ch.members.end(); ++m) {
		if (*m != c.fd)
			queueMessage(*m, prefix + " JOIN " + name);
	}
	if (!ch.topic.empty())
		queueMessage(c.fd, ":ircserv 332 " + c.nick + " " + name + " :" + ch.topic);
	std::ostringstream names;
	for (std::set<int>::const_iterator m = ch.members.begin(); m != ch.members.end(); ++m) {
		std::map<int, Client>::iterator it = _clients.find(*m);
		if (it == _clients.end())
			continue;
		if (m != ch.members.begin())
			names << " ";
		if (ch.operators.count(*m))
			names << "@";
		names << it->second.nick;
	}
	queueMessage(c.fd, ":ircserv 353 " + c.nick + " = " + name + " :" + names.str());
	queueMessage(c.fd, ":ircserv 366 " + c.nick + " " + name + " :End of NAMES list");
}
