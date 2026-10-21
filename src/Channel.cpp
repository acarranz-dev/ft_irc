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

Channel::Channel()
    : inviteOnly(false),
      topicOps(false),
      userLimit(0) {
}

bool Channel::hasMember(int fd) const {
    return members.find(fd) != members.end();
}

bool Channel::isOperator(int fd) const {
    return operators.find(fd) != operators.end();
}

bool Channel::isInvited(int fd) const {
    return invited.find(fd) != invited.end();
}

void Channel::addMember(int fd) {
    members.insert(fd);
}

void Channel::removeMember(int fd) {
    members.erase(fd);
    operators.erase(fd);
    invited.erase(fd);
}

void Channel::addOperator(int fd) {
    if (hasMember(fd)) operators.insert(fd);
}

void Channel::removeOperator(int fd) {
    operators.erase(fd);
}

void Channel::invite(int fd) {
    invited.insert(fd);
}

void Channel::removeInvitation(int fd) {
    invited.erase(fd);
}

bool Channel::isFull() const {
    return (userLimit > 0 && members.size() >= static_cast<std::size_t>(userLimit));
}

bool Channel::canJoin(int fd, const std::string& suppliedKey) const {
    if (hasMember(fd)) return true;
    if (inviteOnly && !isInvited(fd)) return false;
	if (!key.empty() && key != suppliedKey) return false;
	if (isFull()) return false;
    return true;
}
