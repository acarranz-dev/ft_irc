/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acarranz <acarranz@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 12:00:00 by acarranz          #+#    #+#             */
/*   Updated: 2026/10/22 12:00:00 by acarranz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::cmdPing(Client& c, const std::vector<std::string>& args) {
	if (args.empty() || args[0].empty()) {
		sendNumericReply(c.fd, "409", ":No origin specified");
		return;
	}
	queueMessage(c.fd, ":ircserv PONG ircserv :" + args[0]);
}
