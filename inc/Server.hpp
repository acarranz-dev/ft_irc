/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/02 22:08:08 by sarodrig          #+#    #+#             */
/*   Updated: 2026/10/02 22:08:08 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <map>
# include <set>
# include <vector>
# include <poll.h>

struct Client {
	std::string	nick;
	std::string	user;
	std::string	input;
	std::string	output;
	bool		passOK;
	bool		registered;
};

struct Channel {
	std::set<int>	members;
	std::set<int>	operators;
	std::set<int>	invited;
	std::string		topic;
	std::string		key;
	bool			inviteOnly;
	bool			topicOps;
	int				userLimit;
};

class Server {
	public:
		Server(int port, const std::string& password);
		~Server();

	void	init();
	void	run();

private:
	// Server(const Server&);
	// Server& operator=(const Server&);

	int						_port;
	int						_serverFd;
	std::string				_password;
	bool					_running;
	std::map<int, Client>	_clients;
	std::map<std::string, Channel> _channels;
	std::vector<pollfd>		_pollFds;

	// void	setNonBlocking(int fd);
	// void	addPollFd(int fd, short events);
	// void	removePollFd(int fd);
	// void	acceptClient();
	// void	readClient(int fd);
	// void	writeClient(int fd);
	// void	closeClient(int fd);
	// void	processInput(int fd);
	// void	handleCommand(int fd, const std::string& line);
	// void	queueMessage(int fd, const std::string& message);
	// void	updatePollEvents(int fd);

};

#endif
