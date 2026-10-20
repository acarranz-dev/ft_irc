/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 11:41:21 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/19 16:56:44 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Client.hpp"

Client::Client(int socketFd)
    : fd(socketFd),
      pass(false),
      registered(false) {
}

std::string Client::mask() const {
    if (nick.empty()) return "*";

    return nick;
}

bool Client::hasChannel(const std::string& channel) const {
    return channels.find(channel) != channels.end();
}

void Client::addChannel(const std::string& channel) {
    channels.insert(channel);
}

void Client::removeChannel(const std::string& channel) {
    channels.erase(channel);
}
