/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 11:41:04 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/24 12:10:26 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(): 
	inviteOnly(false),
	topicOps(false),
	userLimit(0) {
}

Channel::~Channel() {
}

Channel::Channel(const Channel& other): 
	members(other.members),
	operators(other.operators),
	invited(other.invited),
	topic(other.topic),
	key(other.key),
	inviteOnly(other.inviteOnly),
	topicOps(other.topicOps),
	userLimit(other.userLimit) {
}

Channel& Channel::operator=(const Channel& other) {
	if (this != &other) {
		members = other.members;
		operators = other.operators;
		invited = other.invited;
		topic = other.topic;
		key = other.key;
		inviteOnly = other.inviteOnly;
		topicOps = other.topicOps;
		userLimit = other.userLimit;
	}
	return *this;
}

bool Channel::hasMember(int fd) const {
    return members.find(fd) != members.end();
}

void Channel::addOperator(int fd) {
    if (hasMember(fd)) operators.insert(fd);
}
