/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 11:39:45 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/24 12:43:29 by sarodrig         ###   ########.fr       */
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

    bool isOperator(int fd) const;
    bool isInvited(int fd) const;
    void addMember(int fd);
    void removeMember(int fd);
	bool hasMember(int fd) const ;
	void removeOperator(int fd) ;
	void invite(int fd) ;
	void removeInvitation(int fd) ;
	bool isFull() const ;
	bool canJoin(int fd, const std::string& suppliedKey) const ;
	void addOperator(int fd) ;

};

#endif