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
	std::string reason = args.empty() ? "Client Quit" : args[0];
	quitClient(c.fd, "Quit: " + reason);
}
