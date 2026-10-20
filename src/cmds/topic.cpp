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
	if (args.empty()) {
		_sendNumericReply(c.fd, "461", "TOPIC :Not enough parameters");
		return;
	}
	std::map<std::string, Channel>::iterator ch = _channels.find(args[0]);
	if (ch == _channels.end()) {
		_sendNumericReply(c.fd, "403", args[0] + " :No such channel");
		return;
	}
	if (args.size() == 1) {
		if (ch->second.topic.empty())
			_sendNumericReply(c.fd, "331", args[0] + " :No topic is set");
		else
			_sendNumericReply(c.fd, "332", args[0] + " :" + ch->second.topic);
		return;
	}
	ch->second.topic = args[1];
}
