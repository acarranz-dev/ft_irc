/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 18:32:21 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/19 18:32:23 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::cmdUser(Client& c, const std::vector<std::string>& args) {
	if (args.size() < 4) {
		_sendNumericReply(c.fd, "461", "USER :Not enough parameters");
		return;
	}
	c.user = args[0];
}
