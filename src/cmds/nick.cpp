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

void Server::cmdNick(Client& c, const std::vector<std::string>& args) {
	if (args.empty() || args[0].empty()) {
		sendNumericReply(c.fd, "431", ":No nickname given");
		return;
	}
	for (std::map<int, Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->first != c.fd && it->second.nick == args[0]) {
			sendNumericReply(c.fd, "433", args[0] + " :Nickname is already in use");
			return;
		}
	}
	c.nick = args[0];
}
