/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 18:32:15 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/19 18:32:17 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::cmdTopic(Client& c, const std::vector<std::string>& args) {
	if (!c.registered) {
		sendNumericReply(c.fd, "451", ":You have not registered");
		return;
	}
	if (args.empty()) {
		sendNumericReply(c.fd, "461", "TOPIC :Not enough parameters");
		return;
	}
	std::map<std::string, Channel>::iterator ch = _channels.find(args[0]);
	if (ch == _channels.end()) {
		sendNumericReply(c.fd, "403", args[0] + " :No such channel");
		return;
	}
	if (!ch->second.hasMember(c.fd)) {
		sendNumericReply(c.fd, "442", args[0] + " :You're not on that channel");
		return;
	}
	if (args.size() == 1) {
		if (ch->second.topic.empty())
			sendNumericReply(c.fd, "331", args[0] + " :No topic is set");
		else
			sendNumericReply(c.fd, "332", args[0] + " :" + ch->second.topic);
		return;
	}
	if (ch->second.topicOps && !ch->second.isOperator(c.fd)) {
		sendNumericReply(c.fd, "482", args[0] + " :You're not channel operator");
		return;
	}
	ch->second.topic = args[1];
	broadcast(ch->second, prefix(c) + " TOPIC " + args[0] + " :" + args[1], -1);
}
