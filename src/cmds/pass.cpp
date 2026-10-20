/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 18:31:52 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/19 18:31:54 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::cmdPass(Client& c, const std::vector<std::string>& args) {
	if (c.registered) {
		sendNumericReply(c.fd, "462", ":You may not reregister");
		return;
	}
	if (args.empty()) {
		sendNumericReply(c.fd, "461", "PASS :Not enough parameters");
		return;
	}
	if (args[0] != _password) {
		sendNumericReply(c.fd, "464", ":Password incorrect");
		return;
	}
	c.passOK = true;
}
