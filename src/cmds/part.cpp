/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 18:31:43 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/19 18:31:44 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::cmdPart(Client& c, const std::vector<std::string>& args) {
	if (args.empty())
		return;
	std::string chName = args[0];
	std::map<std::string, Channel>::iterator ch = _channels.find(chName);
	if (ch == _channels.end())
		return;
	ch->second.members.erase(c.fd);
	ch->second.operators.erase(c.fd);
	ch->second.invited.erase(c.fd);
	if (ch->second.members.empty()) {
		std::map<std::string, Channel>::iterator doomed = ch++;
		_channels.erase(doomed);
	}
	else
		++ch;
}
