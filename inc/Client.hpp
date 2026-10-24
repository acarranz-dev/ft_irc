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

class Client {
public:
	Client(int fd = -1);
	~Client();
	Client(const Client&);
	Client& operator=(const Client&);

	int			fd;
	std::string	nick;
	std::string	user;
	std::string	input;
	std::string	output;
	bool		passOK;
	bool		registered;
};

#endif
