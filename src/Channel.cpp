/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 11:41:04 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/19 18:32:28 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(const std::string& channelName)
    : name(channelName),
      inviteOnly(false),
      topicOps(false),
      limit(0) {
}

bool Channel::hasMember(int fd) const {
    return members.find(fd) != members.end();
}

bool Channel::isOperator(int fd) const {
    return ops.find(fd) != ops.end();
}

bool Channel::isInvited(int fd) const {
    return invited.find(fd) != invited.end();
}

void Channel::addMember(int fd) {
    members.insert(fd);
}

void Channel::removeMember(int fd) {
    members.erase(fd);
    ops.erase(fd);
    invited.erase(fd);
}

void Channel::addOperator(int fd) {
    if (hasMember(fd)) ops.insert(fd);
}

void Channel::removeOperator(int fd) {
    ops.erase(fd);
}

void Channel::invite(int fd) {
    invited.insert(fd);
}

void Channel::removeInvitation(int fd) {
    invited.erase(fd);
}

bool Channel::isFull() const {
    return (limit > 0 && members.size() >= static_cast<std::size_t>(limit));
}

bool Channel::canJoin(int fd, const std::string& suppliedKey) const {
    if (hasMember(fd)) return true;
    if (inviteOnly && !isInvited(fd)) return false;
	if (!key.empty() && key != suppliedKey) return false;
	if (isFull()) return false;
    return true;
}
