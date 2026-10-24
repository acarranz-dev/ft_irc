/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 11:39:45 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/23 15:43:34 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <string>
#include <set>

class Channel {
public:
    Channel();
    ~Channel();
    Channel(const Channel&);
    Channel& operator=(const Channel&);

    std::set<int> members;
    std::set<int> operators;
    std::set<int> invited;
    std::string topic;
    std::string key;
    bool inviteOnly;
    bool topicOps;
    int userLimit;

    bool hasMember(int fd) const;
    // bool isOperator(int fd) const;
    // bool isInvited(int fd) const;

    // void addMember(int fd);
    // void removeMember(int fd);

    void addOperator(int fd);
    // void removeOperator(int fd);

    // void invite(int fd);
    // void removeInvitation(int fd);

    // bool isFull() const;
};

#endif