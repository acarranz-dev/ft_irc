/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 18:32:08 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/19 18:32:11 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::cmdQuit(Client& c, const std::vector<std::string>& args) {
	std::string msg = args.empty() ? "Client Quit" : args[0];
	std::string prefix = ":" + c.nick + "!" + c.user + "@localhost";
	for (std::map<std::string, Channel>::iterator ch = _channels.begin(); ch != _channels.end(); ++ch) {
		if (!ch->second.members.count(c.fd))
			continue;
		for (std::set<int>::const_iterator m = ch->second.members.begin(); m != ch->second.members.end(); ++m) {
			if (*m != c.fd)
				queueMessage(*m, prefix + " QUIT :" + msg);
		}
	}
	_handleCloseClient(c.fd);
}
