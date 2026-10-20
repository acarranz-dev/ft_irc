/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 11:39:07 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/20 19:32:43 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <set>

class Client {
public:
    int fd;

    std::string input;
    std::string out;

    std::string nick;
    std::string user;
    std::string real;

    bool pass;
    bool registered;

    std::set<std::string> channels;

    explicit Client(int socketFd);

    std::string mask() const;
    bool hasChannel(const std::string& channel) const;
    void addChannel(const std::string& channel);
    void removeChannel(const std::string& channel);
};

#endif
