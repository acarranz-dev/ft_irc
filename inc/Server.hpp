/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sarodrig <sarodrig@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/02 22:08:08 by sarodrig         #+#    #+#             */
/*   Updated: 2026/10/02 22:08:08 by sarodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
//acrranz 

#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <map>
# include <set>
# include <vector>
# include <poll.h>

struct Client {
	Client();
	int		fd;

	std::string	nick;
	std::string	user;
	std::string	input;
	std::string	output;
	bool		passOK;
	bool		registered;
};

struct Channel {
	Channel();

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
	std::string				_password;
	int						_socketFd;
	bool					_running;
	std::map<int, Client>	_clients;
	std::map<std::string, Channel> _channels;
	std::vector<pollfd>		_pollFds;

	void	setNonBlocking(int fd); 
	void	addPollFd(int fd, short events);
	void	removePollFd(int fd);
	void	updatePollFd(int fd);
	void	_handleNewClient();
	void	_handleReadClient(int fd);
	void	_handleWriteClient(int fd);
	void	_handleCloseClient(int fd);
	void	processInput(int fd);
	void	handleCommand(int fd, const std::string& line);
	void	_sendNumericReply(int fd, const std::string& code, const std::string& msg);

	void	queueMessage(int fd, const std::string& message);

	void	cmdPass(Client&, const std::vector<std::string>&);
	void	cmdNick(Client&, const std::vector<std::string>&);
	void	cmdUser(Client&, const std::vector<std::string>&);
	void	cmdJoin(Client&, const std::vector<std::string>&);
	void	cmdKick(Client&, const std::vector<std::string>&);
	void	cmdInvite(Client&, const std::vector<std::string>&);
	void	cmdTopic(Client&, const std::vector<std::string>&);
	void	cmdPart(Client&, const std::vector<std::string>&);
	void	cmdMode(Client&, const std::vector<std::string>&);
	void	cmdPrivmsg(Client&, const std::vector<std::string>&);
	void	cmdQuit(Client&, const std::vector<std::string>&);

};

#endif
